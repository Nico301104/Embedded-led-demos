/*
 * sequential_signal.c
 *
 * Audi-style sequential turn signal animation on a 10-LED strip.
 * Button press triggers: LEDs light up one by one left→right,
 * then turn off right→left.
 */

#include "System/Os/io_os.h"
#include "MKL26Z4.h"
#include "Platform_Types/std_types.h"
#include "SB_Main.h"
#include "MCU_drivers/Dio/io_dio.h"

#define STEP_MS  100   /* delay between each LED step */

/* ------------------------------------------------------------------ */

static uint16 led_pins[10];
static uint8  led_index          = 0;
static uint8  last_button_state  = LOW;
static uint8  animation_active   = 0;
static uint8  direction          = 1;   /* 1=forward  0=reverse */
static uint16 step_timer         = 0;

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

/* ------------------------------------------------------------------ */

void Main_Init(void)
{
    init_led_array();
    all_leds_off();
}

static void button_check(void)
{
    uint8_t btn = Io_Dio_GetPinLevel(BUTTON_PIN);

    if (btn == HIGH && last_button_state == LOW && !animation_active)
    {
        animation_active = 1;
        led_index        = 0;
        direction        = 1;
        step_timer       = 0;
    }

    last_button_state = btn;
}

static void animation_step(void)
{
    if (!animation_active)
        return;

    step_timer += 100;
    if (step_timer < STEP_MS)
        return;

    step_timer = 0;

    if (direction == 1)
    {
        if (led_index < 10)
            Io_Dio_SetPinLevel(led_pins[led_index++], LED_ON);
        else
            direction = 0;
    }
    else
    {
        if (led_index > 0)
            Io_Dio_SetPinLevel(led_pins[--led_index], LED_OFF);
        else
        {
            direction        = 1;
            animation_active = 0;
        }
    }
}

/* ------------------------------------------------------------------ */

void SB_RunningTask100ms(void)
{
    button_check();
    animation_step();
}

void SB_RunningTask1000ms(void) {}
void SB_RunningTask10ms(void)   {}
