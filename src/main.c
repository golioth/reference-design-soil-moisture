/*
 * Copyright (c) 2022 Golioth, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(golioth_soil_moisture, LOG_LEVEL_DBG);

#include <net/golioth/fw.h>
#include <net/golioth/settings.h>
#include <net/golioth/system_client.h>
#include <samples/common/net_connect.h>
#include <zephyr/net/coap.h>
#include "app_dfu.h"
#include "app_work.h"

#include <zephyr/drivers/gpio.h>

#define DEBOUNCE_TIMEOUT_MS 100
static uint64_t last_time = 0;

static struct golioth_client *client = GOLIOTH_SYSTEM_CLIENT_GET();

K_SEM_DEFINE(connected, 0, 1);

static int32_t _loop_delay_s = 60;
static k_tid_t _system_thread = 0;

static const struct gpio_dt_spec golioth_led = GPIO_DT_SPEC_GET(
		DT_ALIAS(golioth_led), gpios);
static const struct gpio_dt_spec user_btn = GPIO_DT_SPEC_GET(
		DT_ALIAS(sw1), gpios);
static struct gpio_callback button_cb_data;

enum golioth_settings_status on_setting(
		const char *key,
		const struct golioth_settings_value *value)
{
	LOG_DBG("Received setting: key = %s, type = %d", key, value->type);
	if (strcmp(key, "LOOP_DELAY_S") == 0) {
		/* This setting is expected to be numeric, return an error if it's not */
		if (value->type != GOLIOTH_SETTINGS_VALUE_TYPE_INT64) {
			return GOLIOTH_SETTINGS_VALUE_FORMAT_NOT_VALID;
		}

		/* This setting must be in range [1, 100], return an error if it's not */
		if (value->i64 < 1 || value->i64 > 100) {
			return GOLIOTH_SETTINGS_VALUE_OUTSIDE_RANGE;
		}

		/* Setting has passed all checks, so apply it to the loop delay */
		_loop_delay_s = (int32_t)value->i64;
		LOG_INF("Set loop delay to %d seconds", _loop_delay_s);

		k_wakeup(_system_thread);
		return GOLIOTH_SETTINGS_SUCCESS;
	}

	/* If the setting is not recognized, we should return an error */
	return GOLIOTH_SETTINGS_KEY_NOT_RECOGNIZED;
}

static void golioth_on_connect(struct golioth_client *client)
{
	k_sem_give(&connected);

	app_dfu_observe();

	int err = golioth_settings_register_callback(client, on_setting);

	if (err) {
		LOG_ERR("Failed to register settings callback: %d", err);
	}
}

void button_pressed(const struct device *dev, struct gpio_callback *cb,
					uint32_t pins)
{
	// Software timer debounce

	uint64_t now = k_uptime_get();
	LOG_DBG("Now: %d",now);
	// printk("Now is %lld, last time is %lld\n", now, last_time); // debug debounce
	if ((now - last_time) > DEBOUNCE_TIMEOUT_MS)
	{
		LOG_DBG("Now: %d, Last time: %d, Difference: %d", now, last_time, (now-last_time));
		k_wakeup(_system_thread);
	}
	last_time = now;

}

void main(void)
{
	int err;

	LOG_DBG("Start Golioth Soil Moisture Monitor sample");

	/* Get system thread id so loop delay change event can wake main */
	_system_thread = k_current_get();

	/* Initialize Golioth logo LED */
	err = gpio_pin_configure_dt(&golioth_led, GPIO_OUTPUT_INACTIVE);
	if (err) {
		LOG_ERR("Unable to configure LED for Golioth Logo");
	}

	/* Initialize app work */
	app_work_init(client);

	/* Initialize DFU components */
	app_dfu_init(client);

		/*Initialize sensors using sensor subsystem*/
	sensor_init();

	/* Run WiFi/DHCP if necessary */
	if (IS_ENABLED(CONFIG_GOLIOTH_SAMPLES_COMMON)) {
		net_connect();
	}

	/* Start Golioth client */
	client->on_connect = golioth_on_connect;
	golioth_system_client_start();

	/* Block until connected to Golioth */
	k_sem_take(&connected, K_FOREVER);
	/* Turn on Golioth logo LED once connected */
	gpio_pin_set_dt(&golioth_led, 1);

	/* Report current DFU version to Golioth */
	app_dfu_report_state_to_golioth();

	/* Set up user button */
	err = gpio_pin_configure_dt(&user_btn, GPIO_INPUT);
	if (err != 0) {
		printk("Error %d: failed to configure %s pin %d\n",
				err, user_btn.port->name, user_btn.pin);
		return;
	}

	err = gpio_pin_interrupt_configure_dt(&user_btn,
	                                      GPIO_INT_EDGE_TO_ACTIVE);
	if (err != 0) {
		printk("Error %d: failed to configure interrupt on %s pin %d\n",
				err, user_btn.port->name, user_btn.pin);
		return;
	}

	gpio_init_callback(&button_cb_data, button_pressed, BIT(user_btn.pin));
	gpio_add_callback(user_btn.port, &button_cb_data);

	while (true) {
		app_work_submit();

		k_sleep(K_SECONDS(_loop_delay_s));
	}
}
