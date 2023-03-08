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
#include <stdlib.h>
#include <drivers/i2c.h>
#include "app_settings.h"

#if defined(CONFIG_NRF_MODEM_LIB)
#include <modem/lte_lc.h>
#include <modem/nrf_modem_lib.h>
#include <modem/modem_info.h>
#include <nrf_modem.h>
#endif

#include "app_work.h"

#include "libostentus/libostentus.h"

static struct golioth_client *client;

/* Sensor structs */

struct device *imu_sensor;
struct device *weather_sensor;
const struct device *light_sensor = DEVICE_DT_GET(DT_NODELABEL(apds9960));

uint32_t moisture_level = 0;


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
	struct sensor_value intensity;
	struct sensor_value red;
	struct sensor_value green;
	struct sensor_value blue;


	// adding direct i2c access
	uint8_t mcp3221[2]={0,0};
	uint32_t moisture_readings = 0;

	// Measure battery

	int16_t bat_voltage = 0;
	int16_t rrsp = 0;

	const struct device *i2c_dev = DEVICE_DT_GET(DT_NODELABEL(i2c1));
	if (i2c_dev==NULL||!device_is_ready(i2c_dev))
	{
		LOG_ERR("Could not get i2c device\n");
		return;
	}

	/* Request battery voltage data from the modem. */
	err = modem_info_short_get(MODEM_INFO_BATTERY, &bat_voltage);
	if (err != sizeof(bat_voltage)) {
		printk("modem_info_short_get, error: %d\n", err);
		return;
	}
	LOG_DBG("Modem voltage is %d",bat_voltage);

	/* Request rrsp data from the modem. */
	err = modem_info_short_get(MODEM_INFO_RSRP, &rrsp);
	if (err != sizeof(rrsp)) {
		printk("modem_info_short_get, error: %d\n", err);
		return ;
	}
	LOG_DBG("Modem RRSP is %d",rrsp);

	// kick off an IMU sensor reading!
	LOG_DBG("Fetching IMU Reading");
	err = sensor_sample_fetch(imu_sensor);
	if (err) {
		LOG_ERR("IMU sensor fetch failed: %d", err);
		return;
	}	
	sensor_channel_get(imu_sensor, SENSOR_CHAN_ACCEL_X, &accel_x);
	sensor_channel_get(imu_sensor, SENSOR_CHAN_ACCEL_Y, &accel_y);
	sensor_channel_get(imu_sensor, SENSOR_CHAN_ACCEL_Z, &accel_z);
	LOG_DBG("IMU: x=%d.%06d; y=%d.%06d, z=%d.%06d", accel_x.val1, abs(accel_x.val2), accel_y.val1, abs(accel_y.val2), accel_z.val1, abs(accel_z.val2));


	// kick off a weather sensor reading!
	LOG_DBG("Fetching Weather Reading");
	err = sensor_sample_fetch(weather_sensor);
	if (err) {
		LOG_ERR("Weather sensor fetch failed: %d", err);
		return;
	}	
	sensor_channel_get(weather_sensor, SENSOR_CHAN_AMBIENT_TEMP, &temp);
	sensor_channel_get(weather_sensor, SENSOR_CHAN_PRESS, &pressure);
	sensor_channel_get(weather_sensor, SENSOR_CHAN_HUMIDITY, &humidity);
	LOG_DBG("Weather: temp=%d.%06d; pressure=%d.%06d, humidity=%d.%06d", temp.val1, abs(temp.val2), pressure.val1, abs(pressure.val2), humidity.val1, abs(humidity.val2));




	// kick off a light sensor reading!
	LOG_DBG("Fetching Light Reading");

	err = sensor_sample_fetch(light_sensor);
	if (err) {
		LOG_ERR("Light sensor fetch failed: %d", err);
		return;
	}
	sensor_channel_get(light_sensor, SENSOR_CHAN_LIGHT, &intensity);
	sensor_channel_get(light_sensor, SENSOR_CHAN_RED, &red);
	sensor_channel_get(light_sensor, SENSOR_CHAN_GREEN, &green);
	sensor_channel_get(light_sensor, SENSOR_CHAN_BLUE, &blue);
	LOG_DBG("Light: %d; r=%d, g=%d, b=%d", intensity.val1, red.val1,
			green.val1, blue.val1);

    /* Read the data register from the MCP3221 */
    int ret = i2c_burst_read(i2c_dev, 0x4D,
                                0x00, mcp3221,2);
	
    if (ret)
    {
        LOG_ERR("Unable get MSB (err %i)\n", ret);
        return;
    }

	moisture_readings = (mcp3221[0]<<8) + mcp3221[1];
	LOG_DBG("MSB: 0x%x", mcp3221[0]);
	LOG_DBG("LSB: 0x%x", mcp3221[1]);
	LOG_DBG("MS1: %d", moisture_readings);

	// Remember, the values are inverted! The value of a "0" moisture is higher than "100"

	if (moisture_readings > get_moisture_level_threshold(0))
	{
		moisture_level = 0;
		LOG_DBG("Moisture level set to 0");
	}
	else if (moisture_readings > get_moisture_level_threshold(20) && moisture_readings < get_moisture_level_threshold(0))
	{
		moisture_level = 20;
		LOG_DBG("Moisture level set to 20");
	}
	else if (moisture_readings > get_moisture_level_threshold(40) && moisture_readings < get_moisture_level_threshold(20))
	{
		moisture_level = 40;
		LOG_DBG("Moisture level set to 40");
	}
	else if (moisture_readings > get_moisture_level_threshold(60) && moisture_readings < get_moisture_level_threshold(40))
	{
		moisture_level = 60;
		LOG_DBG("Moisture level set to 60");
	}
	else if (moisture_readings > get_moisture_level_threshold(80) && moisture_readings < get_moisture_level_threshold(60))
	{
		moisture_level = 80;
		LOG_DBG("Moisture level set to 80");
	}
	else if (moisture_readings < get_moisture_level_threshold(80))
	{
		moisture_level = 100;
		LOG_DBG("Moisture level set to 100");
	}
	else
	{
		// Error state
		LOG_ERR("Your math or your moisture threshold limits are wrong. Check settings.");

	}
	LOG_DBG("Moisture level is %d", moisture_level);



	/* Send sensor data to Golioth */
	snprintk(json_buf, sizeof(json_buf), 
			"{\"imu\":{\"accel_x\":%f,\"accel_y\":%f,\"accel_z\":%f},\"weather\":{\"temp\":%f,\"pressure\":%f,\"humidity\":%f},\"moisture\":{\"raw\":%d,\"level\":%d},\"light\":{\"int\":%d,\"r\":%d,\"g\":%d,\"b\":%d},\"device\":{\"bat\":%d,\"rrsp\":%d}}",
			sensor_value_to_double(&accel_x),
			sensor_value_to_double(&accel_y),
			sensor_value_to_double(&accel_z),
			sensor_value_to_double(&temp),
			sensor_value_to_double(&pressure),
			sensor_value_to_double(&humidity),
			moisture_readings,						// send back raw moisture readings from i2c sensor
			moisture_level,						// this is the 'level' that will be used in animations on the console, currently a fake value.
			intensity.val1,
			red.val1,
			green.val1,
			blue.val1,
			bat_voltage,
			rrsp			
			);

	//  LOG_DBG("%s",json_buf);
	//  LOG_DBG("%d",strlen(json_buf));
	//  LOG_HEXDUMP_DBG(json_buf,sizeof(json_buf),"JSON buf alt");

	err = golioth_stream_push_cb(client, "sensor",
			GOLIOTH_CONTENT_FORMAT_APP_JSON,
			json_buf, strlen(json_buf),
			async_error_handler, NULL);
	if (err) LOG_ERR("Failed to send sensor data to Golioth: %d", err);

	/* Update slide values on Ostentus
	 *  -values should be sent as strings
	 *  -use the enum from app_work.h for slide key values
	 */
	snprintk(json_buf, sizeof(json_buf), "%d", moisture_readings);
	slide_set(MOISTURE_READING, json_buf, strlen(json_buf));
	snprintk(json_buf, sizeof(json_buf), "%d", moisture_level);
	slide_set(MOISTURE_LEVEL, json_buf, strlen(json_buf));
	snprintk(json_buf, sizeof(json_buf), "%d", intensity.val1);
	slide_set(LIGHT_INT, json_buf, strlen(json_buf));
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

	LOG_DBG("APDS9960 Init");
	light_sensor = (void *)DEVICE_DT_GET_ANY(avago_apds9960);

    if (light_sensor == NULL) {
        printk("Could not get apds9960 device\n");
        return;
    }	
}
