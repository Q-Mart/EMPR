#include "lpc17xx_systick.h"
#include "lpc17xx_rit.h"
#include "lpc17xx_timer.h"
#include "timer.h"

/* Timer values */
static int SysTick_dt = 0;
static int RIT_dt = 0;
static int wait_for_timer = 0;

/* Enable the SysTick timer with some 
 * period `dt` */
void timer_enable_systick(int dt)
{
    SysTick_dt = dt;
    SYSTICK_Cmd(ENABLE);
    SYSTICK_IntCmd(ENABLE);
    SYSTICK_InternalInit(SysTick_dt);
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
    if (wait_for_timer) 
    {
        wait_for_timer -= SysTick_dt;
        if (wait_for_timer < 0) 
            wait_for_timer = 0;
    }
}

/* Get RIT Interrupt Status */
IntStatus timer_get_rit_status(void)
{
    return RIT_GetIntStatus(LPC_RIT);
}

/* wait for `n` ms before returning control */
void timer_delay(int n) 
{
    timer_enable_systick(n);
    wait_for_timer = n;
    while (!wait_for_timer);
}

/* Initialise general purpose timer (2). */
void timer_initialise_TIM(void)
{
    TIM_TIMERCFG_Type TIM_ConfigStruct;
    TIM_ConfigStruct.PrescaleOption = TIM_PRESCALE_USVAL;
    TIM_ConfigStruct.PrescaleValue  = 1000;
    TIM_Init(LPC_TIM2, TIM_TIMER_MODE, &TIM_ConfigStruct);

    //Now call timer_configure_TIM_capture to configure and enable timer 2.
}

/* Config the capturing of timer (2). */
void timer_configure_TIM_capture(int channel, int rising, int falling, int interrupt)
{
    TIM_CAPTURECFG_Type TIM_CaptureConfigStruct;
    TIM_CaptureConfigStruct.CaptureChannel = channel;
    TIM_CaptureConfigStruct.RisingEdge = rising;
    TIM_CaptureConfigStruct.FallingEdge = falling;
    TIM_CaptureConfigStruct.IntOnCaption = interrupt;

    TIM_ConfigCapture(LPC_TIM2, &TIM_CaptureConfigStruct);
    TIM_ResetCounter(LPC_TIM2);

    if (interrupt == 1) {
        NVIC_EnableIRQ(TIMER2_IRQn);
    }

    TIM_Cmd(LPC_TIM2, ENABLE);
}