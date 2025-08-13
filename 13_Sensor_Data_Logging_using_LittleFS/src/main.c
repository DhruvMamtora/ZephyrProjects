
#include "hum_temp_sensor.h"
#include "imu_sensor.h"
#include "pressure_sensor.h"
#include "sensor_shared.h"

#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>

#include <zephyr/logging/log.h>
#include <zephyr/shell/shell.h>

#include <stdbool.h>

#define STACK_SIZE 1024
#define PRIORITY   5

K_THREAD_STACK_DEFINE(hum_temp_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(pressure_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(imu_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(sensor_thread_stack_area, STACK_SIZE);

static struct k_thread hum_temp_thread_data;
static struct k_thread pressure_thread_data;
static struct k_thread imu_thread_data;
static struct k_thread sensor_thread_data;

static k_tid_t hum_temp_tid, pressure_tid, imu_tid, sensor_tid;
static bool terminate_sensor_thread = false;

LOG_MODULE_REGISTER(main);

int main(void)
{
    int ret;

    k_mutex_init(&sensor_data_mutex);

    ret = hun_temp_sensor_init();
    if (ret < 0) {
        LOG_ERR("Humidity-Temperature Sensor init failed");
        return ret;
    }

    ret = pressure_sensor_init();
    if (ret < 0) {
        LOG_ERR("Pressure Sensor init failed");
        return ret;
    }

    ret = imu_sensor_init();
    if (ret < 0) {
        LOG_ERR("IMU Sensor init failed");
        return ret;
    }

    // Start threads for each sensor
    hum_temp_tid = k_thread_create(&hum_temp_thread_data, hum_temp_stack, K_THREAD_STACK_SIZEOF(hum_temp_stack),
                                   (void *)hun_temp_sensor_thread, NULL, NULL, NULL, PRIORITY, 0, K_NO_WAIT);

    pressure_tid = k_thread_create(&pressure_thread_data, pressure_stack, K_THREAD_STACK_SIZEOF(pressure_stack),
                                   (void *)pressure_sensor_thread, NULL, NULL, NULL, PRIORITY, 0, K_NO_WAIT);

    imu_tid = k_thread_create(&imu_thread_data, imu_stack, K_THREAD_STACK_SIZEOF(imu_stack), (void *)imu_sensor_thread,
                              NULL, NULL, NULL, PRIORITY, 0, K_NO_WAIT);

    return 0;
}

static void sensor_demo_thread(void *arg0, void *arg1, void *arg2)
{
    LOG_INF("sensor_demo_thread started.");

    while (!terminate_sensor_thread) {
        hum_temp_sensor_process_sample();
        pressure_sensor_process_sample();
        imu_sensor_sample_process();
        k_sleep(K_MSEC(2000));
    }

    terminate_sensor_thread = false;
    LOG_INF("sensor_demo_thread stopped.");
}

static int shell_fetch_sample(const struct shell *sh, size_t argc, char **argv, void *data)
{
    hum_temp_sensor_process_sample();
    pressure_sensor_process_sample();
    imu_sensor_sample_process();
    return 0;
}

static int shell_start_thread(const struct shell *sh, size_t argc, char **argv, void *data)
{
    sensor_tid =
        k_thread_create(&sensor_thread_data, sensor_thread_stack_area, K_THREAD_STACK_SIZEOF(sensor_thread_stack_area),
                        sensor_demo_thread, NULL, NULL, NULL, 5, 0, K_NO_WAIT);
    return 0;
}

static int shell_stop_thread(const struct shell *sh, size_t argc, char **argv, void *data)
{
    terminate_sensor_thread = true;
    k_thread_join(sensor_tid, K_FOREVER);
    return 0;
}

static int shell_start_hum_temp_thread(const struct shell *sh, size_t argc, char **argv, void *data)
{
    hum_temp_tid = k_thread_create(&hum_temp_thread_data, hum_temp_stack, K_THREAD_STACK_SIZEOF(hum_temp_stack),
                                   (void *)hun_temp_sensor_thread, NULL, NULL, NULL, PRIORITY, 0, K_NO_WAIT);
    shell_print(sh, "Humidity/Temperature thread started.");
    return 0;
}

static int shell_start_pressure_thread(const struct shell *sh, size_t argc, char **argv, void *data)
{
    pressure_tid = k_thread_create(&pressure_thread_data, pressure_stack, K_THREAD_STACK_SIZEOF(pressure_stack),
                                   (void *)pressure_sensor_thread, NULL, NULL, NULL, PRIORITY, 0, K_NO_WAIT);
    shell_print(sh, "Pressure thread started.");
    return 0;
}

static int shell_start_imu_thread(const struct shell *sh, size_t argc, char **argv, void *data)
{
    imu_tid = k_thread_create(&imu_thread_data, imu_stack, K_THREAD_STACK_SIZEOF(imu_stack), (void *)imu_sensor_thread,
                              NULL, NULL, NULL, PRIORITY, 0, K_NO_WAIT);
    shell_print(sh, "IMU thread started.");
    return 0;
}

static int shell_stop_hum_temp_thread(const struct shell *sh, size_t argc, char **argv)
{
    if (hum_temp_tid != NULL) {
        k_thread_abort(hum_temp_tid);
        hum_temp_tid = NULL;
        shell_print(sh, "Humidity/Temperature thread stopped.");
    } else {
        shell_print(sh, "Humidity/Temperature thread not running.");
    }
    return 0;
}

static int shell_stop_pressure_thread(const struct shell *sh, size_t argc, char **argv)
{
    if (pressure_tid != NULL) {
        k_thread_abort(pressure_tid);
        pressure_tid = NULL;
        shell_print(sh, "Pressure thread stopped.");
    } else {
        shell_print(sh, "Pressure thread not running.");
    }
    return 0;
}

static int shell_stop_imu_thread(const struct shell *sh, size_t argc, char **argv)
{
    if (imu_tid != NULL) {
        k_thread_abort(imu_tid);
        imu_tid = NULL;
        shell_print(sh, "IMU thread stopped.");
    } else {
        shell_print(sh, "IMU thread not running.");
    }
    return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(sub_demo, SHELL_CMD(fetch_sample, NULL, "Fetch sample.", shell_fetch_sample),
                               SHELL_CMD(start, NULL, "Start Sample Thread.", shell_start_thread),
                               SHELL_CMD(stop, NULL, "Stop Sample Thread.", shell_stop_thread),
                               SHELL_CMD(start_hum_temp, NULL, "Start HTS221 thread", shell_start_hum_temp_thread),
                               SHELL_CMD(start_pressure, NULL, "Start LPS22HB thread", shell_start_pressure_thread),
                               SHELL_CMD(start_imu, NULL, "Start LSM6DSL thread", shell_start_imu_thread),
                               SHELL_CMD(stop_hum_temp, NULL, "Stop HTS221 thread", shell_stop_hum_temp_thread),
                               SHELL_CMD(stop_pressure, NULL, "Stop LPS22HB thread", shell_stop_pressure_thread),
                               SHELL_CMD(stop_imu, NULL, "Stop LSM6DSL thread", shell_stop_imu_thread),
                               SHELL_SUBCMD_SET_END);
/* Creating root (level 0) command "demo" */
SHELL_CMD_REGISTER(sensor_demo, &sub_demo, "Sensor Demo commands", NULL);
