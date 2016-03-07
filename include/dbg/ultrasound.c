#include "empr.h"
#include "dbg.h"
#include "ultrasound.h"

void ultrasound_initialise_timer_measurement(void)  {}
void ultrasound_send_test_pulse(void) {}
void TIMER2_IRQHandler(void) {}
void ultrasound_set_near_point(){}
void ultrasound_set_far_point(){}
void ultrasound_calibrate(void){}
uint32_t ultrasound_get_sample_median(uint32_t * samples){}
ultrasound_compare_values(const void * elem1, const void * elem2){}
uint32_t ultrasound_process_value(int calibration_gradient, int calibration_offset, int input_value)  {}

uint32_t ultrasound_get_distance(void){
    return dequeue(ULTRASOUND_QUEUE, uint32_t);
}
