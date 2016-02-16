#define PIN_5 1<<5
#define PIN_4 1<<4
#define PIN_3 1<<3
#define PIN_2 1<<2

#define STEP_1 0b1001
#define STEP_2 0b0101
#define STEP_3 0b0110
#define STEP_4 0b1010

#define NUMBER_OF_NIBBLE_STEPS 4

#define NUMBER_OF_STEPS 50

#define MIN_DELAY 3

void stepper_init();
void stepper_send_nibble(int num);
void stepper_step_clockwise();
void stepper_step_anticlockwise();
void stepper_full_clockwise(int delay);
void stepper_full_anticlockwise(int delay);
