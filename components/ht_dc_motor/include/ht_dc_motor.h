/**
 * @file ht_dc_motor.h
 * @brief DC Motor Control Component for ESP32 using MCPWM.
 *
 * This component provides an interface to control DC motors using an L298N
 * (or similar) motor driver. It utilizes the ESP32's MCPWM peripheral for 
 * precise hardware PWM generation and standard GPIOs for direction control.
 */

#pragma once

#include <stdint.h>
#include "esp_err.h"
#include "driver/mcpwm_prelude.h"
#include "driver/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enumeration for DC motor direction states.
 */
typedef enum {
    HT_DC_MOTOR_STOP = 0, /*!< Motor stopped (coasting or braking depending on driver logic) */
    HT_DC_MOTOR_FWD,      /*!< Motor moving in forward direction */
    HT_DC_MOTOR_REV       /*!< Motor moving in reverse direction */
} ht_dc_motor_dir_t;

/**
 * @brief Configuration and state structure for a DC motor.
 *
 * This structure holds both the user-defined configuration (pins, frequency)
 * and the internal handles required by the MCPWM driver.
 */
typedef struct {
    /* User Configuration */
    int group_id;           /*!< MCPWM group ID (0 or 1). Useful for separating different motor groups. */
    int in1_pin;            /*!< GPIO pin connected to driver's IN1 (Direction pin 1) */
    int in2_pin;            /*!< GPIO pin connected to driver's IN2 (Direction pin 2) */
    int en_pin;             /*!< GPIO pin connected to driver's EN (PWM speed control) */
    uint32_t pwm_freq_hz;   /*!< Desired PWM frequency in Hz (e.g., 20000 for 20kHz) */

    /* Internal MCPWM Handles (managed by the init function) */
    mcpwm_timer_handle_t timer; /*!< Handle to the MCPWM timer */
    mcpwm_oper_handle_t oper;   /*!< Handle to the MCPWM operator */
    mcpwm_cmpr_handle_t cmpr;   /*!< Handle to the MCPWM comparator (controls duty cycle) */
    mcpwm_gen_handle_t gen;     /*!< Handle to the MCPWM generator (outputs the signal to the pin) */
} ht_dc_motor_t;

/**
 * @brief Initialize the DC motor driver hardware.
 *
 * This function configures the specified GPIOs for direction control and 
 * sets up the MCPWM timer, operator, comparator, and generator based on 
 * the parameters provided in the ht_dc_motor_t structure.
 *
 * @param motor Pointer to the motor configuration structure.
 * @return 
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if the motor pointer is NULL
 */
esp_err_t ht_dc_motor_init(ht_dc_motor_t *motor);

/**
 * @brief Set the speed and direction of the DC motor.
 *
 * This function updates the MCPWM comparator to set the desired duty cycle 
 * and changes the state of the IN1/IN2 pins to set the direction.
 *
 * @param motor Pointer to the initialized motor configuration structure.
 * @param dir Desired direction (HT_DC_MOTOR_FWD, HT_DC_MOTOR_REV, or HT_DC_MOTOR_STOP).
 * @param speed_percent Desired speed as a percentage (0.0 to 100.0).
 * @return 
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if motor is NULL or speed is out of bounds [0.0, 100.0]
 */
esp_err_t ht_dc_motor_set_speed(ht_dc_motor_t *motor, ht_dc_motor_dir_t dir, float speed_percent);

/**
 * @brief Stop the DC motor immediately.
 *
 * This function sets the PWM duty cycle to 0 and pulls both direction pins LOW, 
 * effectively stopping the motor.
 *
 * @param motor Pointer to the initialized motor configuration structure.
 * @return 
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if the motor pointer is NULL
 */
esp_err_t ht_dc_motor_stop(ht_dc_motor_t *motor);

#ifdef __cplusplus
}
#endif