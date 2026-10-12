#pragma once

#include <stdint.h>
#include "esp_err.h"
#include "driver/mcpwm_prelude.h"
#include "driver/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    HT_DC_MOTOR_STOP = 0,
    HT_DC_MOTOR_FWD,
    HT_DC_MOTOR_REV
} ht_dc_motor_dir_t;

typedef struct {
    int in1_pin;
    int in2_pin;
    int en_pin;
    uint32_t pwm_freq_hz;

    mcpwm_timer_handle_t timer;
    mcpwm_oper_handle_t oper;
    mcpwm_cmpr_handle_t cmpr;
    mcpwm_gen_handle_t gen;
} ht_dc_motor_t;

esp_err_t ht_dc_motor_init(ht_dc_motor_t *motor);

esp_err_t ht_dc_motor_set_speed(ht_dc_motor_t *motor, ht_dc_motor_dir_t dir, float speed_percent);

esp_err_t ht_dc_motor_stop(ht_dc_motor_t *motor);

#ifdef __cplusplus
}
#endif