/*
 * 11. Mutual Exclusion with Shared Counter 
 *      Create two threads that increment a shared counter 1000 times.
 *      Use synchronization to prevent data corruption and ensure the final counter value is correct.
 *  
 * Author: Dhruv Mamtora
 * Date: 11th August 2025
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>


/* size of stack area used by each thread */
#define MY_STACK_SIZE 1024

/* scheduling priority used by each thread */
#define MY_PRIORITY 5
#define MY_INCREMENT 100000
int counter = 0;

/* Define Mutex */
K_MUTEX_DEFINE(counter_mutex);

/* Define thread stacks */
K_THREAD_STACK_DEFINE(stack1, MY_STACK_SIZE);
K_THREAD_STACK_DEFINE(stack2, MY_STACK_SIZE);

/* Thread control blocks */
struct k_thread thread1;
struct k_thread thread2;

void increment_counter1(void)
{
    for (int i = 0; i < MY_INCREMENT; i++) {
        k_mutex_lock(&counter_mutex, K_FOREVER);
        counter++;
        k_mutex_unlock(&counter_mutex);
    }
    printk("Thread 1 finished incrementing counter1. counter = %d \n",counter);
}

void increment_counter2(void)
{
    for (int i = 0; i < MY_INCREMENT; i++) {
        k_mutex_lock(&counter_mutex, K_FOREVER);
        counter++;
        k_mutex_unlock(&counter_mutex);
    }
    printk("Thread 2 finished incrementing counter2. counter = %d \n",counter);
}

int main(void)
{
    k_mutex_init(&counter_mutex);

    k_thread_create(&thread1, stack1, MY_STACK_SIZE,
                    increment_counter1, NULL, NULL, NULL,
                    MY_PRIORITY, 0, K_NO_WAIT);

    
    k_thread_create(&thread2, stack2, MY_STACK_SIZE,
                    increment_counter2, NULL, NULL, NULL,
                    MY_PRIORITY, 0, K_NO_WAIT);

    /* Wait for both threads to finish */
    k_thread_join(&thread1, K_FOREVER);
    k_thread_join(&thread2, K_FOREVER);

    printk("Final counter value: %d\n", counter);

    return 0;
}