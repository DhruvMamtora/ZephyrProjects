/*
 * @file imu_sensor.c
 * @author Dhruv Mamtora
 * @version 0.1.0
 * @date  27 August, 2025
 *
 * @brief This file contains functions to interface with the LSM6DSL IMU sensor.
 *
 * @details
 * This file implements functions to initialize the LSM6DSL IMU sensor, read accelerometer
 * and gyroscope data, and send the data to a logger thread via a message queue.
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
#define IMU_SENSOR_THREAD_STACK_SIZE 1024
#define IMU_SENSOR_THREAD_PRIORITY   5
#define IMU_SENSOR_THREAD_SLEEP_TIME K_SECONDS(30)

#define IMU_Q_MAX_MSGS 10
#define IMU_Q_ALIGN    32

#define IMU_Q_TIMEOUT K_MSEC(1000)

/** DEVICE CONFIGURATION */
/* Check if the LSM6DSL sensor is defined in the device tree. */
#if DT_NODE_EXISTS(DT_ALIAS(imu_sensor))
#define IMU_NODE DT_ALIAS(imu_sensor)
const struct device *const imuDev = DEVICE_DT_GET(DT_ALIAS(imu_sensor));
#else
#error ("IMU sensor not found.");
#endif

/* Function prototypes */
void imuSensorThread(void *a, void *b, void *c);
int imuSensorProcess(motionData_t *motionDataStruct);

/** LOGGING CONFIGURATION */
/* Register the logging module for IMU sensor operations. */
LOG_MODULE_REGISTER(imu);

/* Message queue to send IMU data to the logger thread */
K_MSGQ_DEFINE(imuMsgQ, sizeof(motionData_t), IMU_Q_MAX_MSGS, IMU_Q_ALIGN);

/*
 * @brief imuSensorProcess - Process IMU sensor data.
 *
 * @author Dhruv Mamtora
 * @date 27 August, 2025
 *
 * @details
 * This function reads data from the LSM6DSL IMU sensor and updates the provided motionDataStruct
 * with the latest accelerometer and gyroscope values.
 *
 * @pre The IMU sensor device must be initialized and ready.
 *
 * @syntax
 * int imuSensorProcess(motionData_t *motionDataStruct);
 *
 * @param[in] motionDataStruct Pointer to the structure to store the latest motion data.
 * @param[out] motionDataStruct->accel Updated with the latest accelerometer data.
 * @param[out] motionDataStruct->gyro Updated with the latest gyroscope data.
 *
 * @return 0 on success, -1 on failure.
 *
 * @retval 0 Success
 * @retval -1 Error
 */
int imuSensorProcess(motionData_t *motionDataStruct)
{

	if (!device_is_ready(imuDev)) {
		LOG_ERR("sensor: %s device not ready.", imuDev->name);
		return -1;
	}

	struct sensor_value odr_attr;

	/* set accel/gyro sampling frequency to 104 Hz */
	odr_attr.val1 = 104;
	odr_attr.val2 = 0;

	if (sensor_attr_set(imuDev, SENSOR_CHAN_ACCEL_XYZ, SENSOR_ATTR_SAMPLING_FREQUENCY,
			    &odr_attr) < 0) {
		LOG_ERR("Cannot set sampling frequency for accelerometer.");
		return -1;
	}

	if (sensor_attr_set(imuDev, SENSOR_CHAN_GYRO_XYZ, SENSOR_ATTR_SAMPLING_FREQUENCY,
			    &odr_attr) < 0) {
		LOG_ERR("Cannot set sampling frequency for gyro.");
		return -1;
	}

	if (sensor_sample_fetch(imuDev) < 0) {
		LOG_ERR("Sensor sample update error");
		return -1;
	}

	struct sensor_value accel_x, accel_y, accel_z;
	struct sensor_value gyro_x, gyro_y, gyro_z;

	/* Fetch accelerometer data */
	if (sensor_sample_fetch_chan(imuDev, SENSOR_CHAN_ACCEL_XYZ) < 0) {
		LOG_ERR("Accelerometer Sensor sample update error");
		return -1;
	}
	sensor_channel_get(imuDev, SENSOR_CHAN_ACCEL_X, &accel_x);
	sensor_channel_get(imuDev, SENSOR_CHAN_ACCEL_Y, &accel_y);
	sensor_channel_get(imuDev, SENSOR_CHAN_ACCEL_Z, &accel_z);

	motionDataStruct->accel.x = sensor_value_to_double(&accel_x);
	motionDataStruct->accel.y = sensor_value_to_double(&accel_y);
	motionDataStruct->accel.z = sensor_value_to_double(&accel_z);

	/* Fetch gyroscope data */
	if (sensor_sample_fetch_chan(imuDev, SENSOR_CHAN_GYRO_XYZ) < 0) {
		LOG_ERR("Gyroscope Sensor sample update error");
		return -1;
	}
	sensor_channel_get(imuDev, SENSOR_CHAN_GYRO_X, &gyro_x);
	sensor_channel_get(imuDev, SENSOR_CHAN_GYRO_Y, &gyro_y);
	sensor_channel_get(imuDev, SENSOR_CHAN_GYRO_Z, &gyro_z);

	motionDataStruct->gyro.x = sensor_value_to_double(&gyro_x);
	motionDataStruct->gyro.y = sensor_value_to_double(&gyro_y);
	motionDataStruct->gyro.z = sensor_value_to_double(&gyro_z);

	return 0;
}

/*
 * @brief imuSensorThread - Thread to periodically read IMU sensor data.
 *
 * @author Dhruv Mamtora
 * @date 27 August, 2025
 *
 * @details
 * This thread function continuously reads data from the LSM6DSL IMU sensor at defined intervals
 * and sends the data to the logger thread via a message queue.
 *
 * @syntax
 * void imuSensorThread(void *a, void *b, void *c);
 *
 * @param[in] a Unused parameter.
 * @param[in] b Unused parameter.
 * @param[in] c Unused parameter.
 */
void imuSensorThread(void *a, void *b, void *c)
{
	motionData_t motionDataStruct;

	LOG_INF("IMU sensor thread started.");

	while (1) {
		if (imuSensorProcess(&motionDataStruct) == 0) {
			/* Send the motion data to the logger thread via message queue */
			if (k_msgq_put(&imuMsgQ, &motionDataStruct, IMU_Q_TIMEOUT) != 0) {
				LOG_WRN("IMU message queue full, dropping data");
			}
		}
		k_sleep(IMU_SENSOR_THREAD_SLEEP_TIME);
	}

	/* This line will never be reached */
	LOG_INF("IMU sensor thread stopped.");
}

/* Define the IMU sensor thread */
K_THREAD_DEFINE(imuSensorThreadId, IMU_SENSOR_THREAD_STACK_SIZE, imuSensorThread, NULL, NULL, NULL,
		IMU_SENSOR_THREAD_PRIORITY, 0, 2000);
