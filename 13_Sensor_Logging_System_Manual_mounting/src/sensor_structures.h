/*
 * @file shared_data.h
 * @author Dhruv Mamtora
 * @version 0.1.0
 * @date  21 August, 2025
 *
 * @brief Shared data structures for sensor data.
 *
 * @details
 * This header file defines the data structures used to store and share sensor data
 * between different threads in the Sensor Data Logging System.
 *
 * @copyright Copyright (c) 2025
 */

#ifndef SENSOR_STRUCTURES_H
#define SENSOR_STRUCTURES_H

typedef struct {
	double dHumidity;
} humidityData_t;

typedef struct {
	double dTemperature;
} temperatureData_t;

typedef struct {
	double dPressure;
} pressureData_t;

/* Define a vector structure to hold accelerometer and gyroscope data */
typedef struct {
	double x, y, z;
} vector_t;

typedef struct {
	vector_t accel;
	vector_t gyro;
} motionData_t;

/* Structure to hold the latest sensor values */
typedef struct {
	humidityData_t humidityData;
	temperatureData_t temperatureData;
	pressureData_t pressureData;
	motionData_t motionData;
} sensorSharedBuffer_t;

#endif /* SENSOR_STRUCTURES_H */
