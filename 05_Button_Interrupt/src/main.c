/*
 * Control LED Using Button (Interrupt-Based) 
 * Modify the above program to use GPIO interrupts instead of polling.
 * Configure the button GPIO as an input with interrupt on the rising/falling edge. 
 * When the button is pressed or released, 
 * toggle the LED inside the interrupt service routine (ISR).
 * 
 * 
 * Author : Dhruv Mamtora
 * Date of Creation : 01-08-2025
 * 
 */

#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
//#include <zephyr/sys/printk.h>


#define LED0_NODE DT_ALIAS(led0)
#define SW0_NODE	DT_ALIAS(sw0)

#if !DT_NODE_HAS_STATUS_OKAY(SW0_NODE)
#error "Unsupported board: sw0 devicetree alias is not defined" 
#endif


static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET_OR(SW0_NODE, gpios, 
                                        {0});
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET_OR(LED0_NODE,gpios,
                                        {0});
static struct gpio_callback button_cb_data;


void button_pressed(const struct device *dev,struct gpio_callback *cb,
            uint32_t pins)
{
    static bool led_on = true;
    led_on = !led_on;
    gpio_pin_set_dt(&led,led_on);
    printk("Button pressed: LED %s\n",led_on ? "ON" : "OFF");
}
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
    
    ret = gpio_pin_interrupt_configure_dt(&button,
					      GPIO_INT_EDGE_TO_ACTIVE);
	
    if (ret != 0) {
		printk("Error %d: failed to configure interrupt on %s pin %d\n",
			ret, button.port->name, button.pin);
		return 0;
	}
    
    gpio_init_callback(&button_cb_data, button_pressed, BIT(button.pin));
    gpio_add_callback(button.port, &button_cb_data);

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

    
    while(1) {
    }
}
