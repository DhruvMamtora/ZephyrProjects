/*
 * @file main.c
 * @author Dhruv Mamtora
 * @version 0.1.0
 * @date 01 August, 2025
 *
 * @brief Control LED Using Button (Polling-Based)
 *
 * @details
 * Write a program that continuously polls the state of a button (GPIO input).
 * When the button is pressed, toggle an LED.
 *
 * @copyright Copyright (c) 2025
 */

/** REQUIRED HEADER FILES */
#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>

/** MACRO DEFINITIONS */
#define LED0_NODE DT_ALIAS(led0)
#define SW0_NODE  DT_ALIAS(sw0)

/* Check if the node is available */
#if !DT_NODE_HAS_STATUS_OKAY(LED0_NODE)
#error "Unsupported board: led0 devicetree alias is not defined"
#endif

#if !DT_NODE_HAS_STATUS_OKAY(SW0_NODE)
#error "Unsupported board: sw0 devicetree alias is not defined"
#endif

/** GLOBAL VARIABLES */
/* Get the GPIO device structure from devicetree */
static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET_OR(SW0_NODE, gpios, {0});
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET_OR(LED0_NODE, gpios, {0});

/*
 * @brief main - Application entry point
 *
 * @author Dhruv Mamtora
 * @date 01 August, 2025
 *
 * @details
 * Button is polled continuously in a while loop. When the button is pressed,
 * toggle an LED.
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
	bool bLedOn = true;

	if (!gpio_is_ready_dt(&button)) {
		printk("Error: button device %s is not ready\n", button.port->name);
		return -1;
	}

	iReturn = gpio_pin_configure_dt(&button, GPIO_INPUT);
	if (iReturn < 0) {
		printk("Error %d: failed to configure %s pin %d\n", iReturn, button.port->name,
		       button.pin);
		return -1;
	}

	if (!gpio_is_ready_dt(&led)) {
		printk("Error: led device %s is not ready\n", led.port->name);
		return -1;
	}
	iReturn = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
	if (iReturn < 0) {
		printk("Error %d: failed to configure %s pin %d\n", iReturn, led.port->name,
		       led.pin);
		return -1;
	}

	printk("Button and LED is Configured.\n");
	printk("Press the button\n");

	while (1) {
		int iVal = gpio_pin_get_dt(&button); /* 0: Button pressed , 1: button released */
		if (iVal < 0) {
			printk("failed to get button state\n");
			return -1;
		}

		if (iVal == 0) {                    /* Button pressed */
			k_msleep(20);                   /* Debounce delay */
			if (gpio_pin_get_dt(&button)) { /* check if still pressed */
				bLedOn = !bLedOn;

				gpio_pin_set_dt(&led, bLedOn);
				if (bLedOn) {
					printk("LED ON\n");
				} else {
					printk("LED OFF\n");
				}
			}
		}
	}
	return 0;
}
