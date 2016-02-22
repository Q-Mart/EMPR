#include "empr.h"
#include "dbg.h"
#include "ultrasound.h"


void ultrasound_initialise_timer_measurement(void)  {}
void ultrasound_send_test_pulse(void) {}
void TIMER2_IRQHandler(void) {}
void ultrasound_set_near_point(){}
void ultrasound_set_far_point(){}
void ultrasound_calibrate(void){}
uint32_t ultrasound_valid_response_time = 3;

uint32_t ultrasound_process_value(int calibration_gradient, int calibration_offset, int input_value)
{
    return dequeue(ULTRASOUND_QUEUE, uint32_t);
}
uint32_t ultrasound_get_distance(void){
	return 0;
}
