/*
 * @file main.c
 * @author Dhruv Mamtora
 * @version 0.1.0
 * @date 12 August, 2025
 *
 * @brief Ping-Pong thread using semaphore.
 *
 * @details
 * Ping-Pong thread using 2 semaphore strictly alternate their execution.
 *
 * @copyright Copyright (c) 2025
 */

/** REQUIRED HEADER FILES */
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

/** MACRO DEFINITIONS */
#define MY_STACK_SIZE 500
#define MY_PRIORITY   5

/** GLOBAL VARIABLES */
/* Statically define and initialize a semaphore. */
/* When we take semaphore count = 1 -> 0, when we give semaphore count = 0 -> 1*/
K_SEM_DEFINE(mySem1, 1, 1); /* Initial count = 1, Limit = 1 */
K_SEM_DEFINE(mySem2, 0, 1); /* Initial count = 0, Limit = 1*/

/*
 * @brief myEntryPoint1 - Take mySem1 and print on console.
 *
 * @author Dhruv Mamtora
 * @date 12 August, 2025
 *
 * @details
 * Take mySem1 and print on console. Wait few second and release mySem2.
 *
 * @pre semaphore must be defined and initialised.
 *
 * @return None.
 */
void myEntryPoint1(void)
{
	while (1) {
		k_sem_take(&mySem1, K_FOREVER);
		printk("Ping\n");
		k_msleep(1000);
		k_sem_give(&mySem2);
	}
}

/*
 * @brief myEntryPoint2 - Take mySem1 and print on console.
 *
 * @author Dhruv Mamtora
 * @date 12 August, 2025
 *
 * @details
 * Take mySem1 and print on console. Wait few second and release mySem2.
 *
 * @pre semaphore must be defined and initialised.
 *
 * @return None.
 */
void myEntryPoint2(void)
{
	while (1) {
		k_sem_take(&mySem2, K_FOREVER);
		printk("\t\tPong\n");
		k_msleep(1000);
		k_sem_give(&mySem1);
	}
}

/* Statically define and initialize a thread. */
K_THREAD_DEFINE(my_tid1, MY_STACK_SIZE, myEntryPoint1, NULL, NULL, NULL, MY_PRIORITY, 0, 0);
K_THREAD_DEFINE(my_tid2, MY_STACK_SIZE, myEntryPoint2, NULL, NULL, NULL, MY_PRIORITY, 0, 0);
