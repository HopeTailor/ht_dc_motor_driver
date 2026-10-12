#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ht_dc_motor.h"

void app_main(void) {
    ht_dc_motor_t motor_right = {
        .group_id = 0,
        .in1_pin = 26,
        .in2_pin = 27,
        .en_pin = 14,
        .pwm_freq_hz = 20000
    };

    ht_dc_motor_init(&motor_right);

    while (1) {
        ht_dc_motor_set_speed(&motor_right, HT_DC_MOTOR_FWD, 50.0f);
        vTaskDelay(pdMS_TO_TICKS(2000));

        ht_dc_motor_stop(&motor_right);
        vTaskDelay(pdMS_TO_TICKS(2000));

        ht_dc_motor_set_speed(&motor_right, HT_DC_MOTOR_REV, 50.0f);
        vTaskDelay(pdMS_TO_TICKS(2000));

        ht_dc_motor_stop(&motor_right);
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}