/*
 * Basic LED Blinking Program Write a program to blink a single user-controllable LED on 
 * your board (e.g., LED0). Configure the GPIO pin using Zephyr’s device tree bindings. 
 * Blink the LED with a fixed interval (e.g., 500 ms ON, 500 ms OFF).
 * 
 * 
 * Author : Dhruv Mamtora (166834)
 * Date of Creation : 31-07-2025
 * 
 */

#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

#define LED0_NODE DT_ALIAS(led0)

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

int main(void) 
{
    if(!gpio_is_ready_dt(&led)) {
        return 0;
    }

    if(gpio_pin_configure_dt(&led,GPIO_OUTPUT_ACTIVE) < 0) {
        return 0;
    }
    
    while(1)
    {
        if(gpio_pin_toggle_dt(&led) < 0) {
            return 0;
        }

        k_msleep(500);
    }

}
