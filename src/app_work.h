/*
 * Copyright (c) 2022 Golioth, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __APP_WORK_H__
#define __APP_WORK_H__

void app_work_init(struct golioth_client* work_client);
void app_work_sensor_read(void);
void sensor_init(void);

/**
 * Each Ostentus slide needs a unique key. You may add additional slides by
 * inserting elements with the name of your choice to this enum.
 */
typedef enum {
    MOISTURE_TEMP,
    MOISTURE_LIGHT_INT,
    MOISTURE_READING_KEY,
    MOISTURE_LEVEL_KEY,    
}slide_key;

// /* Ostentus slide labels */
#define SLIDESHOW_TITLE "Soil Moisture"
#define M_READING_LABEL "Moisture Reading:"
#define M_LEVEL_LABEL "Moisture Level:"
#define M_LIGHT_INT_LABEL "Light Level:"
#define M_TEMP_LABEL "Temp Level:"

#endif /* __APP_WORK_H__ */
