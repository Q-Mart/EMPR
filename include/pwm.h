#define PWM_CHANNEL_0 0
#define PWM_CHANNEL_1 1
#define PWM_CHANNEL_2 2
#define PWM_CHANNEL_3 3
#define PWM_CHANNEL_4 4
#define PWM_CHANNEL_5 5
#define PWM_CHANNEL_6 6

#define PWM_ENABLE_NONE 0x0
#define PWM_ENABLE_INT 0x1
#define PWM_ENABLE_STOP 0x2
#define PWM_ENABLE_RESET 0x4

void pwm_init(void);
void pwm_set_match(int channel, int value);
void pwm_config_match(int reg, int options);
void pwm_enable(void);
