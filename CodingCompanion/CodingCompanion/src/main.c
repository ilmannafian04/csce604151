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
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */
#include <asf.h>
#include <stdio.h>
#include <math.h>
#include <adc_sensors/adc_sensors.h>

static char strbuf[128];

#define LIGHT_Y 6 * 1
#define LIGHT_THRESHOLD_MINOR 300
#define LIGHT_THRESHOLD_MAJOR 100
#define SIT_Y 6 * 10
#define SIT_THRESHOLD_MINOR 1
#define SIT_THRESHOLD_MAJOR 2

// sensor results
// light
uint32_t light_intensity_sum = 0;
uint32_t light_intensity_count = 0;
// button
uint32_t button_pressed_duration = 0;

enum severity
{
	SEVERITY_OK,
	SEVERITY_MINOR,
	SEVERITY_MAJOR
};

enum severity light_severity = SEVERITY_OK;
enum severity sit_severity = SEVERITY_OK;

enum message_type
{
	MESSAGE_TYPE_NONE,
	MESSAGE_TYPE_LIGHT,
	MESSAGE_TYPE_SIT
};

enum message_type current_message = MESSAGE_TYPE_NONE;

bool on = false;
void update_actuator(void);
void update_actuator()
{
	// sensor handling
	// handle sitting duration
	if (!ioport_get_pin_level(GPIO_PUSH_BUTTON_1))
	{
		button_pressed_duration++;
	}
	else
	{
		button_pressed_duration = 0;
	}

	// display light intensity
	uint32_t light_intensity = light_intensity_sum / light_intensity_count;
	snprintf(strbuf, sizeof(strbuf), "%5lu", light_intensity);
	gfx_mono_draw_string(strbuf, LIGHT_Y, 8, &sysfont);
	// display sitting duration
	// uint32_t sitting_duration = floor(button_pressed_duration / 3600);
	uint32_t sitting_duration = button_pressed_duration;
	snprintf(strbuf, sizeof(strbuf), "%2lu", sitting_duration);
	gfx_mono_draw_string(strbuf, SIT_Y, 8, &sysfont);
	// display face distance from monitor

	// determine severity
	// light severity
	enum severity prev_light_severity = light_severity;
	if (light_intensity < LIGHT_THRESHOLD_MINOR)
	{
		if (light_intensity > LIGHT_THRESHOLD_MAJOR)
		{
			light_severity = SEVERITY_MINOR;
		}
		else
		{
			light_severity = SEVERITY_MAJOR;
		}
	}
	else
	{
		light_severity = SEVERITY_OK;
	}
	// sitting duration
	enum severity prev_sit_severity = sit_severity;
	if (sitting_duration >= SIT_THRESHOLD_MINOR)
	{
		if (sitting_duration < SIT_THRESHOLD_MAJOR)
		{
			sit_severity = SEVERITY_MINOR;
		}
		else
		{
			sit_severity = SEVERITY_MAJOR;
		}
	}
	else
	{
		sit_severity = SEVERITY_OK;
	}

	// turn on led on warning
	// light
	if (light_severity != prev_light_severity)
	{
		if (light_severity > SEVERITY_OK)
		{
			ioport_set_pin_level(LED0_GPIO, IOPORT_PIN_LEVEL_LOW);
		}
		else
		{
			ioport_set_pin_level(LED0_GPIO, IOPORT_PIN_LEVEL_HIGH);
		}
	}
	// sitting
	if (sit_severity != prev_sit_severity)
	{
		if (sit_severity > SEVERITY_OK)
		{
			ioport_set_pin_level(LED1_GPIO, IOPORT_PIN_LEVEL_LOW);
		}
		else
		{
			ioport_set_pin_level(LED1_GPIO, IOPORT_PIN_LEVEL_HIGH);
		}
	}

	// buzzer handling
	if ((light_severity != prev_light_severity) || (sit_severity != prev_sit_severity))
	{
		if ((light_severity == SEVERITY_MAJOR) || (sit_severity == SEVERITY_MAJOR))
		{
			ioport_set_pin_level(J1_PIN0, IOPORT_PIN_LEVEL_HIGH);
		}
		else
		{
			ioport_set_pin_level(J1_PIN0, IOPORT_PIN_LEVEL_LOW);
		}
	}

	// message handling
	// decide what to display
	enum message_type prev_message = current_message;
	if (light_severity > SEVERITY_OK)
	{
		current_message = MESSAGE_TYPE_LIGHT;
	}
	else if (sit_severity > SEVERITY_OK)
	{
		current_message = MESSAGE_TYPE_SIT;
	}
	else
	{
		current_message = MESSAGE_TYPE_NONE;
	}
	// check whether to rewrite
	if (current_message != prev_message)
	{
		if (current_message == MESSAGE_TYPE_LIGHT)
		{
			snprintf(strbuf, sizeof(strbuf), "%-21s", "Ambient is too dark");
			gfx_mono_draw_string(strbuf, 0, 16, &sysfont);
			snprintf(strbuf, sizeof(strbuf), "%-21s", "Turn on some light");
			gfx_mono_draw_string(strbuf, 0, 24, &sysfont);
		}
		else if (current_message == MESSAGE_TYPE_SIT)
		{
			snprintf(strbuf, sizeof(strbuf), "%-21s", "Sat for too long");
			gfx_mono_draw_string(strbuf, 0, 16, &sysfont);
			snprintf(strbuf, sizeof(strbuf), "%-21s", "Please stand up");
			gfx_mono_draw_string(strbuf, 0, 24, &sysfont);
		}
		else
		{
			snprintf(strbuf, sizeof(strbuf), "%-21s", "");
			gfx_mono_draw_string(strbuf, 0, 16, &sysfont);
			snprintf(strbuf, sizeof(strbuf), "%-21s", "");
			gfx_mono_draw_string(strbuf, 0, 24, &sysfont);
		}
	}

	// reset light intensity
	light_intensity_sum = 0;
	light_intensity_count = 0;
}

