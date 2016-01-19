#include <stdint.h>

#define LCD_ADDR 0x3b

/* LCD Line Addresses */
#define LINE1 0
#define LINE2 0x40

void lcd_init(void);
void lcd_wait_while_busy(void);
void lcd_clear_display(void);
void lcd_send_char(uint8_t, char);
void lcd_send_str(uint8_t, char*);
void lcd_send_pat(uint8_t, int);
void lcd_send_lines(char*, char*);
void lcd_send_strf(uint8_t, char*, ...);
