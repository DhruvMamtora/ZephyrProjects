/*
 * Control LED Using Button (Polling-Based) 
 * Write a program that continuously polls the state of a button (GPIO input). 
 * When the button is pressed, toggle an LED.
 * The program should use a while loop and gpio_pin_get() for button polling.
 * 
 * 
 * Author : Dhruv Mamtora
 * Date of Creation : 01-08-2025
 * 
 */

#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>


#define LED0_NODE DT_ALIAS(led0)
#define SW0_NODE	DT_ALIAS(sw0)


#if !DT_NODE_HAS_STATUS_OKAY(SW0_NODE)
#error "Unsupported board: sw0 devicetree alias is not defined" 
#endif


static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET_OR(SW0_NODE, gpios, 
                                        {0});
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET_OR(LED0_NODE,gpios,
                                        {0});


int main(void)
{
    int ret;

    if(!gpio_is_ready_dt(&button)) {
        printk("Error: button device %s is not ready\n", 
            button.port->name);
        return 0;
    }

    ret = gpio_pin_configure_dt(&button, GPIO_INPUT);
    if (ret < 0) {
        printk("Error %d: failed to configure %s pin %d\n",
            ret, button.port->name, button.pin);
        return 0;
    }
        
    if(!gpio_is_ready_dt(&led)) {
        printk("Error: led device %s is not ready\n", 
            led.port->name);
        return 0;
    }
    ret = gpio_pin_configure_dt(&led,GPIO_OUTPUT_ACTIVE);
	if(ret < 0) {
        printk("Error %d: failed to configure %s pin %d\n",
            ret, led.port->name, led.pin);
	    return 0;
    }

    printk("Button and LED is Configured.\n");
    printk("Press the button\n");

    bool led_on = true;
    
    while(1) {
        int val = gpio_pin_get_dt(&button); // 0: Button pressed , 1: button released
        if(val < 0) {
            printk("failed to get button state\n");
            return 0;
        }

        if(val == 0) {  //Button presed
            k_msleep(20);   //Debounce delay
            if(gpio_pin_get_dt(&button)) {  //check if still presed
                led_on = !led_on;

                gpio_pin_set_dt(&led,led_on);
                if(led_on) {
                    printk("LED ON\n");
                }
                else {
                    printk("LED OFF\n");
                }
            }
        }
    }

}
