#include "keypad.h"
#include "lcd.h"
#include "lpc17xx_gpio.h"
#include "test.h"

static States state = 0;

enum States {
    CALIBRATE,
    SCAN,
    MEASURE,
    MULTI,
    CALIBRATE_NEAR_DONE,
    CALIBRATE_DONE,
    SCAN_PARAMETERS,
    SCAN_DO,
    MEASURE_PARAMETERS,
    MEASURE_DO,
    MULTI_PARAMETERS,
    MULTI_DO_STAGE_1,
    MULTI_DO_STAGE_2,
    MULTI_DO_STAGE_3,
    MULTI_DO_STAGE_4,
    MULTI_DONE
}

int main(void)
{
    lcd_init();
    lcd_clear_display();
    lcd_send_str(0x00, "state 0");
    keypad_init();
    keypad_enable_int();
}

void EINT3_IRQHandler(void) 
{
    static int state = 0;

    if (keypad_is_set('A')) {
	state = 1;
    }

    if (GPIO_GetIntStatus(0, 23, 1))
    { 
        keypad_clear_int();
        char r[16] = {0};
	get_keyboard_presses(r);
	int i;
	for(i = 0; i < 16; ++i){
	    if(r[i] == 1){
		if ((KEYS[i] == 'A'){
		    lcd_send_str(0x00, "Last char %c");
		}
	    }
	}
    }
}
