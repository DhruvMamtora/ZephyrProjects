/*
 * @file sensor_logger.c
 * @author Dhruv Mamtora
 * @version 0.1.0
 * @date  26 August, 2025
 *
 * @brief Sensor logger thread implementation.
 *
 * @details
 * This file contains the implementation of the sensor logger thread, which is responsible for
 * receiving sensor data from various sensor threads and logging it. The logger thread uses a
 * message queue to receive data from the humidity sensor thread.
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
#define LOGGER_THREAD_STACK_SIZE 1024
#define LOGGER_THREAD_PRIORITY   5
#define LOGGER_THREAD_SLEEP_TIME K_SECONDS(5)

#define QUEUE_TIMEOUT K_MSEC(1000)

/** LOGGING CONFIGURATION */
/* Register the logging module for sensor logger operations. */
LOG_MODULE_REGISTER(sensor_logger);

/* Message queue to receive humidity data from the humidity sensor thread */
extern struct k_msgq humMsgQ;
extern struct k_msgq tempMsgQ;
extern struct k_msgq pressureMsgQ;
extern struct k_msgq imuMsgQ;

void printData(sensorSharedBuffer_t *data)
{
	LOG_INF("Humidity: %.2f %%", data->humidityData.dHumidity);
	LOG_INF("Temperature: %.2f C", data->temperatureData.dTemperature);
	LOG_INF("Pressure: %.2f hPa", data->pressureData.dPressure);
	LOG_INF("Accelerometer: X=%.2f Y=%.2f Z=%.2f", data->motionData.accel.x,
		data->motionData.accel.y, data->motionData.accel.z);
	LOG_INF("Gyroscope: X=%.2f Y=%.2f Z=%.2f", data->motionData.gyro.x, data->motionData.gyro.y,
		data->motionData.gyro.z);
}

/*
 * @brief loggerThread - Logger thread to log sensor data.
 *
 * @author Dhruv Mamtora
 * @date 26 August, 2025
 *
 * @details
 * This thread continuously receives sensor data from the humidity sensor thread via a message
 * queue and logs the received data.
 *
 * @pre The humidity sensor thread must be running and sending data to the message queue.
 *
 * @syntax
 * void loggerThread(void *a, void *b, void *c);
 *
 * @param[in] a Unused parameter.
 * @param[in] b Unused parameter.
 * @param[in] c Unused parameter.
 *
 * @return None.
 */
void loggerThread(void *a, void *b, void *c)
{
	sensorSharedBuffer_t localBuffer;
	LOG_INF("Logger thread started.");

	while (1) {
		k_msgq_get(&humMsgQ, &localBuffer.humidityData, QUEUE_TIMEOUT);
		k_msgq_get(&tempMsgQ, &localBuffer.temperatureData, QUEUE_TIMEOUT);
		k_msgq_get(&pressureMsgQ, &localBuffer.pressureData, QUEUE_TIMEOUT);
		k_msgq_get(&imuMsgQ, &localBuffer.motionData, QUEUE_TIMEOUT);

		/* Log the received sensor data */
		printData(&localBuffer);
		k_sleep(LOGGER_THREAD_SLEEP_TIME);
	}
}

/* Define the stack and thread for the logger thread. */
K_THREAD_DEFINE(loggerThreadId, LOGGER_THREAD_STACK_SIZE, loggerThread, NULL, NULL, NULL,
		LOGGER_THREAD_PRIORITY, 0, 1000);
