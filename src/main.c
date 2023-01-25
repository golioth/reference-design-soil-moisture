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

#include "dfu/app_dfu.h"
#include "app_work.h"
#include "app_rpc.h"
#include "app_settings.h"
#include "app_state.h"

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



static void golioth_on_connect(struct golioth_client *client)
{
	k_sem_give(&connected);

	/**
	 * Workaround for duplicate observations. This will eventually be addressed
	 * in the Golioth SDK and iteration/conditional can be removed
	 **/
	static int iteration = 0;
	if (iteration > 0) {
		LOG_INF("Registering observations with Golioth");
		app_dfu_observe();
		app_register_settings(client);
		app_register_rpc(client);
		app_state_observe();
	}
	++iteration;
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

void wake_system_thread(void) {
	k_wakeup(_system_thread);
}

void main(void)
{
	int err;

	LOG_INF("Start Golioth Soil Moisture Monitor sample, FW ver %s",CONFIG_MCUBOOT_IMAGE_VERSION);

	/* Get system thread id so loop delay change event can wake main */
	_system_thread = k_current_get();

	/* Initialize Golioth logo LED */
	err = gpio_pin_configure_dt(&golioth_led, GPIO_OUTPUT_INACTIVE);
	if (err) {
		LOG_ERR("Unable to configure LED for Golioth Logo");
	}

	/* Initialize app state */
	app_state_init(client);

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

		k_sleep(K_SECONDS(get_loop_delay_s()));
	}
}
