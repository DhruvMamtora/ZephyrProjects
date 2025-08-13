#include "pressure_sensor.h"
#include "sensor_shared.h"

#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>

#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(pressure);

#if DT_NODE_EXISTS(DT_ALIAS(pressure_sensor))
#define PRESSURE_NODE DT_ALIAS(pressure_sensor)
const struct device *const pressure_dev = DEVICE_DT_GET(DT_ALIAS(pressure_sensor));
#else
#error ("Pressure sensor not found.");
#endif

void pressure_sensor_process_sample(void)
{
	if (!device_is_ready(pressure_dev)) {
		LOG_ERR("sensor: %s device not ready.", pressure_dev->name);
		return;
	}

	if (sensor_sample_fetch(pressure_dev) < 0) {
		LOG_INF("Sensor sample update error");
		return;
	}

	struct sensor_value pressure;
	if (sensor_channel_get(pressure_dev, SENSOR_CHAN_PRESS, &pressure) < 0) {
		LOG_ERR("Cannot read pressure channel");
		return;
	}

	/* display pressure */
	LOG_INF("Pressure:%.1f kPa", sensor_value_to_double(&pressure));
}

int pressure_sensor_init(void)
{
	if (!device_is_ready(pressure_dev)) {
		LOG_ERR("sensor: %s device not ready.", pressure_dev->name);
		return -1;
	}

	pressure_sensor_process_sample();

	return 0;
}

int pressure_sensor_thread(void *a, void *b, void *c)
{
	if (!device_is_ready(pressure_dev)) {
		LOG_ERR("sensor: %s device not ready.", pressure_dev->name);
		return -1;
	}

	while (1) {
		if (sensor_sample_fetch(pressure_dev) < 0) {
			LOG_INF("Sensor sample update error");
			return -1;
		}

		struct sensor_value pressure;
		if (sensor_channel_get(pressure_dev, SENSOR_CHAN_PRESS, &pressure) < 0) {
			LOG_ERR("Cannot read pressure channel");
			return -1;
		}

		k_mutex_lock(&sensor_data_mutex, K_FOREVER);
		sensor_data.pressure = sensor_value_to_double(&pressure);
		k_mutex_unlock(&sensor_data_mutex);

		/* display pressure */
		LOG_INF("Pressure:%.1f kPa", sensor_value_to_double(&pressure));

		k_sleep(K_SECONDS(5));
	}
}
