/*
 * @file hum_sensor.c
 * @author Dhruv Mamtora
 * @version 0.1.0
 * @date  26 August, 2025
 *
 * @brief Humidity sensor processing for the Sensor Data Logging System.
 *
 * @details
 * This file contains the implementation of the humidity sensor thread, which reads data from
 * the HTS221 sensor and sends the humidity data to the logger thread via a message queue
 * for logging.
 *
 * @copyright Copyright (c) 2025
 */

/** REQUIRED HEADER FILES */
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "sensor_structures.h"

/** MACRO DEFINITIONS */
#define HUM_SENSOR_THREAD_STACK_SIZE 512
#define HUM_SENSOR_THREAD_PRIORITY   5
#define HUM_SENSOR_THREAD_SLEEP_TIME K_SECONDS(30)

#define HUM_Q_MAX_MSGS 10
#define HUM_Q_ALIGN    32

#define HUM_Q_TIMEOUT K_MSEC(1000)

/** DEVICE CONFIGURATION */
/* Check if the HTS221 sensor is defined in the device tree. */
#if DT_NODE_EXISTS(DT_ALIAS(ht_sensor))
#define HUM_TEMP_NODE DT_ALIAS(ht_sensor)
const struct device *const humDev = DEVICE_DT_GET(DT_ALIAS(ht_sensor));
#else
#error ("Humidity sensor not found.");
#endif

/* Function prototypes */
void humSensorThread(void *a, void *b, void *c);
int humSensorProcess(humidityData_t *humidityDataStruct);

/** LOGGING CONFIGURATION */
/* Register the logging module for humidity sensor operations. */
LOG_MODULE_REGISTER(hum);

/* Message queue to send humidity data to the logger thread */
K_MSGQ_DEFINE(humMsgQ, sizeof(humidityData_t), HUM_Q_MAX_MSGS, HUM_Q_ALIGN);

/*
 * @brief humSensorProcess - Process humidity sensor data.
 *
 * @author Dhruv Mamtora
 * @date 26 August, 2025
 *
 * @details
 * This function reads the humidity data from the HTS221 sensor and updates the provided
 *
 * @pre The sensor device must be initialized and ready.
 *
 * @syntax
 * int humSensorProcess(humidityData_t *humidityDataStruct);
 *
 * @param[in] humidityDataStruct Pointer to a humidityData_t structure to store the latest humidity
 * value.
 * @param[out] humidityDataStruct->dHumidity Updated with the latest humidity value in percentage.
 *
 * @return 0 on success, -1 on failure.
 *
 * @retval 0 Success
 * @retval -1 Error
 */
int humSensorProcess(humidityData_t *humidityDataStruct)
{
	if (!device_is_ready(humDev)) {
		LOG_ERR("sensor: %s device not ready.", humDev->name);
		return -1;
	}

	if (sensor_sample_fetch(humDev) < 0) {
		LOG_ERR("Sensor sample update error");
		return -1;
	}

	struct sensor_value humValue;
	if (sensor_channel_get(humDev, SENSOR_CHAN_HUMIDITY, &humValue) < 0) {
		LOG_ERR("Cannot read HTS221 humidity channel");
		return -1;
	}

	/* Update latest humidity value */
	humidityDataStruct->dHumidity = sensor_value_to_double(&humValue);
	return 0;
}

/*
 * @brief humSensorThread - Humidity sensor thread.
 *
 * @author Dhruv Mamtora
 * @date 26 August, 2025
 *
 * @details
 * This thread continuously reads humidity data from the HTS221 sensor and sends it to the logger
 * thread via a message queue for logging.
 *
 * @pre The sensor device must be initialized and ready.
 *
 * @syntax
 * void humSensorThread(void *a, void *b, void *c);
 *
 * @param[in] a Unused parameter.
 * @param[in] b Unused parameter.
 * @param[in] c Unused parameter.
 *
 * @return None.
 */
void humSensorThread(void *a, void *b, void *c)
{
	humidityData_t humidityDataStruct;

	LOG_INF("Humidity sensor thread started.");

	while (1) {
		if (humSensorProcess(&humidityDataStruct) == 0) {
			/* Send the humidity data to the logger thread */
			if (k_msgq_put(&humMsgQ, &humidityDataStruct, HUM_Q_TIMEOUT) != 0) {
				LOG_WRN("Humidity message queue full, dropping data");
			}
		}
		k_sleep(HUM_SENSOR_THREAD_SLEEP_TIME);
	}

	/* This line will never be reached */
	LOG_INF("Humidity sensor thread stopped.");
}

/* Define the humidity sensor thread */
K_THREAD_DEFINE(humThreadId, HUM_SENSOR_THREAD_STACK_SIZE, humSensorThread, NULL, NULL, NULL,
		HUM_SENSOR_THREAD_PRIORITY, 0, 2000);
