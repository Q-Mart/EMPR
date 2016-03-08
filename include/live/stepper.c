#include "gpio.h"
#include "pinsel.h"
#include "stepper.h"
#include "timer.h"
#include "debug.h"
#include "lpc17xx_pinsel.h"

static int step_number = 0;

void stepper_init() {
    pinsel_enable_pin(PINSEL_PORT_2, PINSEL_PIN_5, PINSEL_FUNC_0);
    pinsel_enable_pin(PINSEL_PORT_2, PINSEL_PIN_4, PINSEL_FUNC_0);
    pinsel_enable_pin(PINSEL_PORT_2, PINSEL_PIN_3, PINSEL_FUNC_0);
    pinsel_enable_pin(PINSEL_PORT_2, PINSEL_PIN_2, PINSEL_FUNC_0);

    init_general_gpio(PINSEL_PORT_2, PIN_5 | PIN_4 | PIN_3 | PIN_2, 1);
}

void stepper_send_nibble(int num) {
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

void stepper_step_clockwise() {
    const int steps[NUMBER_OF_NIBBLE_STEPS] = {STEP_1, STEP_2,
                                               STEP_3, STEP_4};

    if (step_number>200) {
        step_number = 0;
    }

    stepper_send_nibble(steps[step_number%NUMBER_OF_NIBBLE_STEPS]);
    step_number++;
    timer_delay(MIN_DELAY);
}

void stepper_step_anticlockwise() {
    const int steps[NUMBER_OF_NIBBLE_STEPS] = {STEP_1, STEP_2,
                                               STEP_3, STEP_4};

    if (step_number<0) {
        step_number = 200;
    }

    stepper_send_nibble(steps[step_number%NUMBER_OF_NIBBLE_STEPS]);
    step_number--;
    timer_delay(MIN_DELAY);
}

void stepper_full_clockwise(int delay) {
    int i;

    for(i=0;i<NUMBER_OF_STEPS;i++) {
        stepper_step_clockwise();

        if (delay < MIN_DELAY) {
            delay = MIN_DELAY;
        }

        timer_delay(delay);
    }
}

void stepper_full_anticlockwise(int delay) {
    int i;

    for(i=0;i<NUMBER_OF_STEPS;i++) {
        stepper_step_anticlockwise();

        if (delay < MIN_DELAY) {
            delay = MIN_DELAY;
        }

        timer_delay(delay);
    }
}
