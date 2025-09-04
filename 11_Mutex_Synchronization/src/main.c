/*
 * 11. Mutual Exclusion with Shared Counter
 *      Create two threads that increment a shared counter 1000 times.
 *      Use synchronization to prevent data corruption and ensure the final counter value is
 * correct.
 *
 * Author: Dhruv Mamtora
 * Date: 11th August 2025
 */
/*
 * @file main.c
 * @author Dhruv Mamtora
 * @version 0.1.0
 * @date 11 August, 2025
 *
 * @brief Mutual Exclusion with shared counter
 *
 * @details
 * Created 2 threads that increment same counter 100times.
 * Used Mutex to synchronisation and prevent data corruption.
 *
 * @copyright Copyright (c) 2025
 */

/** REQUIRED HEADER FILES */
#include <zephyr/kernel.h>
#include <zephyr/device.h>

/** MACRO DEFINITIONS */
/* size of stack area used by each thread */
#define MY_STACK_SIZE 1024
/* scheduling priority used by each thread */
#define MY_PRIORITY   5
#define MY_INCREMENT  100000

/** GLOBAL VARIABLES */
int iCounter = 0;
/* Define Mutex */
K_MUTEX_DEFINE(counterMutex);

/* Define thread stacks */
K_THREAD_STACK_DEFINE(stack1, MY_STACK_SIZE);
K_THREAD_STACK_DEFINE(stack2, MY_STACK_SIZE);

/* Thread control blocks */
struct k_thread thread1;
struct k_thread thread2;

/*
 * @brief incrementCounter1 - Increment counter with Mutex
 *
 * @author Dhruv Mamtora
 * @date 11 August, 2025
 *
 * @details
 * Increment the shared counter with the use of mutex to prevent data corruption.
 *
 * @pre Mutex must be defined and initialised.
 *
 * @return None.
 *
 * @global
 * iCounter increments.
 */
void incrementCounter1(void *a, void *b, void *C)
{
	for (int iNum = 0; iNum < MY_INCREMENT; iNum++) {
		k_mutex_lock(&counterMutex, K_FOREVER);
		iCounter++;
		k_mutex_unlock(&counterMutex);
	}
	printk("Thread 1 finished incrementing counter1. counter = %d \n", iCounter);
}

/*
 * @brief incrementCounter2 - Increment counter with Mutex
 *
 * @author Dhruv Mamtora
 * @date 11 August, 2025
 *
 * @details
 * Increment the shared counter with the use of mutex to prevent data corruption.
 *
 * @pre Mutex must be defined and initialised.
 *
 * @return None.
 *
 * @global
 * iCounter increments.
 */
void incrementCounter2(void *a, void *b, void *C)
{
	for (int iNum = 0; iNum < MY_INCREMENT; iNum++) {
		k_mutex_lock(&counterMutex, K_FOREVER);
		iCounter++;
		k_mutex_unlock(&counterMutex);
	}
	printk("Thread 2 finished incrementing counter2. counter = %d \n", iCounter);
}

/*
 * @brief main - Main entry point for application.
 *
 * @author Dhruv Mamtora
 * @date 04 September, 2025
 *
 * @details
 * Initialise Mutex and create threads. Wait for final counter number.
 *
 * @return 0.
 *
 * @retval 0 Success
 */
int main(void)
{
	k_mutex_init(&counterMutex);

	k_thread_create(&thread1, stack1, MY_STACK_SIZE, incrementCounter1, NULL, NULL, NULL,
			MY_PRIORITY, 0, K_NO_WAIT);

	k_thread_create(&thread2, stack2, MY_STACK_SIZE, incrementCounter2, NULL, NULL, NULL,
			MY_PRIORITY, 0, K_NO_WAIT);

	/* Wait for both threads to finish */
	k_thread_join(&thread1, K_FOREVER);
	k_thread_join(&thread2, K_FOREVER);

	printk("Final counter value: %d\n", iCounter);

	return 0;
}
