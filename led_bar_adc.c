/*
 * led_bar_adc.c
 *
 * Potentiometer controls how many LEDs are lit on the strip.
 * 0% → all off, 100% → all 10 on. Updates every 100ms.
 */

#include "System/Os/io_os.h"
#include "MKL26Z4.h"
#include "Platform_Types/std_types.h"
#include "SB_Main.h"
#include "MCU_drivers/Adc/io_adc.h"
#include "MCU_drivers/Dio/io_dio.h"

/* ------------------------------------------------------------------ */

static uint16 led_pins[10];
static uint8  adc_percent = 0;

/* ------------------------------------------------------------------ */

static void init_led_array(void)
{
    led_pins[0] = LED1;  led_pins[1] = LED2;
    led_pins[2] = LED3;  led_pins[3] = LED4;
    led_pins[4] = LED5;  led_pins[5] = LED6;
    led_pins[6] = LED7;  led_pins[7] = LED8;
    led_pins[8] = LED9;  led_pins[9] = LED10;
}

static uint8 read_percent(char channel)
{
    return (uint8)((100u * Io_Adc_GetResult(channel)) / MAX_VALUE_15BITS);
}

static void update_bar(void)
{
    uint8 active = adc_percent / 10u;   /* 0–10 LEDs */

    for (int i = 0; i < 10; i++)
        Io_Dio_SetPinLevel(led_pins[i], (i < active) ? LED_ON : LED_OFF);
}

/* ------------------------------------------------------------------ */

void Main_Init(void)
{
    init_led_array();
    for (int i = 0; i < 10; i++)
        Io_Dio_SetPinLevel(led_pins[i], LED_OFF);
}

void SB_RunningTask100ms(void)
{
    adc_percent = read_percent(ADC_PIN);
    update_bar();
}

void SB_RunningTask1000ms(void) {}
void SB_RunningTask10ms(void)   { Io_Adc_Autoscan(); }
