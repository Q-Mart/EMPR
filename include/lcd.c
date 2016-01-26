#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "i2c.h"
#include "lcd.h"

/* buffer commands
 * for performance */
static char* buf;
#define sz 80

void lcd_send_buf(void);
void lcd_send_char(uint8_t, char);
void lcd_send_str(uint8_t, char*);
void lcd_send_pat(uint8_t, char);

void lcd_init(void)
{
    i2c_enable_mbed(LPC_I2C1);

    /* setup */
    char data0[] = { 0x00, 0x34, 0x0c, 0x06, 0x35, 0x04, 0x10, 0x42, 0x9f, 0x34, 0x02 };
    i2c_send_mbed_polling(LPC_I2C1, LCD_ADDR, sizeof(data0), data0);

    buf = (char* )malloc(sz);
}

void lcd_clear_display(void)
{
    char data1[] = { 0x00, 0x01 };
    i2c_send_mbed_polling(LPC_I2C1, LCD_ADDR, sizeof(data1), data1);
    lcd_wait_while_busy();


    int i;
    for (i = 0; i < sz; ++i) {
        lcd_send_char(LINE1 + i, ' ');
    }

    lcd_send_buf();
}

void lcd_wait_while_busy(void)
{
    char buf = 0x80;
    while ((buf & 0x80) != 0x00)
    {
        if (!i2c_recv_mbed_polling(LPC_I2C1, LCD_ADDR, 1, &buf))
            break;
    }
}

/* For internal use only
 *  this _will not_ output a char to the lcd on call
 *  rather it will append it to the buffer for display on
 *  call to lcd_send_buf()
 */
void lcd_send_char(uint8_t ddram_addr, char c)
{
    if (c >= 'a' && c <= 'z') {
        c = 0x61 + (c - 'a');
    } else if (c >= 'A' && c <= 'Z') {
        c = 0xC1 + (c - 'A');
    } else if (c >= '0' && c <= '9') {
        c = 0xb0 + (c - '0');
    } else {
        switch (c)
        {
            case ' ':
                c = 0xA0;
                break;
            case '!':
                c = 0xA1;
                break;
            case '"':
                c = 0xA2;
                break;
            case '#':
                c = 0xA3;
                break;
            case '%':
                c = 0xA5;
                break;
            case '(':
                c = 0xA8;
                break;
            case ')':
                c = 0xA9;
                break;
            case ',':
                c = 0xAC;
                break;
            case '.':
                c = 0xAE;
                break;
            case '*':
                c = 0xAA;
                break;
            case '-':
                c = 0xAD;
                break;
            case '+':
                c = 0xAB;
                break;
            case '/':
                c = 0xAF;
                break;
        };
    }


    lcd_send_pat(ddram_addr, c);
}

void lcd_send_str(uint8_t ddram_addr, char* s)
{
    int i;
    for (i = 0; i < strlen(s); ++i) {
        lcd_send_char(ddram_addr + i, s[i]);
    }
}

void lcd_send_strf(uint8_t ddram_addr, char* fmt, ...)
{
   va_list ap;
   va_start(ap, fmt);

    char* buf = (char *)malloc(strlen(fmt));
    vsprintf(buf, fmt, ap);
    int i;

    for (i = 0; i < strlen(buf); ++i) {
        lcd_send_char(ddram_addr+i, buf[i]);
    }

    free(buf);
}

/* Send a char to the lcd ddram
 * ddram_addr has properties for this:
 *  0x00 -> 0x27 = LINE1
 *  0x40 -> 0x50 = LINE2
 */
void lcd_send_pat(uint8_t ddram_addr, char b)
{
    /* adjust ddram_addr to buffer index */
    uint8_t buf_i;
    buf_i = ddram_addr;

    if (ddram_addr >= LINE2) {
        buf_i = 0x28 + (ddram_addr - LINE2);
    }

    buf[buf_i] = b;
}

/* Draw the buffer to the lcd screen
 */
void lcd_send_buf(void)
{
    uint8_t ddram_addr = 0x80;

    char data[2 + 1 + 32] = { 0 };
    data[0] = 0x80;
    data[1] = ddram_addr;
    data[2] = 0x40;

    int i;
    /* send first line */
    for (i = 0; i < 32; ++i) {
        data[2 + 1 + i] = buf[i];
    }

    i2c_send_mbed_polling(LPC_I2C1, LCD_ADDR, 2 + 1 + 32, data);

    /* send next line */
    char data2[2 + 1 + 32] = { 0 };
    data2[0] = 0x80;
    data2[1] = 0x40 | 0x80;
    data2[2] = 0x40;
    for (i = 0; i < 32; ++i) {
        data2[2 + 1 + i] = buf[i + 0x28];
    }

    i2c_send_mbed_polling(LPC_I2C1, LCD_ADDR, 2 + 1 + 32, data2);
    lcd_wait_while_busy();
}

/* Send a string of 16 characters to either LINE1 or LINE2
 * uses formatting and varargs
 */
void lcd_send_line(uint8_t line, char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);

    char* buf = malloc(strlen(fmt));
    vsprintf(buf, fmt, ap);

    char* out[17] = { ' ' };
    strcpy(out, buf);

    /* add NUL to end so it can be used as a string */
    out[16] = '\0';
    lcd_send_str(line, out);
    lcd_send_buf();

    free(buf);
}

void lcd_send_lines(char* top, char* bottom)
{
    lcd_send_line(LINE1, top);
    lcd_send_line(LINE2, bottom);
}
