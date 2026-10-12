#include "ht_dc_motor.h"


esp_err_t ht_dc_motor_init(ht_dc_motor_t *motor) {
    if(!motor) {
        return ESP_ERR_INVALID_ARG;
    }

    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1ULL << motor->in1_pin) | (1ULL << motor->in2_pin),
        .pull_down_en = 0,
        .pull_up_en = 0,
    };
    gpio_config(&io_conf);
    gpio_set_level(motor->in1_pin, 0);
    gpio_set_level(motor->in2_pin, 0);

    mcpwm_timer_config_t timer_config = {
        .group_id = motor->group_id,
        .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT,
        .resolution_hz = 1000000,
        .count_mode = MCPWM_TIMER_COUNT_MODE_UP,
        .period_ticks = 1000000 / motor->pwm_freq_hz,
    };
    mcpwm_new_timer(&timer_config, &motor->timer);

    mcpwm_operator_config_t oper_config = {
        .group_id = motor->group_id,
    };
    mcpwm_new_operator(&oper_config, &motor->oper);
    mcpwm_operator_connect_timer(motor->oper, motor->timer);

    mcpwm_comparator_config_t cmpr_config = {
        .flags.update_cmp_on_tez = true,
    };
    mcpwm_new_comparator(motor->oper, &cmpr_config, &motor->cmpr);
    mcpwm_comparator_set_compare_value(motor->cmpr, 0);

    mcpwm_generator_config_t gen_config = {
        .gen_gpio_num = motor->en_pin,
    };
    mcpwm_new_generator(motor->oper, &gen_config, &motor->gen);

    mcpwm_generator_set_action_on_timer_event(motor->gen, MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH));
    mcpwm_generator_set_action_on_compare_event(motor->gen, MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, motor->cmpr, MCPWM_GEN_ACTION_LOW));

    mcpwm_timer_enable(motor->timer);
    mcpwm_timer_start_stop(motor->timer, MCPWM_TIMER_START_NO_STOP);

    return ESP_OK;
}
