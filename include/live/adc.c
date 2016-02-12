#include "lpc17xx_adc.h"
#include "lpc_types.h"
#include "adc.h"
#include "pinsel.h"

#define ADC_CLOCKRATE 200000

void adc_enable(void) {
    /* enable the ADC, initilising with a clockrate of ADC_CLOCKRATE */

    /* enable all 6 adc0 pins on mbed */
    int i;
    for (i = 23; i < 27; ++i) {
        pinsel_enable_pin(PINSEL_PORT_0, i, PINSEL_FUNC_1);
    }

    pinsel_enable_pin(PINSEL_PORT_1, PINSEL_PIN_30, PINSEL_FUNC_2);
    pinsel_enable_pin(PINSEL_PORT_1, PINSEL_PIN_31, PINSEL_FUNC_2);

    ADC_Init(LPC_ADC, ADC_CLOCKRATE);
}

void adc_enable_channel(int channel) {
    /* enable a single channel on the ADC 
        TODO: Make this enable pin instead of `adc_enable` */

    ADC_ChannelCmd(LPC_ADC, channel, ENABLE);
}

int adc_get_channel_data(int channel) {
    /* Retrieve the 12 bit output from the ADC as an `int`
        given channel 0 .. 7 */

    ADC_StartCmd(LPC_ADC, ADC_START_NOW);
    while (!ADC_ChannelGetStatus(LPC_ADC, channel, ADC_DATA_DONE));

    int data = ADC_ChannelGetData(LPC_ADC, channel);
    return data;
}
