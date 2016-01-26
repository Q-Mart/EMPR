#include <stdint.h>

#define LCD_ADDR 0x3b

/* LCD Line Addresses */
#define LINE1 0
#define LINE2 0x40

void lcd_init(void);
void lcd_wait_while_busy(void);
void lcd_clear_display(void);

void lcd_send_line(uint8_t, char*, ...);
