/*
 * Copyright (c) 2022 Golioth, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __APP_WORK_H__
#define __APP_WORK_H__

void app_work_init(struct golioth_client *work_client);
void app_work_sensor_read(void);
void sensor_init(void);

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
} slide_key;

// /* Ostentus slide labels */
#define SLIDESHOW_TITLE "Soil Moisture"
#define M_READING_LABEL "Moisture Raw"
#define M_LEVEL_LABEL "Moisture Lvl"
#define M_LIGHT_INT_LABEL "Light Lvl"
#define M_TEMP_LABEL "Temperature"
#define M_PRESSURE_LABEL "Pressure"
#define M_HUMIDITY_LABEL "Humidity"

#endif /* __APP_WORK_H__ */
