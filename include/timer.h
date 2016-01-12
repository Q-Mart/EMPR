void timer_enable_systick(int);
void timer_enable_rit(int);

void timer_set_rit(int);
IntStatus timer_get_rit_status(void);

void timer_disable_systick(void);
void timer_disable_rit(void);

void timer_delay(int);

void timer_initialise_TIM(void);
void timer_configure_TIM_capture(int, int, int, int);
