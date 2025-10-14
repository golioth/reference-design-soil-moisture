/*
 * Copyright (c) 2022-2023 Golioth, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(app_settings, LOG_LEVEL_DBG);

#include <golioth/client.h>
#include <golioth/settings.h>
#include "main.h"
#include "app_settings.h"

static int32_t _loop_delay_s = 60;
#define LOOP_DELAY_S_MAX 43200
#define LOOP_DELAY_S_MIN 1

#define MIN_MOISTURE_VALUE 1
#define MAX_MOISTURE_VALUE 5000

static int32_t _MOISTURE_LEVEL_0 = 3400;
static int32_t _MOISTURE_LEVEL_20 = 3200;
static int32_t _MOISTURE_LEVEL_40 = 3000;
static int32_t _MOISTURE_LEVEL_60 = 2800;
static int32_t _MOISTURE_LEVEL_80 = 2600;

int32_t get_loop_delay_s(void)
{
	return _loop_delay_s;
}

int32_t get_moisture_level_threshold(uint32_t moisture_threshold)
{
	switch (moisture_threshold) {
	case 0:
		return _MOISTURE_LEVEL_0;
		break;

	case 20:
		return _MOISTURE_LEVEL_20;
		break;

	case 40:
		return _MOISTURE_LEVEL_40;
		break;

	case 60:
		return _MOISTURE_LEVEL_60;
		break;

	case 80:
		return _MOISTURE_LEVEL_80;
		break;

	default:
		LOG_ERR("Passed invalid value to get_moisture_level() function");
		return -ENOTSUP;
		break;
	}
}

static enum golioth_settings_status on_loop_delay_setting(int32_t new_value, void *arg)
{
	_loop_delay_s = new_value;
	LOG_INF("Set loop delay to %i seconds", new_value);
	wake_system_thread();
	return GOLIOTH_SETTINGS_SUCCESS;
}

static enum golioth_settings_status on_moisture_level_setting(int32_t new_value, void *arg)
{
	intptr_t m_level = (intptr_t) arg;
	int32_t *stored_value;

	switch (m_level) {
		case 0:
			stored_value = &_MOISTURE_LEVEL_0;
			break;
		case 20:
			stored_value = &_MOISTURE_LEVEL_20;
			break;
		case 40:
			stored_value = &_MOISTURE_LEVEL_40;
			break;
		case 60:
			stored_value = &_MOISTURE_LEVEL_60;
			break;
		case 80:
			stored_value = &_MOISTURE_LEVEL_80;
			break;
		default:
			return GOLIOTH_SETTINGS_GENERAL_ERROR;

	}

	/* Only update if value has changed */
	if (*stored_value == new_value) {
		LOG_DBG("Received MOISTURE_LEVEL_%li already matches local value.", m_level);
	} else {
		*stored_value = new_value;
		LOG_INF("Set Moisture Level %li to %d", m_level, *stored_value);
		wake_system_thread();
	}

	return GOLIOTH_SETTINGS_SUCCESS;
}

int app_settings_register(struct golioth_client *client)
{
	struct golioth_settings *settings = golioth_settings_init(client);
	int err;

	err = golioth_settings_register_int_with_range(settings,
						       "LOOP_DELAY_S",
						       LOOP_DELAY_S_MIN,
						       LOOP_DELAY_S_MAX,
						       on_loop_delay_setting,
						       NULL);

	if (err) {
		LOG_ERR("Failed to register LOOP_DELAY_S settings callback: %d", err);
	}

	err = golioth_settings_register_int_with_range(settings,
						       "MOISTURE_LEVEL_0",
						       MIN_MOISTURE_VALUE,
						       MAX_MOISTURE_VALUE,
						       on_moisture_level_setting,
						       (void *) 0);

	if (err) {
		LOG_ERR("Failed to register MOISTURE_LEVEL_0 settings callback: %d", err);
	}

	err = golioth_settings_register_int_with_range(settings,
						       "MOISTURE_LEVEL_20",
						       MIN_MOISTURE_VALUE,
						       MAX_MOISTURE_VALUE,
						       on_moisture_level_setting,
						       (void *) 20);

	if (err) {
		LOG_ERR("Failed to register MOISTURE_LEVEL_20 settings callback: %d", err);
	}

	err = golioth_settings_register_int_with_range(settings,
						       "MOISTURE_LEVEL_40",
						       MIN_MOISTURE_VALUE,
						       MAX_MOISTURE_VALUE,
						       on_moisture_level_setting,
						       (void *) 40);

	if (err) {
		LOG_ERR("Failed to register MOISTURE_LEVEL_40 settings callback: %d", err);
	}

	err = golioth_settings_register_int_with_range(settings,
						       "MOISTURE_LEVEL_60",
						       MIN_MOISTURE_VALUE,
						       MAX_MOISTURE_VALUE,
						       on_moisture_level_setting,
						       (void *) 60);

	if (err) {
		LOG_ERR("Failed to register MOISTURE_LEVEL_60 settings callback: %d", err);
	}

	err = golioth_settings_register_int_with_range(settings,
						       "MOISTURE_LEVEL_80",
						       MIN_MOISTURE_VALUE,
						       MAX_MOISTURE_VALUE,
						       on_moisture_level_setting,
						       (void *) 80);

	if (err) {
		LOG_ERR("Failed to register MOISTURE_LEVEL_80 settings callback: %d", err);
	}

	return 0;
}
