#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>


#define MY_STACK_SIZE 500
#define MY_PRIORITY 5

/* Statically define and initialize a semaphore. */
K_SEM_DEFINE(my_sem1, 1, 1);
K_SEM_DEFINE(my_sem2, 0, 1);

void my_entry_point1(void)
{
    while (1) {
        k_sem_take(&my_sem1, K_FOREVER);
        printk("Ping\n");
        k_msleep(1000);
        k_sem_give(&my_sem2);
    }
}

void my_entry_point2(void)
{
    while (1) {
        k_sem_take(&my_sem2, K_FOREVER);
        printk("\t\tPong\n");
        k_msleep(1000);
        k_sem_give(&my_sem1);
    }
}

/* Statically define and initialize a thread. */
K_THREAD_DEFINE(my_tid1, MY_STACK_SIZE,
                my_entry_point1, NULL, NULL, NULL,
                MY_PRIORITY, 0, 0);

K_THREAD_DEFINE(my_tid2, MY_STACK_SIZE,
                my_entry_point2, NULL, NULL, NULL,
                MY_PRIORITY, 0, 0);