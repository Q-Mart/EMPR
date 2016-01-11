// for LEDS:
// pin32 - LED1
// pin34 - LED2
// pin35 - LED3
// pin37 - LED4

#define LED_PORT 1
#define LED1 (1 << 18)
#define LED2 (1 << 20)
#define LED3 (1 << 21)
#define LED4 (1 << 23)

void delay1(void);
void init_gpio(int);
void set_gpio(int, int);
