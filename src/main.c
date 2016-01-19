#include "debug.h"
#include "servo.h"
#include "timer.h"

int main(void){
    /* debug_init(); */
    /* pwm_init(); */

    /* pwm_set_match(0,500000); */

    /* pwm_config_match(2,PWM_ENABLE_NONE); */
    /* int i; */

    /* pwm_set_match(2,18750); */
    /* pwm_enable(); */

    servo_init();
    servo_set_pos(270);

    timer_delay(500);
    servo_set_pos(160);

    timer_delay(500);
    servo_set_pos(0);
}
