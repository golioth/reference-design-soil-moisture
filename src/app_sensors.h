/*
 * Copyright (c) 2022-2023 Golioth, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __APP_SENSORS_H__
#define __APP_SENSORS_H__

/** The `app_sensors.c` file performs the important work of this application
 * which is to read sensor values and report them to the Golioth LightDB Stream
 * as time-series data.
 *
 * For this demonstration, a `counter` value is periodically logged and pushed
 * to the Golioth time-series database. This simulated sensor reading occurs
 * when the loop in `main.c` calls `app_sensors_read_and_stream()`. The
 * frequency of this loop is determined by values received from the Golioth
 * Settings Service (see app_settings.h).
 *
 * https://docs.golioth.io/firmware/zephyr-device-sdk/light-db-stream/
 */

#include <golioth/client.h>

void app_sensors_set_client(struct golioth_client *sensors_client);
void app_sensors_read_and_stream(void);
void sensor_init(void);

/* Ostentus slide labels */
#define M_READING_LABEL         "Moisture Raw"
#define M_LEVEL_LABEL           "Moisture Lvl"
#define M_LIGHT_INT_LABEL       "Light Lvl"
#define M_TEMP_LABEL            "Temperature"
#define M_PRESSURE_LABEL        "Pressure"
#define M_HUMIDITY_LABEL        "Humidity"
#define LABEL_BATTERY		"Battery"
#define LABEL_FIRMWARE		"Firmware"
#define SUMMARY_TITLE		"Soil Moisture"

/**
 * Each Ostentus slide needs a unique key. You may add additional slides by
 * inserting elements with the name of your choice to this enum.
 */
typedef enum {
	MOISTURE_READING_KEY,
	MOISTURE_LEVEL_KEY,
	MOISTURE_LIGHT_INT,
	TEMPERATURE,
	PRESSURE,
	HUMIDITY,
#ifdef CONFIG_ALUDEL_BATTERY_MONITOR
	BATTERY_V,
	BATTERY_LVL,
#endif
	FIRMWARE
} slide_key;

#endif /* __APP_SENSORS_H__ */
