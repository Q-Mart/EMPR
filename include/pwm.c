#include "pwm.h"
#include "pinsel.h"

void pwm_init() {
    /* Initialise PWM device and set PWM Match Register 0 to default values */

    uint32_t PWM_Tick_Value = 1;

    PWM_TIMERCFG_Type PWMCfgDat;

    PWMCfgDat.PrescaleOption = PWM_TIMER_PRESCALE_TICKVAL; //in absolute value
    PWMCfgDat.PrescaleValue = PWM_Tick_Value;

    PWM_Init(LPC_PWM1, PWM_MODE_TIMER, (void *) &PWMCfgDat);


    /* init match register 0
        used for base timing */

    pinsel_enable_pin(PINSEL_PORT_2, PINSEL_PIN_0, PINSEL_FUNC_1);
    PWM_MATCHCFG_Type PWMMatchCfgDat;
    PWMMatchCfgDat.MatchChannel = PWM_CHANNEL_0;
    PWMMatchCfgDat.IntOnMatch = DISABLE;
    PWMMatchCfgDat.StopOnMatch = DISABLE;
    PWMMatchCfgDat.ResetOnMatch = ENABLE;
    PWM_ConfigMatch(LPC_PWM1, &PWMMatchCfgDat);
}

void pwm_set_match(int channel, int value){
    /* Set PWM Match Register channel to value */

    PWM_MatchUpdate(LPC_PWM1, channel, value, PWM_MATCH_UPDATE_NOW);
}

void pwm_config_match(int channel, int options) {
    /*Configure the match behaviours of PWM match register channel with options */

    pinsel_enable_pin(PINSEL_PORT_2, channel - 1, PINSEL_FUNC_1);

    PWM_MATCHCFG_Type PWMMatchCfgDat;
    PWMMatchCfgDat.MatchChannel = channel;
    PWM_ChannelConfig(LPC_PWM1, channel, PWM_CHANNEL_SINGLE_EDGE); //channel 1 always in single edge

    if ((options & PWM_ENABLE_INT) == PWM_ENABLE_INT)
        PWMMatchCfgDat.IntOnMatch = ENABLE;
    else
        PWMMatchCfgDat.IntOnMatch = DISABLE;

    if ((options & PWM_ENABLE_STOP) == PWM_ENABLE_STOP)
        PWMMatchCfgDat.StopOnMatch = ENABLE;
    else
        PWMMatchCfgDat.StopOnMatch = DISABLE;

    if ((options & PWM_ENABLE_RESET) == PWM_ENABLE_RESET)
        PWMMatchCfgDat.ResetOnMatch = ENABLE;
    else
        PWMMatchCfgDat.ResetOnMatch = DISABLE;

    PWM_ConfigMatch(LPC_PWM1, &PWMMatchCfgDat);
    PWM_ChannelCmd(LPC_PWM1, channel, ENABLE);
}

void pwm_enable(void) {
    /* Enable PWM output */
    
    PWM_ResetCounter(LPC_PWM1);
    PWM_CounterCmd(LPC_PWM1, ENABLE);

    PWM_Cmd(LPC_PWM1, ENABLE);
}