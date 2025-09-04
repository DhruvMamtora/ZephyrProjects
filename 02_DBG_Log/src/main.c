/*
 * @file main.c
 * @author Dhruv Mamtora
 * @version 0.1.0
 * @date  31 July, 2025
 *
 * @brief LED Blinking with Debug Logs and Multiple LEDs using Zephyr RTOS
 *
 * @details
 * This application demonstrates blinking all available user-controllable LEDs
 * using GPIO device tree bindings in Zephyr. It also enables the logging module
 * at debug level to print messages indicating LED states.
 *
 * @copyright Copyright (c) 2025
 */

/** REQUIRED HEADER FILES */
#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>

/** MACRO DEFINITIONS */
#define LED0_NODE DT_ALIAS(led0)
#define LED1_NODE DT_ALIAS(led1)
#define LED2_NODE DT_ALIAS(led2)
#define LED3_NODE DT_ALIAS(led3)

/* Create gpio_dt_spec structures for each LED */
static const struct gpio_dt_spec led_00 = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct gpio_dt_spec led_01 = GPIO_DT_SPEC_GET(LED1_NODE, gpios);
static const struct gpio_dt_spec led_02 = GPIO_DT_SPEC_GET(LED2_NODE, gpios);
static const struct gpio_dt_spec led_03 = GPIO_DT_SPEC_GET(LED3_NODE, gpios);

/** LOGGING CONFIGURATION */
LOG_MODULE_REGISTER(main, LOG_LEVEL_DBG);

/*
 * @brief main - Main application entry point
 *
 * @author Dhruv Mamtora
 * @date 31 July, 2025
 *
 * @details
 * Configures each LED pin as output and toggles all LEDs on a fixed interval,
 * while logging the LED state.
 *
 * @pre
 * - All LEDs (led0 to led3) must be defined in the device tree with valid aliases.
 * - The GPIO devices backing these LEDs must be ready and available.
 * - Zephyr logging subsystem must be enabled and configured.
 *
 * @return int Returns -1 on error.
 */
int main(void)
{
	int iReturn;
	bool bLedState = true;

	if (!gpio_is_ready_dt(&led_00) || !gpio_is_ready_dt(&led_01) ||
	    !gpio_is_ready_dt(&led_02) || !gpio_is_ready_dt(&led_03)) {
		return -1;
	}

	iReturn = gpio_pin_configure_dt(&led_00, GPIO_OUTPUT_ACTIVE);
	if (iReturn < 0) {
		return -1;
	}

	iReturn = gpio_pin_configure_dt(&led_01, GPIO_OUTPUT_ACTIVE);
	if (iReturn < 0) {
		return -1;
	}

	iReturn = gpio_pin_configure_dt(&led_02, GPIO_OUTPUT_ACTIVE);
	if (iReturn < 0) {
		return -1;
	}

	iReturn = gpio_pin_configure_dt(&led_03, GPIO_OUTPUT_ACTIVE);
	if (iReturn < 0) {
		return -1;
	}

	while (1) {
		bLedState = !bLedState;

		iReturn = gpio_pin_toggle_dt(&led_00);
		if (iReturn < 0) {
			return -1;
		}

		iReturn = gpio_pin_toggle_dt(&led_01);
		if (iReturn < 0) {
			return -1;
		}

		iReturn = gpio_pin_toggle_dt(&led_02);
		if (iReturn < 0) {
			return -1;
		}

		iReturn = gpio_pin_toggle_dt(&led_03);
		if (iReturn < 0) {
			return -1;
		}

		/* Debug Log */
		LOG_DBG("LED %s", bLedState ? "ON" : "OFF");
		k_msleep(750);
	}
}
