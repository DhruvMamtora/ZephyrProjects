/*
 * @file main.c
 * @author Dhruv Mamtora
 * @version 0.1.0
 * @date 05 August, 2025
 *
 * @brief  PWM-based LED blinking application
 *
 * @details
 * This file contains the implementation of a PWM blinking application
 * for two LEDs. It initializes PWM devices, calibrates the maximum period
 * supported, and toggles the blinking frequencies of the LEDs in a loop
 * to create a dynamic lighting effect.
 *
 * @copyright Copyright (c) 2025
 */

/** REQUIRED HEADER FILES */
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/device.h>
#include <zephyr/drivers/pwm.h>

/** MACRO DEFINITIONS */
#define MIN_PERIOD PWM_SEC(1U) / 128U
#define MAX_PERIOD PWM_SEC(1U)

/** GLOBAL VARIABLES */
static const struct pwm_dt_spec pwm_led0 = PWM_DT_SPEC_GET(DT_ALIAS(pwm_led0));
static const struct pwm_dt_spec pwm_led1 = PWM_DT_SPEC_GET(DT_ALIAS(pwm_led1));

/*
 * @brief main - Entry point for PWM-based LED blinking application.
 *
 * @author Dhruv Mamtora
 * @date 05 August, 2025
 *
 * @details
 * This function initializes two PWM devices and generates a blinking effect with varying frequency
 * on two LEDs using PWM. One LED blinks faster as the other blinks slower, creating a dynamic
 * visual effect. The program calibrates the maximum usable PWM period and then continuously
 * alternates the duty cycle and period of the PWM signals in a loop.
 *
 * @pre PWM device aliases must be defined in the device tree.
 *
 * @return return 0 on success and -1 on Error
 *
 * @retval 0 Success
 * @retval -1 Error
 *
 * @remarks
 * - The application uses `pwm_set_dt()` for setting PWM signal parameters.
 * - The period of LED0 increases/decreases exponentially, and LED1 is inverted
 *   to maintain a contrast in blinking rates.
 */
int main(void)
{
	uint32_t iMaxPeriod;
	uint32_t iPeriod;
	uint32_t iInvertPeriod;
	uint8_t iDir = 0U;
	int iReturn;

	printk("PWM-based blinky\n");

	if (!pwm_is_ready_dt(&pwm_led0)) {
		printk("Error: PWM device %s is not ready\n", pwm_led0.dev->name);
		return -1;
	}

	if (!pwm_is_ready_dt(&pwm_led1)) {
		printk("Error: PWM device %s is not ready\n", pwm_led1.dev->name);
		return -1;
	}

	/*
	 * In case the default MAX_PERIOD value cannot be set for
	 * some PWM hardware, decrease its value until it can.
	 *
	 * Keep its value at least MIN_PERIOD * 4 to make sure
	 * the sample changes frequency at least once.
	 */
	printk("Calibrating for channel %d...\n", pwm_led0.channel);
	iMaxPeriod = MAX_PERIOD;
	/* pwm_led0 set to Max period (slow blink) */
	/* pwm_led1 set to Min period (fast blink) */
	while (pwm_set_dt(&pwm_led0, iMaxPeriod, iMaxPeriod / 2U) ||
	       pwm_set_dt(&pwm_led1, MIN_PERIOD, MIN_PERIOD / 2U)) {
		iMaxPeriod /= 2U;
		if (iMaxPeriod < (4U * MIN_PERIOD)) {
			printk("Error: PWM device "
			       "does not support a period at least %lu\n",
			       4U * MIN_PERIOD);
			return -1;
		}
	}

	printk("Done calibrating; maximum/minimum periods %u/%lu nsec\n", iMaxPeriod, MIN_PERIOD);

	iPeriod = iMaxPeriod;
	while (1) {

		iInvertPeriod = iMaxPeriod + MIN_PERIOD - iPeriod;

		iReturn = pwm_set_dt(&pwm_led0, iPeriod, iPeriod / 2U);
		if (iReturn) {
			printk("Error %d: failed to set pulse width\n", iReturn);
			return 0;
		}

		iReturn = pwm_set_dt(&pwm_led1, iInvertPeriod, iInvertPeriod / 2U);
		if (iReturn) {
			printk("Error %d: failed to set pulse width\n", iReturn);
			return 0;
		}

		printk("&pwm_led0 iPeriod = %d\t\t&pwm_led1 iPeriod = %d \n", iPeriod,
		       iInvertPeriod);

		iPeriod = iDir ? (iPeriod * 2U) : (iPeriod / 2U);
		if (iPeriod > iMaxPeriod) {
			iPeriod = iMaxPeriod / 2U;
			iDir = 0U;
		} else if (iPeriod < MIN_PERIOD) {
			iPeriod = MIN_PERIOD * 2U;
			iDir = 1U;
		}

		k_sleep(K_SECONDS(4U));
	}
	return 0;
}
