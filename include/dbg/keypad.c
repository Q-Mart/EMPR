#include "empr.h"
#include "dbg.h"

#include "keypad.h"


void keypad_init(void)  {}
void keypad_enable_int(void) {}
void keypad_clear_int(void) {}
int keypad_get_int(void) {}
char poll_keyboard(char column) {}

void get_keyboard_presses(char * r) {
    *r = dequeue(KEYPAD_QUEUE, char);
}
