#include "pwm.h"
#include "timer.h"
#include "lpc17xx_gpio.h"
#include "debug.h"

#define HCSR_GPIO_PORT 0
#define HCSR_GPIO_PIN (1 << 16)

void initialise_pwm(void);