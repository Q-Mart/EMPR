void timer_enable_systick();
void timer_enable_rit(int);

void timer_set_rit(int);
IntStatus timer_get_rit_status(void);

void timer_disable_systick(void);
void timer_disable_rit(void);

void timer_delay(int);

void timer_initialise_tim(void);
void timer_configure_tim_capture(int, int, int, int);
