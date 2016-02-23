#include "empr.h"

#include "servo.h"
#include "network.h"

static uint16_t current_pos = 0;

void servo_init(){
    pwm_init();
    pwm_set_match(0, FREQUENCY);
    pwm_config_match(2, PWM_ENABLE_NONE);
    pwm_enable();
}

uint16_t servo_set_pos(int angle){
    if (angle >= 270){
        current_pos = 270;
        pwm_set_match(2, MAX_WIDTH);
    } else if (angle <= 0){
        current_pos = 0;
        pwm_set_match(2, MIN_WIDTH);
    } else{
        current_pos = angle;
        pwm_set_match(2, MIN_WIDTH + (angle * STEP));
    }

    return current_pos;
}

uint16_t servo_get_pos(){
    record(SERVO_HEADER, &current_pos, sizeof(uint16_t), NULL);
    return current_pos;
}
