/*
 * led_bounce.c
 *
 * Short press  → one LED bounces left↔right across the strip.
 * Long press (≥3s) → two adjacent LEDs bounce together.
 * Step interval: 200ms.
 */

#include "System/Os/io_os.h"
#include "MKL26Z4.h"
#include "Platform_Types/std_types.h"
#include "SB_Main.h"
#include "MCU_drivers/Dio/io_dio.h"

#define LONG_PRESS_TICKS  30   /* 30 × 100ms = 3 s  */
#define STEP_TICKS         2   /* 2  × 100ms = 200ms */

/* ------------------------------------------------------------------ */

static uint16 led_pins[10];
static uint8  last_button_state  = LOW;
static uint16 press_ticks        = 0;

static uint8  mode               = 0;   /* 0=idle  1=single  2=double */
static uint8  direction          = 1;   /* 1=right  0=left */
static sint8  position           = 0;
static uint8  step_tick          = 0;

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

static void render(void)
{
    all_leds_off();
    if (position < 0 || position >= 10) return;

    Io_Dio_SetPinLevel(led_pins[position], LED_ON);
    if (mode == 2 && position + 1 < 10)
        Io_Dio_SetPinLevel(led_pins[position + 1], LED_ON);
}

/* ------------------------------------------------------------------ */

void Main_Init(void)
{
    init_led_array();
    all_leds_off();
}

static void button_logic(void)
{
    uint8_t btn = Io_Dio_GetPinLevel(BUTTON_PIN);

    if (btn == HIGH && last_button_state == LOW)
        press_ticks = 0;

    if (btn == HIGH)
        press_ticks++;

    if (btn == LOW && last_button_state == HIGH)
    {
        mode      = (press_ticks >= LONG_PRESS_TICKS) ? 2 : 1;
        position  = 0;
        direction = 1;
        step_tick = 0;
        render();
    }

    last_button_state = btn;
}

static void animation(void)
{
    if (mode == 0) return;

    if (++step_tick < STEP_TICKS) return;
    step_tick = 0;

    sint8 limit = (mode == 1) ? 9 : 8;

    if (direction == 1)
    {
        if (position < limit)  position++;
        else                   direction = 0;
    }
    else
    {
        if (position > 0)      position--;
        else                   direction = 1;
    }

    render();
}

/* ------------------------------------------------------------------ */

void SB_RunningTask100ms(void)
{
    button_logic();
    animation();
}

void SB_RunningTask1000ms(void) {}
void SB_RunningTask10ms(void)   {}
