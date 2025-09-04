/*
 * @file main.c
 * @author Dhruv Mamtora
 * @version 0.1.0
 * @date 08 August, 2025
 *
 * @brief UART Communication - Transmit & Receive Message by Polling
 *
 * @details
 * Transmit a Welcome message via UART when board starts.
 * Wait to receive input from the user through UART in polling mode.
 * Echo back received characters.
 *
 * @copyright Copyright (c) 2025
 */

/** REQUIRED HEADER FILES */
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>

/** MACRO DEFINITIONS */
#define UART_DEVICE_NODE DT_CHOSEN(zephyr_shell_uart)

/** GLOBAL VARIABLES */
static const struct device *const uartDev = DEVICE_DT_GET(UART_DEVICE_NODE);

/*
 * @brief main - Entry point for UART Communication app.
 *
 * @author Dhruv Mamtora
 * @date 08 August, 2025
 *
 * @details
 * Transmit a Welcome message via UART when board starts.
 * Wait to receive input from the user through UART in polling mode.
 * Echo back received characters.
 *
 * @pre device node must be defined in the device tree.
 *
 * @return Return 0 on success and -1 on Error
 *
 * @retval 0 Success
 * @retval -1 Error
 */
int main(void)
{
	if (!device_is_ready(uartDev)) {
		printk("UART device not ready\n");
		return -1;
	}

	const char *welcomeMsg = "Hello, Welcome to the UART Serial Terminal !\n\r";

	/* Print welcome message */
	for (int iNum = 0; welcomeMsg[iNum] != '\0'; iNum++) {
		uart_poll_out(uartDev, welcomeMsg[iNum]);
	}

	while (1) {
		uint8_t iReceivedChar;

		/* Wait for a character to be received */
		while (uart_poll_in(uartDev, &iReceivedChar) != 0) {
			k_yield(); /* Yield CPU, don't hog in tight loop */
		}

		/* Echo the received character */
		printk("\nreceived char: ");
		uart_poll_out(uartDev, iReceivedChar);
        printk("\n");
	}

	return 0;
}
