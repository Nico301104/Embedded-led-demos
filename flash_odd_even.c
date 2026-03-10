/*
 * flash_odd_even.c
 *
 * Short press  → flashes odd-indexed LEDs  (LED1, LED3, LED5 …)
 * Long press (≥5s) → flashes even-indexed LEDs (LED2, LED4, LED6 …)
 * Flash period: 500ms.
 */

#include "System/Os/io_os.h"
#include "MKL26Z4.h"
#include "Platform_Types/std_types.h"
#include "SB_Main.h"
#include "MCU_drivers/Dio/io_dio.h"

#define LONG_PRESS_TICKS  50   /* 50 × 100ms = 5 s */
#define FLASH_TICKS        5   /* 5 × 100ms  = 500ms */

/* ------------------------------------------------------------------ */

static uint16 led_pins[10];
static uint8  last_button_state  = LOW;
static uint16 press_ticks        = 0;

static uint8  flash_mode         = 0;   /* 0=off  1=odd  2=even */
static uint8  flash_tick         = 0;
static uint8  flash_state        = 0;

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

static void button_logic(void)
{
    uint8_t btn = Io_Dio_GetPinLevel(BUTTON_PIN);

    if (btn == HIGH)
        press_ticks++;
    else if (last_button_state == HIGH)
    {
        flash_mode  = (press_ticks >= LONG_PRESS_TICKS) ? 2 : 1;
        press_ticks = 0;
        flash_tick  = 0;
        flash_state = 0;
        all_leds_off();
    }

    last_button_state = btn;
}

static void flash_update(void)
{
    if (flash_mode == 0)
        return;

    if (++flash_tick < FLASH_TICKS)
        return;

    flash_tick  = 0;
    flash_state ^= 1u;

    for (int i = 0; i < 10; i++)
    {
        /* index even (0,2,4…) → physical LED is odd-numbered, and vice-versa */
        uint8_t target = (flash_mode == 1) ? (i % 2 == 0) : (i % 2 != 0);
        Io_Dio_SetPinLevel(led_pins[i], (target && flash_state) ? LED_ON : LED_OFF);
    }
}

/* ------------------------------------------------------------------ */

void SB_RunningTask100ms(void)
{
    button_logic();
    flash_update();
}

void SB_RunningTask1000ms(void) {}
void SB_RunningTask10ms(void)   {}
