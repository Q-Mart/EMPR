#include "ultrasound.h"

void initialise_pwm(void) 
{
    debug_init();
    pwm_init();
    pwm_set_match(0,95624);
    pwm_enable();
    pwm_config_match(PWM_CHANNEL_2, PWM_ENABLE_NONE);
    pwm_set_match(PWM_CHANNEL_2, 16);
    debug_send("PWM SET!\r\n");
    GPIO_SetDir(HCSR_GPIO_PORT, HCSR_GPIO_PIN, 0);

}

