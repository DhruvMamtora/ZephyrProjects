/*
 * @file temp_sensor.c
 * @author Dhruv Mamtora
 * @version 0.1.0
 * @date  27 August, 2025
 *
 * @brief Temperature sensor processing for the Sensor Data Logging System.
 *
 * @details
 * This file contains the implementation of the temperature sensor thread, which reads data from
 * the HTS221 sensor and sends the temperature data to the logger thread via a message queue
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
#define TEMP_SENSOR_THREAD_STACK_SIZE 512
#define TEMP_SENSOR_THREAD_PRIORITY   5
#define TEMP_SENSOR_THREAD_SLEEP_TIME K_SECONDS(30)

#define TEMP_Q_MAX_MSGS 10
#define TEMP_Q_ALIGN    32

#define TEMP_Q_TIMEOUT K_MSEC(1000)

/** DEVICE CONFIGURATION */
/* Check if the HTS221 sensor is defined in the device tree. */
#if DT_NODE_EXISTS(DT_ALIAS(ht_sensor))
#define HUM_TEMP_NODE DT_ALIAS(ht_sensor)
const struct device *const tempDev = DEVICE_DT_GET(DT_ALIAS(ht_sensor));
#else
#error ("Temperature sensor not found.");
#endif

/* Function prototypes */
void tempSensorThread(void *a, void *b, void *c);
int tempSensorProcess(temperatureData_t *temperatureDataStruct);

/** LOGGING CONFIGURATION */
/* Register the logging module for humidity sensor operations. */
LOG_MODULE_REGISTER(temp);

/* Message queue to send humidity data to the logger thread */
K_MSGQ_DEFINE(tempMsgQ, sizeof(temperatureData_t), TEMP_Q_MAX_MSGS, TEMP_Q_ALIGN);

/*
 * @brief tempSensorProcess - Process temperature sensor data.
 *
 * @author Dhruv Mamtora
 * @date 27 August, 2025
 *
 * @details
 * This function reads the temperature data from the HTS221 sensor and updates the provided
 * temperatureData_t structure with the latest temperature value.
 *
 * @pre The sensor device must be initialized and ready.
 *
 * @syntax
 * int tempSensorProcess(temperatureData_t *temperatureDataStruct);
 *
 * @param[in] temperatureDataStruct Pointer to a temperatureData_t structure to store the latest
 * temperature value.
 * @param[out] temperatureDataStruct->dTemperature Updated with the latest temperature value in °C.
 *
 * @return 0 on success, -1 on failure.
 *
 * @retval 0 Success
 * @retval -1 Error
 */
int tempSensorProcess(temperatureData_t *temperatureDataStruct)
{
	if (!device_is_ready(tempDev)) {
		LOG_ERR("sensor: %s device not ready.", tempDev->name);
		return -1;
	}

	if (sensor_sample_fetch(tempDev) < 0) {
		LOG_ERR("Sensor sample update error");
		return -1;
	}

	struct sensor_value tempValue;
	if (sensor_channel_get(tempDev, SENSOR_CHAN_AMBIENT_TEMP, &tempValue) < 0) {
		LOG_ERR("Cannot read HTS221 temperature channel");
		return -1;
	}

	/* Update latest temperature value */
	temperatureDataStruct->dTemperature = sensor_value_to_double(&tempValue);
	return 0;
}

/*
 * @brief tempSensorThread - Temperature sensor thread.
 *
 * @author Dhruv Mamtora
 * @date 27 August, 2025
 *
 * @details
 * This thread continuously reads temperature data from the HTS221 sensor and sends it to the logger
 * thread via a message queue for logging.
 *
 * @pre The sensor device must be initialized and ready.
 *
 * @syntax
 * void tempSensorThread(void *a, void *b, void *c);
 *
 * @param[in] a Unused parameter.
 * @param[in] b Unused parameter.
 * @param[in] c Unused parameter.
 *
 * @return None.
 */
void tempSensorThread(void *a, void *b, void *c)
{
	temperatureData_t temperatureDataStruct;

	LOG_INF("Temperature sensor thread started.");

	while (1) {
		if (tempSensorProcess(&temperatureDataStruct) == 0) {
			/* Send the temperature data to the logger thread */
			if (k_msgq_put(&tempMsgQ, &temperatureDataStruct, TEMP_Q_TIMEOUT) != 0) {
				LOG_WRN("Temperature message queue full, dropping data");
			}
		}
		k_sleep(TEMP_SENSOR_THREAD_SLEEP_TIME);
	}

	/* This line will never be reached */
	LOG_INF("Temperature sensor thread stopped.");
}

/* Define the temperature sensor thread */
K_THREAD_DEFINE(tempThreadId, TEMP_SENSOR_THREAD_STACK_SIZE, tempSensorThread, NULL, NULL, NULL,
		TEMP_SENSOR_THREAD_PRIORITY, 0, 2000);
