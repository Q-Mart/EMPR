#include "lpc17xx_gpio.h"
#include "ultrasound.h"
#include "pinsel.h"

void initialise_timer_measurement(void) 
{
    pinsel_enable_pin(HCSR_TIMER_PORT, HCSR_TIMER_PIN, HCSR_TIMER_FUNC);
    timer_initialise_TIM();
    timer_configure_TIM_capture(0, 1, 1, 1);
}

void initialise_trigger_timer(void)
{
    GPIO_SetDir(HCSR_SIGNAL_PORT, HCSR_SIGNAL_PIN, 1);
    timer_enable_rit(60);
}
