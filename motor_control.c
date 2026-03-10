/*
 * motor_control.c
 *
 * Potentiometer controls DC motor speed via PWM (500Hz – 1kHz).
 * Button toggles motor rotation direction.
 */

#include "System/Os/io_os.h"
#include "MKL26Z4.h"
#include "Platform_Types/std_types.h"
#include "SB_Main.h"
#include "MCU_drivers/Adc/io_adc.h"
#include "MCU_drivers/Dio/io_dio.h"
#include "MCU_drivers/Tpm/io_tpm.h"

/* ------------------------------------------------------------------ */

static uint8_t last_button_state = LOW;
static uint8   adc_percent       = 0;
static uint16  pwm_period        = 2000;

/* ------------------------------------------------------------------ */

static uint8 read_percent(char channel)
{
    return (uint8)((100u * Io_Adc_GetResult(channel)) / MAX_VALUE_15BITS);
}

static void pwm_set_duty(uint32 channel, uint8 duty_pct)
{
    Io_Tpm_PwmChangeDutycycle(channel, (DUTYCYCLE_MAX_VALUE * duty_pct) / 100u);
}

/* ------------------------------------------------------------------ */

void Main_Init(void)
{
    pwm_set_duty(PWM_PIN, MOTOR_ON);
}

static void speed_update(void)
{
    /* PWM period: 2000µs (500Hz) → 1000µs (1kHz) based on pot position */
    uint16 period_us = 2000u - (adc_percent * 10u);
    pwm_period = IO_TPM_PERIOD_TO_TICKS(period_us, IO_TPM_PRESCALER16);
    Io_Tpm_PwmChangePeriod(PWM_PIN, pwm_period);
}

static void direction_check(void)
{
    uint8_t btn = Io_Dio_GetPinLevel(BUTTON_PIN);

    if (btn == HIGH && last_button_state == LOW)
        Io_Dio_SetPinLevel(PIN_MOTOR_DIR, !Io_Dio_GetPinLevel(PIN_MOTOR_DIR));

    last_button_state = btn;
}

/* ------------------------------------------------------------------ */

void SB_RunningTask100ms(void)
{
    adc_percent = read_percent(ADC_PIN);
    speed_update();
}

void SB_RunningTask10ms(void)  { direction_check(); }
void SB_RunningTask1000ms(void) {}
