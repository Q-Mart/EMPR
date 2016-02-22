#include "empr.h"

#include <stdio.h>
#include <stdarg.h>

#include "lcd.h"

void lcd_init(void) {}
void lcd_clear_display(void) {}
void lcd_wait_while_busy(void) {}

/* Send a string of 16 characters to either LINE1 or LINE2
 * uses formatting and varargs
 * `fmt` must be NUL-terminated
 */
void lcd_send_line(uint8_t line, char* fmt, ...) {
    char s[16];
    va_list ap;
    va_start(ap, fmt);
    vsprintf(s, fmt, ap);

    printf("LCD: %s\r\n", s);
    va_end(ap);
}

void lcd_send_lines(char* top, char* bottom)
{
    printf("------\r\n");
    lcd_send_line(LINE1, top);
    lcd_send_line(LINE2, bottom);
    printf("------\r\n");
}
