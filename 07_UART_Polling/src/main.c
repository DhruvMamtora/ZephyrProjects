/*
 * 7. UART Communication – Transmit & Receive Message (Polling) Write a UART program to:
 *      a. Transmit a welcome message via UART when the board starts.
 *      b. Wait to receive input from the user through UART in polling mode.
 *      c. Echo back the received characters.
 * 
 * Author: Dhruv Mamtora
 * Date: 08th August 2025
 */
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>


#define UART_DEVICE_NODE DT_CHOSEN(zephyr_shell_uart)

static const struct device *const uart_dev = DEVICE_DT_GET(UART_DEVICE_NODE);

int main(void)
{
    if(!device_is_ready(uart_dev)) {
        printk("UART device not ready\n");
        return 0;
    }

    const char *welcome_msg = "Hello, Welcome to the UART Serial Terminal !\n\r";

    /* Print welcome message */
    for (int i = 0; welcome_msg[i] != '\0'; i++)
    {
        uart_poll_out(uart_dev, welcome_msg[i]);
    }

    while(1) {
        uint8_t received_char;

        /* Wait for a character to be received */
        while (uart_poll_in(uart_dev, &received_char) != 0) {
            k_yield();  // Yield CPU, don't hog in tight loop
        }

        /* Echo the received character */
        printk("\nreceived char: ");
        uart_poll_out(uart_dev, received_char);
    }
}