#ifndef PRESSURE_SENSOR_H
#define PRESSURE_SENSOR_H

void pressure_sensor_process_sample(void);
int pressure_sensor_init(void);
int pressure_sensor_thread(void *a, void *b, void *c);

#endif /* PRESSURE_SENSOR_H */
