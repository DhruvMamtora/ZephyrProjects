/*
 * @file main.c
 * @author Dhruv Mamtora
 * @version 0.1.0
 * @date 31 July, 2025
 *
 * @brief Basic LED blinking using Zephyr RTOS.
 *
 * @details
 * This file contains a simple implementation for blinking a user-controllable LED
 * on a Zephyr-supported board. It demonstrates GPIO configuration and toggling using
 * the device tree.
 *
 * @copyright Copyright (c) 2025
 */

/** REQUIRED HEADER FILES */
#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

/** MACRO DEFINITIONS */
#define LED0_NODE DT_ALIAS(led0)

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

/*
 * @brief main - Entry point for the LED blinking application.
 *
 * @author Dhruv Mamtora
 * @date 31 July, 2025
 *
 * @details
 * This function initializes the GPIO pin connected to LED0, configures it for output,
 * and toggles it in a loop to blink the LED at 500 ms intervals. It uses Zephyr's
 * device tree API and GPIO helpers.
 *
 * @pre The board's device tree must define an alias for 'led0'.
 *
 * @return int Returns -1 on error.
 *
 * @error
 * - GPIO device not ready.
 * - GPIO configuration or toggle failure.
 */

int main(void)
{
	if (!gpio_is_ready_dt(&led)) {
		return -1;
	}

	if (gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE) < 0) {
		return -1;
	}

	while (1) {
		if (gpio_pin_toggle_dt(&led) < 0) {
			return -1;
		}

		k_msleep(500);
	}
}
