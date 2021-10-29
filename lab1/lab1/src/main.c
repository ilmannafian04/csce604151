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

static char strbuf[128];

int main (void)
{
	board_init();
	gfx_mono_init();

	gpio_set_pin_high(LCD_BACKLIGHT_ENABLE_PIN);
	ioport_set_pin_dir(J1_PIN0, IOPORT_DIR_OUTPUT);

	gfx_mono_draw_string("Embedded System Lab 1", 0, 0, &sysfont);
	gfx_mono_draw_string("Ilman - 1706067626", 0, 8, &sysfont);

	bool buzzer_led_is_on = false;

	int counter = 0;
	bool button_1_was_pressed = false;
	bool button_2_was_pressed = false;
	bool counter_changed = true;

	while(1) {
		// hold button 0 to turn on led 0 and buzzer
		uint64_t button_0_level = ioport_get_pin_level(GPIO_PUSH_BUTTON_0);
		if (button_0_level == 0 && !buzzer_led_is_on) {
			ioport_set_pin_level(LED2_GPIO, 0);
			ioport_set_pin_level(J1_PIN0, 1);
			buzzer_led_is_on = true;
		} else if (button_0_level != 0 && buzzer_led_is_on) {
			ioport_set_pin_level(LED2_GPIO, 1);
			ioport_set_pin_level(J1_PIN0, 0);
			buzzer_led_is_on = false;
		}

		// increment counter
		uint64_t button_1_current_level = ioport_get_pin_level(GPIO_PUSH_BUTTON_1);
		if (button_1_current_level == 0 && !button_1_was_pressed) {
			button_1_was_pressed = true;
		} else if (button_1_current_level != 0 && button_1_was_pressed) {
			counter += 1;
			counter_changed = true;
			button_1_was_pressed = false;
			ioport_set_pin_level(LED0_GPIO, 0);
			ioport_set_pin_level(LED1_GPIO, 1);
			gfx_mono_draw_string("Increment", 0, 24, &sysfont);
		}
		// decrement counter
		uint64_t button_2_current_level = ioport_get_pin_level(GPIO_PUSH_BUTTON_2);
		if (button_2_current_level == 0 && !button_2_was_pressed) {
			button_2_was_pressed = true;
		} else if (button_2_current_level != 0 && button_2_was_pressed) {
			counter -= 1;
			counter_changed = true;
			button_2_was_pressed = false;
			ioport_set_pin_level(LED0_GPIO, 1);
			ioport_set_pin_level(LED1_GPIO, 0);
			gfx_mono_draw_string("Decrement", 0, 24, &sysfont);
		}
		if (counter_changed) {
			snprintf(strbuf, sizeof(strbuf), "Counter: %3d", counter);
			gfx_mono_draw_string(strbuf, 0, 16, &sysfont);
			counter_changed = false;
		}
	}
}
