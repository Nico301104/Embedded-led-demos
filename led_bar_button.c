/*
 * led_bar_button.c
 *
 * Each button press lights up the next LED in the strip (left→right).
 * Once all 10 are on, further presses turn them off one by one (right→left).
 */

#include "System/Os/io_os.h"
#include "MKL26Z4.h"
#include "Platform_Types/std_types.h"
#include "SB_Main.h"
#include "MCU_drivers/Dio/io_dio.h"

/* ------------------------------------------------------------------ */

static uint16 led_pins[10];
static uint8  led_index          = 0;
static uint8  direction          = 1;   /* 1=filling  0=emptying */
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

/* ------------------------------------------------------------------ */

void Main_Init(void)
{
    init_led_array();
    all_leds_off();
}

static void button_step(void)
{
    uint8_t btn = Io_Dio_GetPinLevel(BUTTON_PIN);

    if (btn == HIGH && last_button_state == LOW)
    {
        if (direction == 1)
        {
            if (led_index < 10)
                Io_Dio_SetPinLevel(led_pins[led_index++], LED_ON);
            if (led_index == 10)
                direction = 0;
        }
        else
        {
            if (led_index > 0)
                Io_Dio_SetPinLevel(led_pins[--led_index], LED_OFF);
            if (led_index == 0)
                direction = 1;
        }
    }

    last_button_state = btn;
}

/* ------------------------------------------------------------------ */

void SB_RunningTask100ms(void) { button_step(); }
void SB_RunningTask1000ms(void) {}
void SB_RunningTask10ms(void)   {}
