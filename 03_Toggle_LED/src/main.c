/*
 * Toggle LEDs Simultaneously and Sequentially Write a program to:
 *      a. First toggle all board LEDs at once every second.
 *      b. Then toggle each LED one-by-one with a 500 ms delay in sequence.
 *      c. Use k_sleep() or k_msleep() to manage delays.
 * 
 * 
 * Author : Dhruv Mamtora (166834)
 * Date of Creation : 31-07-2025
 */

#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>


#define LED0_NODE DT_ALIAS(led0)
#define LED1_NODE DT_ALIAS(led1)
#define LED2_NODE DT_ALIAS(led2)
#define LED3_NODE DT_ALIAS(led3)

static const struct gpio_dt_spec led[4] = {
                                            GPIO_DT_SPEC_GET(LED0_NODE, gpios), 
                                            GPIO_DT_SPEC_GET(LED1_NODE, gpios),
                                            GPIO_DT_SPEC_GET(LED2_NODE, gpios),
                                            GPIO_DT_SPEC_GET(LED3_NODE, gpios)
                                        };


int main(void)
{
    int ret;

    for(int i = 0 ; i < 4 ; i++) {
        if(!gpio_is_ready_dt(&led[i])) {
            return 0;
        }
    
        ret = gpio_pin_configure_dt(&led[i],GPIO_OUTPUT_ACTIVE);
	    if(ret < 0) {
		    return 0;
	    }
    }

	while(1)
    {
        //toggle all leds every 1 second delay
        for (int j = 0 ; j < 6 ; j++) {
            for (int i = 0 ; i < 4 ; i++) {
                ret = gpio_pin_toggle_dt(&led[i]);
	    	    if(ret < 0) {
		    	    return 0;
		        }
            }
            k_msleep(1000);
        }
        
        //toggle all leds in sequence one by one with 500 ms delay
        for (int j = 0 ; j < 4 ; j++) {
            for (int i = 0 ; i < 8 ; i++) {
                ret = gpio_pin_toggle_dt(&led[j]);
	    	    if(ret < 0) {
		    	    return 0;
		        }
                k_msleep(500);
            }
        }

    }
}