# embedded-led-demos

A collection of bare-metal C projects targeting the **NXP MKL26Z4** microcontroller. Each file is self-contained and demonstrates a different peripheral or interaction pattern — button debouncing, ADC reading, PWM, and various LED animations.

---

## Projects

| File | Description |
|---|---|
| `rgb_color_cycle.c` | Cycles RGB LED through RED → GREEN → BLUE on each button press |
| `sequential_signal.c` | Audi-style sequential turn signal on a 10-LED strip |
| `binary_counter.c` | 10-bit binary counter on LED strip, toggled by button |
| `led_bar_button.c` | Fill/empty LED bar one step per button press |
| `flash_odd_even.c` | Short press flashes odd LEDs, long press (≥5s) flashes even LEDs |
| `led_bounce.c` | Single or double LED bouncing across strip (short / long press ≥3s) |
| `led_converge.c` | Two LEDs converge to center or diverge outward (short / long press ≥2s) |
| `led_bar_adc.c` | Potentiometer controls how many LEDs are lit |
| `motor_control.c` | Potentiometer sets motor speed, button toggles rotation direction |

---

## Hardware

- **MCU:** NXP MKL26Z4 (ARM Cortex-M0+)
- **Peripherals used:** GPIO, ADC, TPM/PWM, DIO
- **Tested on:** FRDM-KL26Z development board

---

## Structure

Each `.c` file depends on the project's internal driver layer (`MCU_drivers/`, `Platform_Types/`) and exposes three task functions called by the OS scheduler:

```c
void SB_RunningTask10ms(void);
void SB_RunningTask100ms(void);
void SB_RunningTask1000ms(void);
```

