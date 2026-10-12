# ESP32 DC Motor Component (`ht_dc_motor`)

A modular and robust DC motor driver component for ESP32 using ESP-IDF v5. It leverages the MCPWM peripheral for precise, high-resolution hardware PWM generation, making it ideal for robotics (like AV robots), PID control loops, and general motor driving (e.g., L298N, TB6612FNG).

## Features
- **ESP-IDF v5 Ready:** Fully compatible with the updated `esp_driver_mcpwm` API.
- **Hardware PWM (MCPWM):** Zero CPU overhead for PWM generation.
- **High Resolution:** Uses float percentage (`0.0%` to `100.0%`) for smooth PID tuning instead of 8-bit limits.
- **Configurable:** Adjustable PWM frequency and MCPWM group IDs to prevent hardware resource conflicts.
- **Doxygen Documented:** Fully commented source code for easy maintenance.

## Hardware Connection (L298N Example)
| ESP32 Pin | Motor Driver Pin | Function |
| :--- | :--- | :--- |
| GPIO 26 | IN1 | Direction Control 1 |
| GPIO 27 | IN2 | Direction Control 2 |
| GPIO 14 | ENA / ENB | PWM Speed Control |

## Installation
If using as a component, simply drop the `ht_dc_motor` folder into your ESP-IDF project's `components/` directory. CMake will automatically discover it.

## Usage Example
```c
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ht_dc_motor.h"

void app_main(void) {
    // 1. Configure the motor hardware
    ht_dc_motor_t motor_right = {
        .group_id = 0,
        .in1_pin = 26,
        .in2_pin = 27,
        .en_pin = 14,
        .pwm_freq_hz = 20000 // 20kHz (Silent operation)
    };

    // 2. Initialize the MCPWM and GPIOs
    ht_dc_motor_init(&motor_right);

    // 3. Control the motor
    while (1) {
        // Move forward at 50.5% speed
        ht_dc_motor_set_speed(&motor_right, HT_DC_MOTOR_FWD, 50.5f);
        vTaskDelay(pdMS_TO_TICKS(2000));

        // Stop
        ht_dc_motor_stop(&motor_right);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}