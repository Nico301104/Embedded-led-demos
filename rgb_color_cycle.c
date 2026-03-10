/*
 * rgb_color_cycle.c
 *
 * Cycles through RED → GREEN → BLUE on each button press.
 * Motor speed is controlled via potentiometer (PWM frequency 500Hz–1kHz).
 */

#include "System/Os/io_os.h"
#include "MKL26Z4.h"
#include "Platform_Types/std_types.h"
#include "SB_Main.h"
#include "MCU_drivers/Adc/io_adc.h"
#include "MCU_drivers/Dio/io_dio.h"
#include "MCU_drivers/Tpm/io_tpm.h"

/* ------------------------------------------------------------------ */

static uint8_t current_color    = 0;   /* 0=RED  1=GREEN  2=BLUE */
static uint8_t last_button_state = LOW;
static uint8_t adc_percent       = 0;
static uint16  pwm_period        = 2000;

/* ------------------------------------------------------------------ */

static uint16 read_analog_percent(char channel)
{
    uint16 adc_value = Io_Adc_GetResult(channel);
    return (100u * adc_value) / MAX_VALUE_15BITS;
}

static void pwm_set_duty(uint32 channel, uint8 duty_pct)
{
    uint16 dutycycle = (DUTYCYCLE_MAX_VALUE * duty_pct) / 100u;
    Io_Tpm_PwmChangeDutycycle(channel, dutycycle);
}

/* ------------------------------------------------------------------ */

void Main_Init(void)
{
    pwm_set_duty(PWM_PIN, MOTOR_ON);
}

static void color_cycle(void)
{
    uint8_t btn = Io_Dio_GetPinLevel(BUTTON_PIN);

    if (btn == HIGH && last_button_state == LOW)
    {
        Io_Dio_SetPinLevel(SET_LED_RED,   LED_OFF);
        Io_Dio_SetPinLevel(SET_LED_GREEN, LED_OFF);
        Io_Dio_SetPinLevel(SET_LED_BLUE,  LED_OFF);

        const uint16 colors[] = { SET_LED_RED, SET_LED_GREEN, SET_LED_BLUE };
        Io_Dio_SetPinLevel(colors[current_color], LED_ON);

        current_color = (current_color + 1u) % 3u;
    }

    last_button_state = btn;
}

static void motor_speed_update(void)
{
    uint16 period_us = 2000u - (adc_percent * 10u);
    pwm_period = IO_TPM_PERIOD_TO_TICKS(period_us, IO_TPM_PRESCALER16);
    Io_Tpm_PwmChangePeriod(PWM_PIN, pwm_period);
}

/* ------------------------------------------------------------------ */

void SB_RunningTask100ms(void)
{
    adc_percent = read_analog_percent(ADC_PIN);
    color_cycle();
    motor_speed_update();
}

void SB_RunningTask1000ms(void) {}
void SB_RunningTask10ms(void)   {}
