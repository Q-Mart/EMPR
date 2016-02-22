#include "empr.h"

#include <stdio.h>
#include <stdarg.h>

#include "lcd.h"

void lcd_init(void) {}
void lcd_clear_display(void) {}
void lcd_wait_while_busy(void) {}

static char top[16];
static char bot[16];

/* Send a string of 16 characters to either LINE1 or LINE2
 * uses formatting and varargs
 * `fmt` must be NUL-terminated
 */
void lcd_send_line(uint8_t line, char* fmt, ...) {
    char* s = bot;
    va_list ap;
    va_start(ap, fmt);

    if (line == LINE1)
        s = top;

    vsprintf(s, fmt, ap);

    printf("\r: %-32s | %-32s", top, bot);
    va_end(ap);
}

void lcd_send_lines(char* top, char* bottom)
{
    lcd_send_line(LINE1, top);
    lcd_send_line(LINE2, bottom);
}
