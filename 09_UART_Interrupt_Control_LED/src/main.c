/*
 * @file main.c
 * @author Dhruv Mamtora
 * @version 0.1.0
 * @date 08 August, 2025
 *
 * @brief Control LED via UART Commands
 *
 * @details
 * Create a simple UART command parser:
 * a. Accept commands like "LED ON\n", "LED OFF\n", "TOGGLE\n".
 * b. Based on the command, turn ON, OFF, or TOGGLE an LED.
 * c. Use UART reception either via polling or interrupts.
 *
 * @copyright Copyright (c) 2025
 */

/** REQUIRED HEADER FILES */
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/drivers/gpio.h>

#include <string.h>

/** MACRO DEFINITIONS */
#define LED0_NODE        DT_ALIAS(led0)
#define UART_DEVICE_NODE DT_CHOSEN(zephyr_shell_uart)

#define MSG_SIZE 32

/* Message queue to store up to 10 messages (aligned to 4-byte boundary) */
K_MSGQ_DEFINE(uartMsgq, MSG_SIZE, 10, 4);

/** GLOBAL VARIABLES */
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct device *const uartDev = DEVICE_DT_GET(UART_DEVICE_NODE);

/* receive buffer used in UART ISR callback */
static char rxBuf[MSG_SIZE];
static int rxBufPos;

/*
 * @brief printUart - Print character to console
 *
 * @author Dhruv Mamtora
 * @date 08 August, 2025
 *
 * @details
 * Print character to console.
 * Send buffer data character by character via uart.
 *
 * @pre Uart device must be configured and initialised.
 *
 * @syntax
 * void printUart(char *cBuf);
 *
 * @param[in] cBuf Pointer to char Buffer
 *
 * @return None
 */
void printUart(char *cBuf)
{
	int iMsgLen = strlen(cBuf);

	for (int iNum = 0; iNum < iMsgLen; iNum++) {
		uart_poll_out(uartDev, cBuf[iNum]);
	}
}

/*
 * @brief readUart - UART RX Interrupt Callback
 *
 * @author Dhruv Mamtora
 * @date 08 August, 2025
 *
 * @details
 * UART RX Interrupt Callback to read characters from UART until line end is detected.
 * Afterwards push the data to the message queue.
 *
 * @pre Uart device must be configured and initialised.
 *
 * @syntax
 * void readUart(const struct device *dev, void *user_data);
 *
 * @param[in] dev Pointer to UART device structure.
 * @param[in] user_data Pointer to user data (not used here).
 *
 * @return None
 */
void readUart(const struct device *dev, void *user_data)
{
	uint8_t c;

	if (!uart_irq_update(uartDev)) {
		return;
	}

	if (!uart_irq_rx_ready(uartDev)) {
		return;
	}

	/* read until FIFO empty */
	while (uart_fifo_read(uartDev, &c, 1) == 1) {
		if ((c == '\n' || c == '\r') && rxBufPos > 0) {
			/* terminate string */
			rxBuf[rxBufPos] = '\0';

			/* if queue is full, message is silently dropped */
			k_msgq_put(&uartMsgq, &rxBuf, K_NO_WAIT);

			/* reset the buffer (it was copied to the msgq) */
			rxBufPos = 0;
		} else if (rxBufPos < (sizeof(rxBuf) - 1)) {
			rxBuf[rxBufPos++] = c;
		}
		/* else: characters beyond buffer size are dropped */
	}
}

/*
 * @brief main - Entry point for UART Communication app.
 *
 * @author Dhruv Mamtora
 * @date 08 August, 2025
 *
 * @details
 * Initialize UART and GPIO devices, set up UART interrupt handling,
 * and process incoming UART messages to control an LED.
 * Supported commands are "LED ON", "LED OFF", and "TOGGLE".
 * The application echoes received messages back to the sender.
 *
 * @pre device node must be defined in the device tree.
 *
 * @return Return 0 on success and -1 on Error
 *
 * @retval 0 Success
 * @retval -1 Error code description
 *
 * @error
 * -ENOTSUP	Interrupt-driven UART API support not enabled
 * -ENOSYS	UART device does not support interrupt-driven API
 */
int main(void)
{
	char txBuf[MSG_SIZE];

	if (!gpio_is_ready_dt(&led)) {
		return -1;
	}

	if (gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE) < 0) {
		return -1;
	}

	if (!device_is_ready(uartDev)) {
		printk("UART device not ready\n");
		return -1;
	}

	/* configure interrupt and callback to receive data */
	int iReturn = uart_irq_callback_user_data_set(uartDev, readUart, NULL);

	if (iReturn < 0) {
		if (iReturn == -ENOTSUP) {
			printk("Interrupt-driven UART API support not enabled\n");
		} else if (iReturn == -ENOSYS) {
			printk("UART device does not support interrupt-driven API\n");
		} else {
			printk("Error setting UART callback: %d\n", iReturn);
		}
		return -1;
	}

	const char *welcomeMsg = "Hello, Welcome to the UART Serial Terminal !\n\r";

	uart_irq_rx_enable(uartDev);

	printUart((char *)welcomeMsg);

	/* wait for messages from the UART */
	while (k_msgq_get(&uartMsgq, &txBuf, K_FOREVER) == 0) {
		printUart("Echo: ");
		printUart(txBuf);
		printUart("\r\n");

		if (strcmp(txBuf, "LED ON") == 0) {
			gpio_pin_set_dt(&led, 1); /* Turn ON LED */
		} else if (strcmp(txBuf, "LED OFF") == 0) {
			gpio_pin_set_dt(&led, 0); /* Turn OFF LED */
		} else if (strcmp(txBuf, "TOGGLE") == 0) {
			gpio_pin_toggle_dt(&led); /* TOGGLE LED state*/
		} else {
			printUart("Unknown command! write 'LED ON', 'LED OFF', 'TOGGLE'\r\n");
		}
		k_msleep(1000); /* Small delay to avoid flooding the UART */
	}
	return 0;
}
