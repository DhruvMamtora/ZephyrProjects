/*
 * @file main.c
 * @author Dhruv Mamtora
 * @version 0.1.0
 * @date  27 August, 2025
 *
 * @brief Main application file for Sensor Data Logging System.
 *
 * @details
 * This file contains the main function.
 *
 * @copyright Copyright (c) 2025
 */

/** REQUIRED HEADER FILES */
#include <zephyr/logging/log.h>
#include <zephyr/kernel.h>

/* Register the logging module for main application */
LOG_MODULE_REGISTER(main);

/*
 * @brief main - Main function.
 *
 * @author Dhruv Mamtora
 * @date 26 August, 2025
 *
 * @details
 * This function logs a startup message and returns.
 *
 * @return 0 on success.
 */
int main(void)
{
	LOG_INF("Sensor Data Logging System started.");
	return 0;
}
