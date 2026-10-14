#include "ht_dc_motor.h"


esp_err_t ht_dc_motor_init(ht_dc_motor_t *motor) {
    if(!motor) {
        return ESP_ERR_INVALID_ARG;
    }

    uint64_t pin_mask = (1ULL << motor->in1_pin);
    if(motor->in2_pin >= 0) {
        pin_mask |= (1ULL << motor->in2_pin);
    }

    /* 1. Configure Direction Pins (IN1 & IN2) as Outputs */
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_DISABLE, // No interrupts needed for direction control
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = pin_mask,
        .pull_down_en = 0, // Hardware driver handles voltage levels
        .pull_up_en = 0,
    };
    gpio_config(&io_conf);
    
    // Set initial safe state (motor stopped)
    gpio_set_level(motor->in1_pin, 0);
    if(motor->in2_pin >= 0) {
        gpio_set_level(motor->in2_pin, 0);
    }

    /* 2. Configure MCPWM Timer */
    mcpwm_timer_config_t timer_config = {
        .group_id = motor->group_id,
        .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT,
        .resolution_hz = 1000000, // 1 MHz tick resolution
        .count_mode = MCPWM_TIMER_COUNT_MODE_UP,
        .period_ticks = 1000000 / motor->pwm_freq_hz, // Auto-calculate period based on desired frequency
    };
    mcpwm_new_timer(&timer_config, &motor->timer);

    /* 3. Configure MCPWM Operator */
    mcpwm_operator_config_t oper_config = {
        .group_id = motor->group_id,
    };
    mcpwm_new_operator(&oper_config, &motor->oper);
    mcpwm_operator_connect_timer(motor->oper, motor->timer);

    /* 4. Configure MCPWM Comparator */
    mcpwm_comparator_config_t cmpr_config = {
        .flags.update_cmp_on_tez = true, // Update duty cycle smoothly when timer hits zero
    };
    mcpwm_new_comparator(motor->oper, &cmpr_config, &motor->cmpr);
    mcpwm_comparator_set_compare_value(motor->cmpr, 0); // Start with 0% duty cycle

    /* 5. Configure MCPWM Generator */
    mcpwm_generator_config_t gen_config = {
        .gen_gpio_num = motor->en_pin,
    };
    mcpwm_new_generator(motor->oper, &gen_config, &motor->gen);

    /* 6. Define Generator Actions (Create PWM Waveform) */
    // Set pin HIGH when timer hits zero
    mcpwm_generator_set_action_on_timer_event(motor->gen, MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH));
    // Set pin LOW when timer hits the comparator value
    mcpwm_generator_set_action_on_compare_event(motor->gen, MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, motor->cmpr, MCPWM_GEN_ACTION_LOW));

    /* 7. Start the Timer */
    mcpwm_timer_enable(motor->timer);
    mcpwm_timer_start_stop(motor->timer, MCPWM_TIMER_START_NO_STOP);

    return ESP_OK;
}

esp_err_t ht_dc_motor_set_speed(ht_dc_motor_t *motor, ht_dc_motor_dir_t dir, float speed_percent) {
    if(!motor || speed_percent < 0.0f || speed_percent > 100.0f) {
        return ESP_ERR_INVALID_ARG;
    }

    // Calculate comparator value based on percentage and frequency
    uint32_t compare_val = (uint32_t)((speed_percent / 100.0f) * (1000000 / motor->pwm_freq_hz));
    mcpwm_comparator_set_compare_value(motor->cmpr, compare_val);

    // Set direction pins
    if(dir == HT_DC_MOTOR_FWD) {
        gpio_set_level(motor->in1_pin, 1);
        if(motor->in2_pin >= 0) {
            gpio_set_level(motor->in2_pin, 0);
        }
    }
    else if(dir == HT_DC_MOTOR_REV) {
        gpio_set_level(motor->in1_pin, 0);
        if(motor->in2_pin >= 0) {
            gpio_set_level(motor->in2_pin, 1);
        }
    }
    else {
        // HT_DC_MOTOR_STOP falls here
        gpio_set_level(motor->in1_pin, 0);
        if(motor->in2_pin >= 0) {
            gpio_set_level(motor->in2_pin, 0);
        }
    }

    return ESP_OK;
}

esp_err_t ht_dc_motor_stop(ht_dc_motor_t *motor) {
    if(!motor) {
        return ESP_ERR_INVALID_ARG;
    }

    // Reset duty cycle to 0% and pull direction pins LOW
    mcpwm_comparator_set_compare_value(motor->cmpr, 0);
    gpio_set_level(motor->in1_pin, 0);
    if(motor->in2_pin >= 0) {
        gpio_set_level(motor->in2_pin, 0);
    }

    return ESP_OK;
}