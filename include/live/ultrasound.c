#include "lpc17xx_gpio.h"
#include "lpc17xx_timer.h"

#include "ultrasound.h"
#include "network.h"
#include <stdio.h>

/* Ultrasound calibration variables. */
static uint32_t ultrasound_calibration_m;
static uint32_t ultrasound_calibration_c;
static uint32_t ultrasound_near_point;
static uint32_t ultrasound_far_point;

/* Timer global variables, do not read from them. */
static uint32_t ultrasound_current_timer_diff = 0;
static uint32_t ultrasound_previous_timer_value = 0;
static int ultrasound_false_edge_expected = 0;
static int ultrasound_pulse_count = 0;

/* Read this for the raw response time for the ultrasonic pulse.
 Value is in the unit of 10 us. 
 Includes both time taken to travel to object and back! */
uint32_t ultrasound_valid_response_time = 0; 

/* Call process_ultrasound_value with calibrated gradient and
  raw response time value to get a processed value in centimeters */

/* Initialise CAP2.1 and the pulse sending pin for ultrasonic measurements */
void ultrasound_initialise_timer_measurement(void) 
{
    pinsel_enable_pin(HCSR_TIMER_PORT, HCSR_TIMER_PIN, HCSR_TIMER_FUNC);
    timer_initialise_tim();
    timer_configure_tim_capture(1, 1, 1, 1);
    timer_enable_systick();
    init_general_gpio(HCSR_SIGNAL_PORT, HCSR_SIGNAL_PIN, GPIO_OUTPUT);
    set_general_gpio(HCSR_SIGNAL_PORT, HCSR_SIGNAL_PIN, 0);

    //Initialise ultrasound calibration values as well.
    ultrasound_calibration_m = 0;
    ultrasound_calibration_c = 0;
}

/* Ultrasound calibration functions. Except for sending test pulse,
    is nearly identical to the ultrasound calibration. */

void ultrasound_set_near_point(){
    ultrasound_send_test_pulse();
    timer_delay(50);
    ultrasound_near_point = 10 * ultrasound_valid_response_time / 2;
}

void ultrasound_set_far_point(){
    ultrasound_send_test_pulse();
    timer_delay(50);
    ultrasound_far_point = 10 * ultrasound_valid_response_time / 2;
    ultrasound_calibrate();
}

void ultrasound_calibrate(){
    ultrasound_calibration_m = (300000.0f - 150000.0f) / (ultrasound_far_point - ultrasound_near_point);
    ultrasound_calibration_c = 150000.0f - (ultrasound_calibration_m * ultrasound_near_point);
}

/* Send a pulse to trigger the sensor to measure, on Pin 8 */
void ultrasound_send_test_pulse(void){
    debug_sendfc("Send pulse... %d\r\n", ultrasound_pulse_count++);
    ultrasound_false_edge_expected = 1;
    set_general_gpio(HCSR_SIGNAL_PORT, HCSR_SIGNAL_PIN, 1);
    timer_delay(1);
    set_general_gpio(HCSR_SIGNAL_PORT, HCSR_SIGNAL_PIN, 0);
}

/*Using General Purpose Timer 2.1 to capture
rising and falling edges on ultrasound output. */
void TIMER2_IRQHandler(void)
{
    static uint32_t timer_value;
    static char debug_string[80];

    TIM_GetIntCaptureStatus(LPC_TIM2, TIM_CR1_INT);
    TIM_ClearIntCapturePending(LPC_TIM2, TIM_CR1_INT);
    timer_value = TIM_GetCaptureValue(LPC_TIM2, TIM_COUNTER_INCAP1);
    
    //overflow handling
    if (timer_value < ultrasound_previous_timer_value) {
        debug_send("Error: timer value inconsistent. Resetting timer.\r\n");
        ultrasound_current_timer_diff = 0;
        ultrasound_previous_timer_value = 0;
    } else {
        ultrasound_current_timer_diff = timer_value - ultrasound_previous_timer_value;
        ultrasound_previous_timer_value = timer_value;
    }
    
    /*Two transition sets will be detected by the input channel,
     one of them is the width of the output pulse, and the other
     is the duration between two output pulses. The duration always
     come first, so it is safe to ignore and only record the
     valid results.*/
    if (ultrasound_false_edge_expected == 1) {
        ultrasound_false_edge_expected = 0;
    } else {
        ultrasound_valid_response_time = ultrasound_current_timer_diff;
        sprintf(debug_string, "Timer Value: %lu \r\nTimer duration: %lu\r\n\r\n", (unsigned long)ultrasound_previous_timer_value, (unsigned long)ultrasound_valid_response_time);
        debug_send(debug_string);
    }
}
uint32_t ultrasound_get_distance(void){
	return ultrasound_process_value(ultrasound_calibration_m, ultrasound_calibration_c, ultrasound_valid_response_time);
}

/* Return a uint32_t with value of distance in micrometers of the object */
uint32_t ultrasound_process_value(int calibration_gradient, int calibration_offset, int input_value)
{
    uint32_t ultrasound_prescale = 10; 
    //Value of TIM_ConfigStruct.PrescaleValue in timer.c

    //Account for both inbound and outbound trip.
    uint32_t travel_time = (int)input_value / 2;
    uint32_t microseconds = travel_time * ultrasound_prescale;

    /*calibration_gradient would be the calculated speed of sound that 
     travelled betweenthe calibration object and the sensor, likely 
     different from 340m/s. Unit is microsecond/s with offset. */
    uint32_t distance = (microseconds * calibration_gradient + calibration_offset);

#ifdef RECORD
	network_send(ULTRASOUND_HEADER, distance, sizeof(uint32_t), NULL);
#endif

    return distance;
}
