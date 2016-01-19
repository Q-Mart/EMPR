#include "lpc17xx_systick.h"
#include "lpc17xx_rit.h"
#include "timer.h"

/* Timer values */
static int SysTick_on = 0;
static int RIT_dt = 0;

/* Enable the SysTick timer with some 
 * period `dt` */
void timer_enable_systick()
{
    SysTick_Config(SystemCoreClock/1000);
}

/* Enable the RIT with some period `dt` */
void timer_enable_rit(int dt)
{
    RIT_dt = dt;
    RIT_Init(LPC_RIT);
    RIT_TimerConfig(LPC_RIT, dt);
    NVIC_EnableIRQ(RIT_IRQn);
}

/* Set the RIT's timer to `dt` */
void timer_set_rit(int dt)
{
    RIT_dt = dt;
    RIT_TimerConfig(LPC_RIT, dt);
}

/* Disable RIT interrupts */
void timer_disable_rit(void)
{
    NVIC_DisableIRQ(RIT_IRQn);
}

void timer_disable_systick(void)
{
    SYSTICK_IntCmd(DISABLE);
}

void SysTick_Handler(void) 
{
    SysTick_on++;
}

/* Get RIT Interrupt Status */
IntStatus timer_get_rit_status(void)
{
    return RIT_GetIntStatus(LPC_RIT);
}

/* wait for `n` ms before returning control */
void timer_delay(int n) 
{
    unsigned long SysTick_count;
    SysTick_count = SysTick_on;
    while((SysTick_on - SysTick_count) < n);
}
