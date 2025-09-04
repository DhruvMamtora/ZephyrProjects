/*
 * @file main.c
 * @author Dhruv Mamtora
 * @version 0.1.0
 * @date 07 August, 2025
 *
 * @brief Thread coordination with semaphore.
 *
 * @details
 * Create two threads where one waits for a signal to start (e.g., a semaphore), and the other
 * thread gives the signal after a delay.
 *
 * @copyright Copyright (c) 2025
 */

/** REQUIRED HEADER FILES */
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

/** MACRO DEFINITIONS */
/* size of stack area used by each thread */
#define MY_STACK_SIZE 500
/* scheduling priority used by each thread */
#define MY_PRIORITY   5

/* Statically define and initialize a semaphore. */
K_SEM_DEFINE(mySemaphore, 0, 1);    /* Initial count = 0 , Limit = 1 */

/*
 * @brief threadA - Wait for signal(semaphore) to process.
 *
 * @author Dhruv Mamtora
 * @date 07 August, 2025
 *
 * @details
 * Start threadA executing. Wait for signal(semaphore). when received signal execute ThreadA.
 * Not released Semaphore.
 *
 * @syntax
 * void threadA(void);
 *
 * @return Noun.
 */
void threadA(void)
{
	while (1) {
		printk("Thread A: Started Thread A, waiting for semaphore\n");
		k_sem_take(&mySemaphore, K_FOREVER);
		printk("Thread A: Get the semaphore\n");

		for (int i = 0; i < 5; i++) {
			printk("Thread A: In progress....\n");
			k_msleep(1000);
		}

		printk("Thread A: Finished Execution.\n");
	}
}

/*
 * @brief threadB - Sleep for 5 second and then release semaphore.
 *
 * @author Dhruv Mamtora
 * @date 07 August, 2025
 *
 * @details
 * Start threadB execution. Sleep for 5 seconds then release semaphore(send signal).
 *
 * @syntax
 * void threadA(void);
 *
 * @return Noun.
 */
void threadB(void)
{
	while (1) {
		printk("Thread B: started.\n");
		k_msleep(5000);
		printk("Thread B: Give sempahire now\n");
		k_sem_give(&mySemaphore);
		printk("Thread B: Stopping..\n");
	}
}

/* Statically define and initialize a thread. */
K_THREAD_DEFINE(myTid1, MY_STACK_SIZE, threadA, NULL, NULL, NULL, MY_PRIORITY, 0, 0);
K_THREAD_DEFINE(myTid2, MY_STACK_SIZE, threadB, NULL, NULL, NULL, MY_PRIORITY, 0, 0);
