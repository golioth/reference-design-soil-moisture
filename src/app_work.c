/*
 * Copyright (c) 2022 Golioth, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(app_work, LOG_LEVEL_DBG);

#include <net/golioth/system_client.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>
#include <drivers/i2c.h>

#include "app_work.h"

#include "app_work.h"
#include "libostentus/libostentus.h"

static struct golioth_client *client;

/* Sensor structs */

struct device *imu_sensor;
struct device *weather_sensor;

/* Callback for LightDB Stream */
static int async_error_handler(struct golioth_req_rsp *rsp) {
	if (rsp->err) {
		LOG_ERR("Async task failed: %d", rsp->err);
		return rsp->err;
	}
	else
	{
		LOG_DBG("Successful Async LightDB Stream write!");		
	}
	return 0;
}

/* This will be called by the main() loop */
/* Do all of your work here! */
void app_work_sensor_read(void) {
	int err;
	char json_buf[256];
	struct sensor_value temp;
	struct sensor_value pressure;	
	struct sensor_value humidity;
	struct sensor_value accel_x;
	struct sensor_value accel_y;
	struct sensor_value accel_z;

	// adding direct i2c access
	uint8_t mcp3221[2]={0,0};
	uint32_t raw_readings = 0;

	const struct device *i2c_dev = DEVICE_DT_GET(DT_NODELABEL(i2c1));
	if (i2c_dev==NULL||!device_is_ready(i2c_dev))
	{
		LOG_ERR("Could not get i2c device\n");
		return;
	}

	/* For this demo, we just send Hello to Golioth */
	static uint8_t counter = 0;

	LOG_INF("Sending hello! %d", counter);

	err = golioth_send_hello(client);
	if (err) {
		LOG_WRN("Failed to send hello!");
	}


	// kick off an IMU sensor reading!
	LOG_DBG("Fetching IMU Reading");
	sensor_sample_fetch(imu_sensor);
	sensor_channel_get(imu_sensor, SENSOR_CHAN_ACCEL_X, &accel_x);
	LOG_DBG("  Accel X is %d.%06d", accel_x.val1, abs(accel_x.val2));
	sensor_channel_get(imu_sensor, SENSOR_CHAN_ACCEL_Y, &accel_y);
	LOG_DBG("  Accel Y is %d.%06d", accel_y.val1, abs(accel_y.val2));	
	sensor_channel_get(imu_sensor, SENSOR_CHAN_ACCEL_Z, &accel_z);
	LOG_DBG("  Accel Z is %d.%06d", accel_z.val1, abs(accel_z.val2));



	// kick off a weather sensor reading!
	LOG_DBG("Fetching Weather Reading");
	sensor_sample_fetch(weather_sensor);
	sensor_channel_get(weather_sensor, SENSOR_CHAN_AMBIENT_TEMP, &temp);
	LOG_DBG("  Temp is %d.%06d", temp.val1, abs(temp.val2));
	sensor_channel_get(weather_sensor, SENSOR_CHAN_PRESS, &pressure);
	LOG_DBG("  Pressure is %d.%06d", pressure.val1, abs(pressure.val2));
	sensor_channel_get(weather_sensor, SENSOR_CHAN_HUMIDITY, &humidity);
	LOG_DBG("  Humidity is %d.%06d", humidity.val1, abs(humidity.val2));
	

    /* Read the data register from the MCP3221 */
    int ret = i2c_burst_read(i2c_dev, 0x4D,
                                0x00, mcp3221,2);
	
    if (ret)
    {
        LOG_ERR("Unable get MSB (err %i)\n", ret);
        return;
    }

	raw_readings = (mcp3221[0]<<8) + mcp3221[1];

	LOG_INF("MSB: 0x%x\n", mcp3221[0]);
	LOG_INF("LSB: 0x%x\n", mcp3221[1]);
	LOG_INF("MS1: %d\n", raw_readings);


	/* Send sensor data to Golioth */
	snprintk(json_buf, sizeof(json_buf), 
			"{\"imu\":{\"accel_x\":%f,\"accel_y\":%f,\"accel_z\":%f},\"weather\":{\"temp\":%f,\"pressure\":%f,\"humidity\":%f},\"moisture\":{\"raw\":%d,\"level\":%d}}",
			sensor_value_to_double(&accel_x),
			sensor_value_to_double(&accel_y),
			sensor_value_to_double(&accel_z),
			sensor_value_to_double(&temp),
			sensor_value_to_double(&pressure),
			sensor_value_to_double(&humidity),
			raw_readings,	// send back raw moisture readings from i2c sensor
			60				// this is the 'level' that will be used in animations on the console, currently a fake value.
			);

	LOG_DBG("%s",json_buf);
	LOG_DBG("%d",strlen(json_buf));
	LOG_HEXDUMP_DBG(json_buf,sizeof(json_buf),"JSON buf alt");

	err = golioth_stream_push_cb(client, "sensor",
			GOLIOTH_CONTENT_FORMAT_APP_JSON,
			json_buf, strlen(json_buf),
			async_error_handler, NULL);
	if (err) LOG_ERR("Failed to send sensor data to Golioth: %d", err);

	/* Update slide values on Ostentus
	 *  -values should be sent as strings
	 *  -use the enum from app_work.h for slide key values
	 */
	snprintk(json_buf, 6, "%d", counter);
	slide_set(UP_COUNTER, json_buf, strlen(json_buf));
	snprintk(json_buf, 6, "%d", 255-counter);
	slide_set(DN_COUNTER, json_buf, strlen(json_buf));

	++counter;
}

void app_work_init(struct golioth_client* work_client) {
	client = work_client;
}

void sensor_init(void)
{

	LOG_DBG("LIS2DH Init");
	imu_sensor = (void *)DEVICE_DT_GET_ANY(st_lis2dh);

    if (imu_sensor == NULL) {
        printk("Could not get lis2dh device\n");
        return;
    }

	LOG_DBG("BME280 Init");
	weather_sensor = (void *)DEVICE_DT_GET_ANY(bosch_bme280);

    if (weather_sensor == NULL) {
        printk("Could not get bme280 device\n");
        return;
    }
}
