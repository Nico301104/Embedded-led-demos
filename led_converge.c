/*
 * led_converge.c
 *
 * Short press  → two LEDs move from both ends toward the center.
 * Long press (≥2s) → two LEDs move from the center toward both ends.
 * Step interval: 200ms. Animation loops continuously once triggered.
 */

#include "System/Os/io_os.h"
#include "MKL26Z4.h"
#include "Platform_Types/std_types.h"
#include "SB_Main.h"
#include "MCU_drivers/Dio/io_dio.h"

#define STEP_MS            200
#define LONG_PRESS_MS     2000

/* ------------------------------------------------------------------ */

static uint16 led_pins[10];
static uint8  last_button_state   = LOW;
static uint16 press_ms            = 0;
static uint8  long_press_handled  = 0;

static uint8  mode                = 0;   /* 0=idle  1=inward  2=outward */
static uint8  position            = 0;   /* 0–4: step index */
static uint16 move_timer_ms       = 0;

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
    if (mode == 0) return;

    uint8 left, right;

    if (mode == 1)   /* extremities → center */
    {
        left  = position;
        right = 9 - position;
    }
    else             /* center → extremities */
    {
        left  = 4 - position;
        right = 5 + position;
    }

    Io_Dio_SetPinLevel(led_pins[left],  LED_ON);
    Io_Dio_SetPinLevel(led_pins[right], LED_ON);
}

static void start(uint8 new_mode)
{
    mode          = new_mode;
    position      = 0;
    move_timer_ms = 0;
    render();
}

/* ------------------------------------------------------------------ */

void Main_Init(void)
{
    init_led_array();
    all_leds_off();
}

static void button_logic(void)
{
    uint8 btn = Io_Dio_GetPinLevel(BUTTON_PIN);

    if (btn == HIGH)
    {
        if (last_button_state == LOW)
        {
            press_ms           = 0;
            long_press_handled = 0;
        }
        else
        {
            if (press_ms < 60000u)
                press_ms += 100u;

            if (press_ms >= LONG_PRESS_MS && !long_press_handled)
            {
                start(2);
                long_press_handled = 1;
            }
        }
    }
    else if (last_button_state == HIGH && !long_press_handled)
    {
        start(1);
    }

    last_button_state = btn;
}

static void animation(void)
{
    if (mode == 0) return;

    move_timer_ms += 100u;
    if (move_timer_ms < STEP_MS) return;

    move_timer_ms = 0;
    position      = (position + 1u) % 5u;
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
