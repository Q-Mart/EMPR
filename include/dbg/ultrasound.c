#include "empr.h"
#include "ultrasound.h"

void ultrasound_initialise_timer_measurement(void) {}

void ultrasound_send_test_pulse(void) {}
void TIMER2_IRQHandler(void) {}
void ultrasound_set_near_point(uint32_t x){}
void ultrasound_set_far_point(uint32_t x){}
void ultrasound_calibrate(void){}
uint32_t ultrasound_valid_response_time = 3;

uint32_t ultrasound_process_value(int calibration_gradient, int calibration_offset, int input_value)
{
    return 0;
}
