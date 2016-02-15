#include "empr.h"
#include "dbg.h"

#include "keypad.h"

QUEUE* KEYPAD_QUEUE;

void keypad_init(void) 
{
    KEYPAD_QUEUE = new_queue(REPLAY_SAMPLES);
}

void keypad_enable_int(void) {}
void keypad_clear_int(void) {}
int keypad_get_int(void) {}
void get_keyboard_presses(char * r) {}

char poll_keyboard(char column)
{
    return (char )dequeue(KEYPAD_QUEUE);
}
