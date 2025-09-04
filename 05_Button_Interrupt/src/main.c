/*
 * @file main.c
 * @author Dhruv Mamtora
 * @version 0.1.0
 * @date  01 August, 2025
 *
 * @brief Control LED Using Button (Interrupt Based)
 *
 * @details
 * Configured the Button GPIO as an Input with interrupt on the rising/falling edge.
 * When the button is pressed LED Toggle.
 * toggle the LED inside the interrupt service routine (ISR).
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
static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET_OR(SW0_NODE, gpios, {0});
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET_OR(LED0_NODE, gpios, {0});
static struct gpio_callback button_cb_data;

/*
 * @brief buttonPressed - set LED ON/OFF based on flag(button press)
 *
 * @author Dhruv Mamtora
 * @date 01 August, 2025
 *
 * @details
 * Bydefault LED is ON it Toggle the stat of Flag and set the value on LED.
 * print the status of LED on console.
 *
 * @pre led should be configured as GPIO_OUTPUT_ACTIVE.
 *
 * @syntax
 * void buttonPressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
 *
 * @param[in] dev Pointer to the device structure for the driver instance.
 * @param[in] cb Pointer to the callback structure.
 * @param[in] pins Bitmask of the pins that triggered the interrupt.
 *
 * @return None
 */
void buttonPressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	static bool bLedOn = true;
	bLedOn = !bLedOn;
	gpio_pin_set_dt(&led, bLedOn);
	printk("Button pressed: LED %s\n", bLedOn ? "ON" : "OFF");
}

/*
 * @brief main - Application entry point
 *
 * @author Dhruv Mamtora
 * @date 03 September, 2025
 *
 * @details
 * configured the button GPIO as Input with interrupt on the rising edge.
 * Press Button to LED Toggle.
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

	/* Configure the interrupt with Rising Edge(on Button Press) */
	iReturn = gpio_pin_interrupt_configure_dt(&button, GPIO_INT_EDGE_TO_ACTIVE);
	if (iReturn != 0) {
		printk("Error %d: failed to configure interrupt on %s pin %d\n", iReturn,
		       button.port->name, button.pin);
		return -1;
	}

	/* Initialize the static callback structure */
	gpio_init_callback(&button_cb_data, buttonPressed, BIT(button.pin));
	/* Add the callback function to the GPIO device */
	gpio_add_callback(button.port, &button_cb_data);

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
	}

	return 0;
}
