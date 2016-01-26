#include "ultrasound.h"

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
void initialise_timer_measurement(void) 
{
    pinsel_enable_pin(HCSR_TIMER_PORT, HCSR_TIMER_PIN, HCSR_TIMER_FUNC);
    timer_initialise_TIM();
    timer_configure_TIM_capture(1, 1, 1, 1);
    timer_enable_systick();
    init_general_gpio(HCSR_SIGNAL_PORT, HCSR_SIGNAL_PIN, GPIO_OUTPUT);
    set_general_gpio(HCSR_SIGNAL_PORT, HCSR_SIGNAL_PIN, 0);
}

/* Send a pulse to trigger the sensor to measure, on Pin 8 */
void send_test_pulse(void){
    debug_sendfc("Send pulse... %d\r\n", ultrasound_pulse_count++);
    ultrasound_false_edge_expected = 1;
    set_general_gpio(HCSR_SIGNAL_PORT, HCSR_SIGNAL_PIN, 1);
    timer_delay(1);
    set_general_gpio(HCSR_SIGNAL_PORT, HCSR_SIGNAL_PIN, 0);
    timer_delay(1000);
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

/* Return a double with value of distance in centimeters of the object */
double process_ultrasound_value(int calibration_gradient, int input_value)
{
    double ultrasound_prescale = 10.0; 
    //Value of TIM_ConfigStruct.PrescaleValue in timer.c

    //Account for both inbound and outbound trip.
    double travel_time = (int)input_value / 2;
    double seconds = travel_time * ultrasound_prescale / 10000.0;

    /*calibration_gradient would be the calculated speed of sound that 
     travelled betweenthe calibration object and the sensor, likely 
     different from 340m/s. Unit is m/s. */
    double distance = seconds * (double)calibration_gradient;

    return distance;
}