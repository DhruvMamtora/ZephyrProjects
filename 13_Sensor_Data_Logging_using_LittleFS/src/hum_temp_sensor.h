#ifndef HUM_TEMP_SENSOR_H
#define HUM_TEMP_SENSOR_H

void hum_temp_sensor_process_sample(void);
int hun_temp_sensor_init(void);
int hun_temp_sensor_thread(void *a, void *b, void *c);

#endif /* HUM_TEMP_SENSOR_H */