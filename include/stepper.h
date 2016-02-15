#define PIN_5 1<<5
#define PIN_4 1<<4
#define PIN_3 1<<3
#define PIN_2 1<<2

void stepper_init();
void stepper_send_nibble(int num);
