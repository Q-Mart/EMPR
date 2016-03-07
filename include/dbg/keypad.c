#include "empr.h"
#include "dbg.h"

#include <stdio.h>
#include "keypad.h"


void keypad_init(void)  {}
void keypad_enable_int(void) {}
void keypad_clear_int(void) {}
int keypad_get_int(void) {}
char poll_keyboard(char column) {}

void get_keyboard_presses(char * r) {
    char* s = ptr_dequeue(KEYPAD_QUEUE);
    int i;
    for (i = 0; i < 16; ++i) {
        r[i] = s[i];
    }
}
