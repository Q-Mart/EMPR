#include "dac.h"
#include "pinsel.h"

/* Initialise the DAC and enable that pin */
void dac_init(void)
{
    pinsel_enable_pin(PINSEL_PORT_0, PINSEL_PIN_26, PINSEL_FUNC_2);
    DAC_Init(LPC_DAC);
}

/* Update DAC output with a new value
 * between 0 - 2^10 */
void dac_update(int value)
{
    DAC_UpdateValue(LPC_DAC, value);
}
