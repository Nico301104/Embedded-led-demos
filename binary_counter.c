/*
 * binary_counter.c
 *
 * 10-bit binary counter displayed on a LED strip (LED1 = LSB).
 * Button toggles counting on/off. Counter increments every second
 * and wraps at 1023.
 */

#include "System/Os/io_os.h"
#include "MKL26Z4.h"
#include "Platform_Types/std_types.h"
#include "SB_Main.h"
#include "MCU_drivers/Dio/io_dio.h"

/* ------------------------------------------------------------------ */

static uint16 led_pins[10];
static uint16 counter            = 0;
static uint8  counting_active    = 0;
static uint8  last_button_state  = LOW;

/* ------------------------------------------------------------------ */

static void init_led_array(void)
{
    led_pins[0] = LED1;  led_pins[1] = LED2;
    led_pins[2] = LED3;  led_pins[3] = LED4;
    led_pins[4] = LED5;  led_pins[5] = LED6;
    led_pins[6] = LED7;  led_pins[7] = LED8;
    led_pins[8] = LED9;  led_pins[9] = LED10;
}

static void all_leds_off(void)
{
    for (int i = 0; i < 10; i++)
        Io_Dio_SetPinLevel(led_pins[i], LED_OFF);
}

static void display_binary(uint16 value)
{
    for (int i = 0; i < 10; i++)
    {
        Io_Dio_SetPinLevel(led_pins[i], (value & 1u) ? LED_ON : LED_OFF);
        value >>= 1;
    }
}

/* ------------------------------------------------------------------ */

void Main_Init(void)
{
    init_led_array();
    all_leds_off();
}

static void button_toggle(void)
{
    uint8_t btn = Io_Dio_GetPinLevel(BUTTON_PIN);

    if (btn == HIGH && last_button_state == LOW)
        counting_active ^= 1u;

    last_button_state = btn;
}

/* ------------------------------------------------------------------ */

void SB_RunningTask100ms(void)
{
    button_toggle();
}

void SB_RunningTask1000ms(void)
{
    if (!counting_active)
        return;

    if (++counter > 1023u)
        counter = 0;

    display_binary(counter);
}

void SB_RunningTask10ms(void) {}
