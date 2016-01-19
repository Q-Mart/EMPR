#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "i2c.h"
#include "lcd.h"

static char* buf;

void lcd_init(void)
{
    i2c_enable_mbed(LPC_I2C1);

    /* setup */
    char data0[] = { 0x00, 0x34, 0x0c, 0x06, 0x35, 0x04, 0x10, 0x42, 0x9f, 0x34, 0x02 };
    i2c_send_mbed_polling(LPC_I2C1, LCD_ADDR, sizeof(data0), data0);

    buf = (char* )malloc(80);
}


void lcd_clear_display(void)
{
    char data1[] = { 0x00, 0x01 };
    i2c_send_mbed_polling(LPC_I2C1, LCD_ADDR, sizeof(data1), data1);
    lcd_wait_while_busy();

    int i;
    for (i = 0; i < 16; ++i) {
        lcd_send_char(LINE1 + i, ' '); 
        lcd_send_char(LINE2 + i, ' '); 
    }

    lcd_send_buf(0x00);
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
            case ':
                c = ', 0xAC;
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
        lcd_send_char(ddram_addr+i, s[i]);  
    }

    lcd_send_buf(0x00);
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

    lcd_send_buf(0x00);
    free(buf);
}

void lcd_send_pat(uint8_t ddram_addr, uint8_t b)
{
    buf[ddram_addr] = b;

/* do not remove- possible requirement to re-introduce
    ddram_addr = ddram_addr | 0x80;
    char instr[] = {0x00, ddram_addr};
    i2c_send_mbed_polling(LPC_I2C1, LCD_ADDR, 2, instr);

    char data[] = {0x40, b};
    i2c_send_mbed_polling(LPC_I2C1, LCD_ADDR, 2, data);
    lcd_wait_while_busy(); */
}

void lcd_send_buf(uint8_t ddram_addr)
{
    ddram_addr = ddram_addr | 0x80;

    char data[2 + 2*80] = { 0 };
    data[0] = 0x00;
    data[1] = ddram_addr;

    int i;
    for (i = 0; i < 80; ++i) {
        data[2 + 2*i] = 0x40;
        data[2 + 1 + 2*i] = buf[i];
    }

    i2c_send_mbed_polling(LPC_I2C1, LCD_ADDR, 2 + 2*80, data);
    lcd_wait_while_busy();
}

/* TODO: Make this faster */
void lcd_send_lines(char* top, char* bottom)
{
    char topstr[17] = { 0 };
    char botstr[17] = { 0 };
    int i;

    strcpy(topstr, top);
    for (i = strlen(top); i < 16; ++i)
    {
        topstr[i] = ' ';
    }
    topstr[16] = '\0';

    strcpy(botstr, bottom);
    for (i = strlen(bottom); i < 16; ++i)
    {
        botstr[i] = ' ';
    }
    botstr[16] = '\0';

    lcd_send_str(LINE1, topstr);
    lcd_send_str(LINE2, botstr);
}
