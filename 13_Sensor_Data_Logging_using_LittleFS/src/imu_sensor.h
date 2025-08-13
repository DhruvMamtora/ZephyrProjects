#ifndef IMU_SENSOR_H
#define IMU_SENSOR_H

void imu_sensor_sample_process(void);
int imu_sensor_init(void);
int imu_sensor_thread(void *a, void *b, void *c);

#endif /* IMU_SENSOR_H */
