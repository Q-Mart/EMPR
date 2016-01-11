#define LCD_ADDR 0x3b

/* LCD Line Addresses */
#define LINE1 0
#define LINE2 0x40

void lcd_init(void);
void lcd_wait_while_busy(void);
void lcd_clear_display(void);
void lcd_send_char(char, char);
void lcd_send_str(char, char*);
void lcd_send_pat(char, int);
void lcd_send_lines(char*, char*);