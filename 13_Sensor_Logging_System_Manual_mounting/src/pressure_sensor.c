/*
 * @file pressure_sensor.c
 * @author Dhruv Mamtora
 * @version 0.1.0
 * @date  27 August, 2025
 *
 * @brief Pressure sensor operations for the Sensor Data Logging System.
 *
 * @details
 * This file contains the implementation of the pressure sensor thread, which reads data from
 * the LPS22HH sensor and sends the pressure data to the logger thread via a message queue
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
#define PRESSURE_SENSOR_THREAD_STACK_SIZE 512
#define PRESSURE_SENSOR_THREAD_PRIORITY   5
#define PRESSURE_SENSOR_THREAD_SLEEP_TIME K_SECONDS(30)

#define PRESSURE_Q_MAX_MSGS 10
#define PRESSURE_Q_ALIGN    32

#define PRESSURE_Q_TIMEOUT K_MSEC(1000)

/** DEVICE CONFIGURATION */
/* Check if the LPS22HH sensor is defined in the device tree. */
#if DT_NODE_EXISTS(DT_ALIAS(pressure_sensor))
#define PRESSURE_NODE DT_ALIAS(pressure_sensor)
const struct device *const pressureDev = DEVICE_DT_GET(DT_ALIAS(pressure_sensor));
#else
#error ("Pressure sensor not found.");
#endif

/* Function prototypes */
void pressureSensorThread(void *a, void *b, void *c);
int pressureSensorProcess(pressureData_t *pressureDataStruct);

/** LOGGING CONFIGURATION */
/* Register the logging module for pressure sensor operations. */
LOG_MODULE_REGISTER(pressure);

/* Message queue to send pressure data to the logger thread */
K_MSGQ_DEFINE(pressureMsgQ, sizeof(pressureData_t), PRESSURE_Q_MAX_MSGS, PRESSURE_Q_ALIGN);

/*
 * @brief pressureSensorProcess - Process pressure sensor data.
 *
 * @author Dhruv Mamtora
 * @date 27 August, 2025
 *
 * @details
 * This function reads data from the LPS22HH pressure sensor and updates the provided
 * pressureDataStruct with the latest pressure value.
 *
 * @pre The sensor device must be initialized and ready.
 *
 * @syntax
 * int pressureSensorProcess(pressureData_t *pressureDataStruct);
 *
 * @param[in] pressureDataStruct Pointer to the structure to store the latest pressure value.
 * @param[out] pressureDataStruct->dPressure Updated with the latest pressure value in hPa.
 *
 * @return 0 on success, -1 on failure.
 *
 * @retval 0 Success
 * @retval -1 Error
 */
int pressureSensorProcess(pressureData_t *pressureDataStruct)
{
	if (!device_is_ready(pressureDev)) {
		LOG_ERR("sensor: %s device not ready.", pressureDev->name);
		return -1;
	}

	if (sensor_sample_fetch(pressureDev) < 0) {
		LOG_ERR("Sensor sample update error");
		return -1;
	}

	struct sensor_value pressure;
	if (sensor_channel_get(pressureDev, SENSOR_CHAN_PRESS, &pressure) < 0) {
		LOG_ERR("Cannot read pressure channel");
		return -1;
	}

	pressureDataStruct->dPressure = sensor_value_to_double(&pressure);
	return 0;
}

/*
 * @brief pressureSensorThread - Thread to periodically read pressure sensor data.
 *
 * @author Dhruv Mamtora
 * @date 27 August, 2025
 *
 * @details
 * This thread function continuously reads data from the LPS22HH pressure sensor at defined
 * intervals and sends the data to the logger thread via a message queue.
 *
 * @syntax
 * void pressureSensorThread(void *a, void *b, void *c);
 *
 * @param[in] a Unused parameter.
 * @param[in] b Unused parameter.
 * @param[in] c Unused parameter.
 */
void pressureSensorThread(void *a, void *b, void *c)
{
	pressureData_t pressureDataStruct;

	LOG_INF("Pressure sensor thread started.");
	while (1) {
		if (pressureSensorProcess(&pressureDataStruct) == 0) {
			/* Send the pressure data to the logger thread */
			if (k_msgq_put(&pressureMsgQ, &pressureDataStruct, PRESSURE_Q_TIMEOUT) !=
			    0) {
				LOG_WRN("Pressure message queue full, dropping data.");
			}
		}
		k_sleep(PRESSURE_SENSOR_THREAD_SLEEP_TIME);
	}

	/* This line will never be reached */
	LOG_INF("Pressure sensor thread stopped.");
}

/* Define and start the pressure sensor thread */
K_THREAD_DEFINE(pressureSensorThreadId, PRESSURE_SENSOR_THREAD_STACK_SIZE, pressureSensorThread,
		NULL, NULL, NULL, PRESSURE_SENSOR_THREAD_PRIORITY, 0, 2000);
