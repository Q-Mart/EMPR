#include "timer.h"

void timer_enable_systick() {}
void timer_enable_rit(int dt) {}
void timer_set_rit(int dt) {}
void timer_disable_rit(void) {}
void timer_disable_systick(void) {}
void SysTick_Handler(void) {}

/* Get RIT Interrupt Status */
int timer_get_rit_status(void)
{
    return 0;
}

void timer_delay(int n) {}

/* Initialise general purpose timer 2 (TIM2). */
void timer_initialise_tim(void) {}

/* Config the capturing of TIM2, capture channel 1 (CAP2.1). */
void timer_configure_tim_capture(int channel, int rising, int falling, int interrupt) {}
