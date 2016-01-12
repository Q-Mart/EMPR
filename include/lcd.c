#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "i2c.h"
#include "lcd.h"


void lcd_init(void)
{
    i2c_enable_mbed(LPC_I2C1);

    /* setup */
    char data0[] = { 0x00, 0x34, 0x0c, 0x06, 0x35, 0x04, 0x10, 0x42, 0x9f, 0x34, 0x02 };
    i2c_send_mbed_polling(LPC_I2C1, LCD_ADDR, sizeof(data0), data0);
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

/* output a character to the lcd with character set 'R' */
void lcd_send_char(char ddram_addr, char c)
{
    if (c >= 'a' && c <= 'z') {
        c = 0x61 + (c - 'a');
    } else if (c >= 'A' && c <= 'Z') {
        c = 0xC1 + (c - 'A');
    } else if (c >= '0' && c <= '9') {
        c = 0xb0 + (c - '0');
    } else {
        char lookup[] = { 
            ' ', 0xA0,
            '!', 0xA1,
            '"', 0xA2,
            '#', 0xA3,
            '%', 0xA5,
            '(', 0xA8,
            ')', 0xA9,
            ',', 0xAC,
            '.', 0xAE,
            '*', 0xAA,
            '-', 0xAD,
            '+', 0xAB,
            '/', 0xAF,
        };


        int i;
        for (i = 0; i < sizeof(lookup); i+=2) {
            if (c == lookup[i])
                c = lookup[i+1];
        }
    }

    lcd_send_pat(ddram_addr, c);
}

void lcd_send_str(char ddram_addr, char* s)
{
    int i;
    for (i = 0; i < strlen(s); ++i) {
        lcd_send_char(ddram_addr+i, s[i]);  
    }
}

void lcd_send_strf(char ddram_addr, char* fmt, ...)
{
   va_list ap; 
   va_start(ap, fmt);

    char* buf = (char *)malloc(strlen(fmt));
    vsprintf(buf, fmt, ap);
    int i;

    for (i = 0; i < strlen(s); ++i) {
        lcd_send_char(ddram_addr+i, buf[i]);  
    }

    free(buf);
}

void lcd_send_pat(char ddram_addr, int b)
{
    ddram_addr = ddram_addr | 0x80;
    char instr[] = {0x00, ddram_addr};
    i2c_send_mbed_polling(LPC_I2C1, LCD_ADDR, 2, instr);

    char data[] = {0x40, b};
    i2c_send_mbed_polling(LPC_I2C1, LCD_ADDR, 2, data);
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
