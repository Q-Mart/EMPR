#include "lpc17xx_gpio.h"
#include "lpc17xx_timer.h"
#include "pinsel.h"
#include "timer.h"
#include "debug.h"
#include "gpio.h"

#define HCSR_TIMER_PORT 0
#define HCSR_TIMER_PIN 5
#define HCSR_TIMER_FUNC 3
#define HCSR_SIGNAL_PORT 0
#define HCSR_SIGNAL_PIN (1 << 6)

void ultrasound_initialise_timer_measurement(void);
void ultrasound_send_test_pulse(void);
uint32_t ultrasound_process_value(int, int, int);

extern uint32_t ultrasound_valid_response_time;
