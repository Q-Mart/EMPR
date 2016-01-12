#include "pwm.h"
#include "timer.h"
#include "lpc17xx_gpio.h"
#include "debug.h"

#define HCSR_TIMER_PORT 0
#define HCSR_TIMER_PIN 4
#define HCSR_TIMER_FUNC 3
#define HCSR_SIGNAL_PORT 0
#define HCSR_SIGNAL_PIN (1 << 5)

#define HCSR_GPIO_PORT 0

void initialise_timer_measurement(void);