void setup_timer(void);
void setup_timer()
{
	tc_enable(&TCC0);
	tc_set_overflow_interrupt_callback(&TCC0, update_actuator);
	tc_set_wgm(&TCC0, TC_WG_NORMAL);
	tc_write_period(&TCC0, 2000);
	tc_set_overflow_interrupt_level(&TCC0, TC_INT_LVL_LO);
	tc_write_clock_source(&TCC0, TC_CLKSEL_DIV1024_gc);
}

int main(void)
{
	/* Insert system clock initialization code here (sysclk_init()). */

	// inits
	board_init();
	sysclk_init();
	pmic_init();
	gfx_mono_init();

	// Wait for RTC32 sysclk to become stable
	sysclk_enable_module(SYSCLK_PORT_GEN, SYSCLK_RTC);
	while (RTC32.SYNCCTRL & RTC32_SYNCBUSY_bm)
	{
	}
	delay_ms(1000);

	// setup timer
	setup_timer();
	cpu_irq_enable();

	// setup adc
	adc_sensors_init();

	// setup ioport
	// turn on lcd
	ioport_set_pin_level(LCD_BACKLIGHT_ENABLE_PIN, IOPORT_PIN_LEVEL_HIGH);
	// set j1.0 as output
	ioport_set_pin_dir(J1_PIN0, IOPORT_DIR_OUTPUT);

	// print name and skeleton
	gfx_mono_draw_string("Coding Companion", 0, 0, &sysfont);
	gfx_mono_draw_string("L    0lx S 0h D   0cm", 0, 8, &sysfont);

	//forever loop
	while (1)
	{
		// ROOM LIGHTNING TOO DIM
		// get light intensity and wait until it is ready
		lightsensor_measure();
		while (!lightsensor_data_is_ready())
		{
		}
		light_intensity_sum += lightsensor_get_raw_value();
		light_intensity_count++;
	}
}
