#include "empr.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "lcd.h"
#include "dbg.h"

void lcd_init(void) {}
void lcd_clear_display(void) {}
void lcd_wait_while_busy(void) {}

static char top[17];
static char bot[17];

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
    else if (line == LINE2)
        s = bot;

    vsprintf(s, fmt, ap);
    s[16] = '\0';

    debug_sendf("Top: `%s`[%d]\r\n", top, strlen(top));
    debug_sendf("Bot: `%s`[%d]\r\n", bot, strlen(bot));

    uint8_t sz = strlen(top);
    send(SOCK, &sz, 1, 0);
    send(SOCK, top, sz, 0);

    sz = strlen(bot);
    send(SOCK, &sz, 1, 0);
    send(SOCK, bot, sz, 0);

    char data = '\0';
    send(SOCK, &data, 1, 0);

    va_end(ap);
}

void lcd_send_lines(char* top, char* bottom)
{
    lcd_send_line(LINE1, top);
    lcd_send_line(LINE2, bottom);
}
