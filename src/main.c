/**
 * @file main.c
 * @brief Main application entry point for the ESP32 robotic system.
 * 
 * This application initializes the hardware peripherals (like the DC motor driver)
 * and runs a basic test routine in a FreeRTOS task loop.
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ht_dc_motor.h"

void app_main(void) {
    /* 1. Hardware Configuration */
    // Define and configure the right motor parameters
    ht_dc_motor_t motor_right = {
        .group_id = 0,          // Use MCPWM group 0
        .in1_pin = 26,          // Direction pin 1
        .in2_pin = 27,          // Direction pin 2
        .en_pin = 14,           // PWM enable pin (speed control)
        .pwm_freq_hz = 20000    // 20kHz PWM frequency (outside human hearing range)
    };

    /* 2. Hardware Initialization */
    // Initialize the motor driver with the provided configuration
    ht_dc_motor_init(&motor_right);

    /* 3. Main Application Loop */
    // Infinite loop running as the main FreeRTOS task
    while (1) {
        // Move motor forward at 50% speed for 2 seconds
        ht_dc_motor_set_speed(&motor_right, HT_DC_MOTOR_FWD, 50.0f);
        vTaskDelay(pdMS_TO_TICKS(2000));

        // Stop the motor for 2 seconds
        ht_dc_motor_stop(&motor_right);
        vTaskDelay(pdMS_TO_TICKS(2000));

        // Move motor in reverse at 50% speed for 2 seconds
        ht_dc_motor_set_speed(&motor_right, HT_DC_MOTOR_REV, 50.0f);
        vTaskDelay(pdMS_TO_TICKS(2000));

        // Stop the motor for 2 seconds before repeating
        ht_dc_motor_stop(&motor_right);
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}