/*
 * @file main.c
 * @author Dhruv Mamtora
 * @version 0.1.0
 * @date 08 August, 2025
 *
 * @brief UART Communication - Transmit & Receive Message by Interrupt
 *
 * @details
 * Transmit a welcome message via UART when the board starts.
 * Wait to receive input from the user through UART in polling mode.
 * Echo back received characters.
 *
 * @copyright Copyright (c) 2025
 */

/** REQUIRED HEADER FILES */
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>

#include <string.h>

/** MACRO DEFINITIONS */
#define UART_DEVICE_NODE DT_CHOSEN(zephyr_shell_uart)
#define MSG_SIZE         32

/* Message queue to store up to 10 messages (aligned to 4-byte boundary) */
K_MSGQ_DEFINE(uartMsgq, MSG_SIZE, 10, 4);

/** GLOBAL VARIABLES */
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
	uint8_t cChar;

	if (!uart_irq_update(uartDev)) {
		return;
	}

	if (!uart_irq_rx_ready(uartDev)) {
		return;
	}

	/* read until FIFO empty */
	while (uart_fifo_read(uartDev, &cChar, 1) == 1) {
		if ((cChar == '\n' || cChar == '\r') && rxBufPos > 0) {
			/* terminate string */
			rxBuf[rxBufPos] = '\0';

			/* if queue is full, message is silently dropped */
			k_msgq_put(&uartMsgq, &rxBuf, K_NO_WAIT);

			/* reset the buffer (it was copied to the msgq) */
			rxBufPos = 0;
		} else if (rxBufPos < (sizeof(rxBuf) - 1)) {
			rxBuf[rxBufPos++] = cChar;
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
 * Transmit a Welcome message via UART when board starts.
 * Wait to receive input from the user through UART in interrupt mode.
 * Echo back received characters.
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
	char cTxBuf[MSG_SIZE];

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

	/* Print welcome message */
	printUart((char *)welcomeMsg);

	/* wait for messages from the UART */
	while (k_msgq_get(&uartMsgq, &cTxBuf, K_FOREVER) == 0) {
		printUart("Echo: ");
		printUart(cTxBuf);
		printUart("\r\n");
	}

	return 0;
}
