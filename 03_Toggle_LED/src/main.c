/*
 * @file main.c
 * @author Dhruv Mamtora
 * @version 0.1.0
 * @date  31 July, 2025
 *
 * @brief Toggle all board LEDs
 *
 * @details
 * This code toggles all board LEDs at once every second and then toggles each LED one-by-one with a
 * 500 ms delay in sequence.
 *
 * @copyright Copyright (c) 2025
 */

/** REQUIRED HEADER FILES */
#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

/** MACRO DEFINITIONS */
#define LED0_NODE DT_ALIAS(led0)
#define LED1_NODE DT_ALIAS(led1)
#define LED2_NODE DT_ALIAS(led2)
#define LED3_NODE DT_ALIAS(led3)

/* Create gpio_dt_spec structure array for each LED */
static const struct gpio_dt_spec led[4] = {
	GPIO_DT_SPEC_GET(LED0_NODE, gpios), GPIO_DT_SPEC_GET(LED1_NODE, gpios),
	GPIO_DT_SPEC_GET(LED2_NODE, gpios), GPIO_DT_SPEC_GET(LED3_NODE, gpios)};

/*
 * @brief main - Application entry point
 *
 * @author Dhruv Mamtora
 * @date 02 September, 2025
 *
 * @details
 * This is the main function that initializes the GPIO pins for the LEDs and enters an infinite loop
 * to toggle the LEDs.
 *
 * @pre Ensure that the GPIO device is ready and configured correctly.
 *
 * @return -1 on error, 0 on success
 *
 * @retval 0 Success
 * @retval -1 Error
 */
int main(void)
{
	int iReturn;

	for (int iNum = 0; iNum < 4; iNum++) {
		if (!gpio_is_ready_dt(&led[iNum])) {
			return -1;
		}

		iReturn = gpio_pin_configure_dt(&led[iNum], GPIO_OUTPUT_ACTIVE);
		if (iReturn < 0) {
			return -1;
		}
	}

	while (1) {
		/* Toggle all leds every 1 second delay */
		for (int iTimesToggle = 0; iTimesToggle < 6; iTimesToggle++) {
			for (int iToggle = 0; iToggle < 4; iToggle++) {
				iReturn = gpio_pin_toggle_dt(&led[iToggle]);
				if (iReturn < 0) {
					return -1;
				}
			}
			k_msleep(1000);
		}

		/* Toggle all leds in sequence one by one with 500 ms delay */
		for (int iTimesToggle = 0; iTimesToggle < 4; iTimesToggle++) {
			for (int iToggle = 0; iToggle < 8; iToggle++) {
				iReturn = gpio_pin_toggle_dt(&led[iTimesToggle]);
				if (iReturn < 0) {
					return -1;
				}
				k_msleep(500);
			}
		}
	}
	return 0;
}
