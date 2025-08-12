/*
 * LED Blinking with De-bug Logs and All LEDs Enable Zephyr’s 
 * logging system with LOG_LEVEL_DBG. Modify the blink program to:
 *      a. Blink all available user LEDs on the board.
 *      b. Add a delay of 750 milliseconds between each toggle.
 *      c. Log "LED ON" and "LED OFF" messages for each LED state change.
 * 
 * 
 * Author : Dhruv Mamtora (166834)
 * Date of Creation : 31-07-2025
 */

#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>


#define LED0_NODE DT_ALIAS(led0)
#define LED1_NODE DT_ALIAS(led1)
#define LED2_NODE DT_ALIAS(led2)
#define LED3_NODE DT_ALIAS(led3)

static const struct gpio_dt_spec led_00 = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct gpio_dt_spec led_01 = GPIO_DT_SPEC_GET(LED1_NODE, gpios);
static const struct gpio_dt_spec led_02 = GPIO_DT_SPEC_GET(LED2_NODE, gpios);
static const struct gpio_dt_spec led_03 = GPIO_DT_SPEC_GET(LED3_NODE, gpios);

LOG_MODULE_REGISTER(main,LOG_LEVEL_DBG);

int main(void)
{
    int ret;
	bool led_state = true;

	if(!gpio_is_ready_dt(&led_00) || !gpio_is_ready_dt(&led_01) || !gpio_is_ready_dt(&led_02) || !gpio_is_ready_dt(&led_03)) {
		return 0;
	}

	ret = gpio_pin_configure_dt(&led_00,GPIO_OUTPUT_ACTIVE);
	if(ret < 0) {
		return 0;
	}

	ret = gpio_pin_configure_dt(&led_01,GPIO_OUTPUT_ACTIVE);
	if(ret < 0) {
		return 0;
	}

	ret = gpio_pin_configure_dt(&led_02,GPIO_OUTPUT_ACTIVE);
	if(ret < 0) {
		return 0;
	}

	ret = gpio_pin_configure_dt(&led_03,GPIO_OUTPUT_ACTIVE);
	if(ret < 0) {
		return 0;
	}

	while(1)
    {
        led_state = !led_state;

        ret = gpio_pin_toggle_dt(&led_00);
		if(ret < 0) {
			return 0;
		}

		ret = gpio_pin_toggle_dt(&led_01);
		if(ret < 0) {
			return 0;
		}

		ret = gpio_pin_toggle_dt(&led_02);
		if(ret < 0) {
			return 0;
		}

		ret = gpio_pin_toggle_dt(&led_03);
		if(ret < 0) {
			return 0;
		}

        LOG_DBG("LED %s",led_state ? "ON" : "OFF");
        k_msleep(750);
    }
}