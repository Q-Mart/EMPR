#include "gpio.h"
#include "pinsel.h"
#include "stepper.h"
#include "lpc17xx_pinsel.h"

void stepper_init(){
    pinsel_enable_pin(PINSEL_PORT_2, PINSEL_PIN_5, PINSEL_FUNC_0);
    pinsel_enable_pin(PINSEL_PORT_2, PINSEL_PIN_4, PINSEL_FUNC_0);
    pinsel_enable_pin(PINSEL_PORT_2, PINSEL_PIN_3, PINSEL_FUNC_0);
    pinsel_enable_pin(PINSEL_PORT_2, PINSEL_PIN_2, PINSEL_FUNC_0);

    init_general_gpio(PINSEL_PORT_2, PIN_5, 1);
    init_general_gpio(PINSEL_PORT_2, PIN_4, 1);
    init_general_gpio(PINSEL_PORT_2, PIN_3, 1);
    init_general_gpio(PINSEL_PORT_2, PIN_2, 1);
}

void stepper_send_nibble(int num){
    int val = num % 16;

    //clear all pins
    set_general_gpio(PINSEL_PORT_2 , PIN_5, 0);
    set_general_gpio(PINSEL_PORT_2 , PIN_4, 0);
    set_general_gpio(PINSEL_PORT_2 , PIN_3, 0);
    set_general_gpio(PINSEL_PORT_2 , PIN_2, 0);

    if (val-8 >= 0) {
        set_general_gpio(PINSEL_PORT_2 , PIN_5, 1);
        val = val - 8;
    }

    if (val-4 >= 0) {
        set_general_gpio(PINSEL_PORT_2, PIN_4, 1);
        val = val - 4;
    }

    if (val-2 >= 0) {
        set_general_gpio(PINSEL_PORT_2 , PIN_3, 1);
        val = val - 2;
    }

    if (val-1 >= 0) {
        set_general_gpio(PINSEL_PORT_2 , PIN_2, 1);
        val = val - 1;
    }
}
