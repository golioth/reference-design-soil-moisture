/*
 * Copyright (c) 2022-2023 Golioth, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/** The `app_work.c` file performs the important work of this application which
 * is to read sensor values and report them to the Golioth LightDB Stream as
 * time-series data.
 *
 * https://docs.golioth.io/firmware/zephyr-device-sdk/light-db-stream/
 */

#ifndef __APP_WORK_H__
#define __APP_WORK_H__

#include <net/golioth/system_client.h>

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

void app_work_init(struct golioth_client *work_client);
void app_work_sensor_read(void);
void sensor_init(void);

#endif /* __APP_WORK_H__ */
