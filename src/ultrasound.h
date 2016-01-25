#include "pwm.h"
#include "timer.h"
#include "lpc17xx_gpio.h"
#include "debug.h"

#define HCSR_TIMER_PORT 0
#define HCSR_TIMER_PIN 24
#define HCSR_TIMER_FUNC 3
#define HCSR_SIGNAL_PORT 0
#define HCSR_SIGNAL_PIN (1 << 6)

#define HCSR_GPIO_PORT 0

void initialise_timer_measurement(void);
