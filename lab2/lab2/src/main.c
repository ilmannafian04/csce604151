/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# "Insert system clock initialization code here" comment
 * -# Minimal main function that starts with a call to board_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include <asf.h>
#include <stdio.h>
#include <ioport.h>
#include <board.h>

#define MY_ADC ADCA
#define MY_ADC_CH ADC_CH0

static char strbuf[201];

static void adc_init(void)
{
	struct adc_config adc_conf;
	struct adc_channel_config adcch_conf;

	adc_read_configuration(&MY_ADC, &adc_conf);
	adcch_read_configuration(&MY_ADC, MY_ADC_CH, &adcch_conf);

	adc_set_conversion_parameters(&adc_conf, ADC_SIGN_OFF, ADC_RES_12, ADC_REF_VCC);
	adc_set_conversion_trigger(&adc_conf, ADC_TRIG_MANUAL, 1, 0);
	adc_set_clock_rate(&adc_conf, 200000UL);

	adcch_set_input(&adcch_conf, J2_PIN0, ADCCH_NEG_NONE, 1);

	adc_write_configuration(&MY_ADC, &adc_conf);
	adcch_write_configuration(&MY_ADC, MY_ADC_CH, &adcch_conf);
}

static uint16_t adc_read()
{
	uint16_t result;
	adc_enable(&MY_ADC);
	adc_start_conversion(&MY_ADC, MY_ADC_CH);
	adc_wait_for_interrupt_flag(&MY_ADC, MY_ADC_CH);
	result = adc_get_result(&MY_ADC, MY_ADC_CH);
	return result;
}

void PWM_Init(void)
{
	/* Set output */
	PORTC.DIR |= PIN0_bm;

	/* Set Register */
	TCC0.CTRLA = (PIN2_bm) | (PIN0_bm);
	TCC0.CTRLB = (PIN4_bm) | (PIN2_bm) | (PIN1_bm);

	/* Set Period */
	TCC0.PER = 10000;

	/* Set Compare Register value*/
	TCC0.CCA = 0;
}

int main(void)
{
	/* Insert system clock initialization code here (sysclk_init()). */

	board_init();

	gfx_mono_init();

	adc_init();

	PWM_Init();

	uint32_t result;
	uint32_t max = 2000;
	uint32_t min = 2000;
	uint32_t period = 10000;
	uint32_t final = 0;

	ioport_set_pin_level(LCD_BACKLIGHT_ENABLE_PIN, 1);

	while (1)
	{
		result = adc_read();

		snprintf(strbuf, sizeof(strbuf), "ADC: %4lu", result);
		gfx_mono_draw_string(strbuf, 0, 0, &sysfont);

		if (result > max)
		{
			max = result;
		}
		else if (result < min)
		{
			min = result;
		}

		snprintf(strbuf, sizeof(strbuf), "Max: %4lu Min: %4lu", max, min);
		gfx_mono_draw_string(strbuf, 0, 8, &sysfont);

		uint32_t result_normalized = (result - min) * period;
		final = result_normalized / (max - min);

		TCC0.CCA = final;

		snprintf(strbuf, sizeof(strbuf), "PWM: %5lu/10000", final);
		gfx_mono_draw_string(strbuf, 0, 16, &sysfont);

		delay_ms(50);
	}

	/* Insert application code here, after the board has been initialized. */
}