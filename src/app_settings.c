/*
 * Copyright (c) 2022-2023 Golioth, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(app_settings, LOG_LEVEL_DBG);

#include <net/golioth/settings.h>

#include "main.h"
#include "app_settings.h"

static struct golioth_client *client;

#define MAX_MOISTURE_VALUE 5000

static int32_t _loop_delay_s = 60;
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
		return _MOISTURE_LEVEL_0;
		break;

	default:
		LOG_ERR("Passed invalid value to get_moisture_level() function");
		return -ENOTSUP;
		break;
	}
}

enum golioth_settings_status on_setting(const char *key,
					const struct golioth_settings_value *value)
{

	LOG_DBG("Received setting: key = %s, type = %d", key, value->type);
	if (strcmp(key, "LOOP_DELAY_S") == 0) {
		/* This setting is expected to be numeric, return an error if it's not */
		if (value->type != GOLIOTH_SETTINGS_VALUE_TYPE_INT64) {
			return GOLIOTH_SETTINGS_VALUE_FORMAT_NOT_VALID;
		}

		/* Limit to 12 hour max delay: [1, 43200] */
		if (value->i64 < 1 || value->i64 > 43200) {
			return GOLIOTH_SETTINGS_VALUE_OUTSIDE_RANGE;
		}

		/* Only update if value has changed */
		if (_loop_delay_s == (int32_t)value->i64) {
			LOG_DBG("Received LOOP_DELAY_S already matches local value.");
		} else {
			_loop_delay_s = (int32_t)value->i64;
			LOG_INF("Set loop delay to %d seconds", _loop_delay_s);

			wake_system_thread();
		}
		return GOLIOTH_SETTINGS_SUCCESS;
	}

	if (strcmp(key, "MOISTURE_LEVEL_0") == 0) {
		/* This setting is expected to be numeric, return an error if it's not */
		if (value->type != GOLIOTH_SETTINGS_VALUE_TYPE_INT64) {
			return GOLIOTH_SETTINGS_VALUE_FORMAT_NOT_VALID;
		}

		/* Limit to MAX_MOISTURE_VALUE */
		if (value->i64 < 1 || value->i64 > MAX_MOISTURE_VALUE) {
			return GOLIOTH_SETTINGS_VALUE_OUTSIDE_RANGE;
		}

		/* Only update if value has changed */
		if (_MOISTURE_LEVEL_0 == (int32_t)value->i64) {
			LOG_DBG("Received MOISTURE_LEVEL_0 already matches local value.");
		} else {
			_MOISTURE_LEVEL_0 = (int32_t)value->i64;
			LOG_INF("Set Moisture Level 0 to %d", _MOISTURE_LEVEL_0);

			wake_system_thread();
		}
		return GOLIOTH_SETTINGS_SUCCESS;
	}

	if (strcmp(key, "MOISTURE_LEVEL_20") == 0) {
		/* This setting is expected to be numeric, return an error if it's not */
		if (value->type != GOLIOTH_SETTINGS_VALUE_TYPE_INT64) {
			return GOLIOTH_SETTINGS_VALUE_FORMAT_NOT_VALID;
		}

		/* Limit to MAX_MOISTURE_VALUE */
		if (value->i64 < 1 || value->i64 > MAX_MOISTURE_VALUE) {
			return GOLIOTH_SETTINGS_VALUE_OUTSIDE_RANGE;
		}

		/* Only update if value has changed */
		if (_MOISTURE_LEVEL_20 == (int32_t)value->i64) {
			LOG_DBG("Received MOISTURE_LEVEL_20 already matches local value.");
		} else {
			_MOISTURE_LEVEL_20 = (int32_t)value->i64;
			LOG_INF("Set Moisture Level 20 to %d", _MOISTURE_LEVEL_20);

			wake_system_thread();
		}
		return GOLIOTH_SETTINGS_SUCCESS;
	}

	if (strcmp(key, "MOISTURE_LEVEL_40") == 0) {
		/* This setting is expected to be numeric, return an error if it's not */
		if (value->type != GOLIOTH_SETTINGS_VALUE_TYPE_INT64) {
			return GOLIOTH_SETTINGS_VALUE_FORMAT_NOT_VALID;
		}

		/* Limit to MAX_MOISTURE_VALUE */
		if (value->i64 < 1 || value->i64 > MAX_MOISTURE_VALUE) {
			return GOLIOTH_SETTINGS_VALUE_OUTSIDE_RANGE;
		}

		/* Only update if value has changed */
		if (_MOISTURE_LEVEL_40 == (int32_t)value->i64) {
			LOG_DBG("Received MOISTURE_LEVEL_40 already matches local value.");
		} else {
			_MOISTURE_LEVEL_40 = (int32_t)value->i64;
			LOG_INF("Set Moisture Level 40 to %d", _MOISTURE_LEVEL_40);

			wake_system_thread();
		}
		return GOLIOTH_SETTINGS_SUCCESS;
	}

	if (strcmp(key, "MOISTURE_LEVEL_60") == 0) {
		/* This setting is expected to be numeric, return an error if it's not */
		if (value->type != GOLIOTH_SETTINGS_VALUE_TYPE_INT64) {
			return GOLIOTH_SETTINGS_VALUE_FORMAT_NOT_VALID;
		}

		/* Limit to MAX_MOISTURE_VALUE */
		if (value->i64 < 1 || value->i64 > MAX_MOISTURE_VALUE) {
			return GOLIOTH_SETTINGS_VALUE_OUTSIDE_RANGE;
		}

		/* Only update if value has changed */
		if (_MOISTURE_LEVEL_60 == (int32_t)value->i64) {
			LOG_DBG("Received MOISTURE_LEVEL_60 already matches local value.");
		} else {
			_MOISTURE_LEVEL_60 = (int32_t)value->i64;
			LOG_INF("Set Moisture Level 60 to %d", _MOISTURE_LEVEL_60);

			wake_system_thread();
		}
		return GOLIOTH_SETTINGS_SUCCESS;
	}

	if (strcmp(key, "MOISTURE_LEVEL_80") == 0) {
		/* This setting is expected to be numeric, return an error if it's not */
		if (value->type != GOLIOTH_SETTINGS_VALUE_TYPE_INT64) {
			return GOLIOTH_SETTINGS_VALUE_FORMAT_NOT_VALID;
		}

		/* Limit to MAX_MOISTURE_VALUE */
		if (value->i64 < 1 || value->i64 > MAX_MOISTURE_VALUE) {
			return GOLIOTH_SETTINGS_VALUE_OUTSIDE_RANGE;
		}

		/* Only update if value has changed */
		if (_MOISTURE_LEVEL_80 == (int32_t)value->i64) {
			LOG_DBG("Received MOISTURE_LEVEL_80 already matches local value.");
		} else {
			_MOISTURE_LEVEL_80 = (int32_t)value->i64;
			LOG_INF("Set Moisture Level 80 to %d", _MOISTURE_LEVEL_80);

			wake_system_thread();
		}
		return GOLIOTH_SETTINGS_SUCCESS;
	}

	/* If the setting is not recognized, we should return an error */
	return GOLIOTH_SETTINGS_KEY_NOT_RECOGNIZED;
}

int app_settings_init(struct golioth_client *state_client)
{
	client = state_client;
	int err = app_settings_register(client);

	return err;
}

int app_settings_observe(void)
{
	int err = golioth_settings_observe(client);
	if (err) {
		LOG_ERR("Failed to observe settings: %d", err);
	}

	return err;
}

int app_settings_register(struct golioth_client *settings_client)
{
	int err = golioth_settings_register_callback(settings_client, on_setting);

	if (err) {
		LOG_ERR("Failed to register settings callback: %d", err);
	}

	return err;
}
