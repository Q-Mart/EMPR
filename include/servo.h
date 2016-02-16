#include "empr.h"
#include "pwm.h"

#define FREQUENCY 500000
#define MIN_WIDTH 17500
#define MAX_WIDTH 57500
#define STEP 148

void servo_init();
uint16_t servo_set_pos(int angle);
uint16_t servo_get_pos();
