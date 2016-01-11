#define KEYPAD_ADDR 0x21

#define COLUMN1 0x7f
#define COLUMN2 0xbf
#define COLUMN3 0xdf
#define COLUMN4 0xef

static char KEYS[] = {
    '1', '2', '3', 'A', 
    '4', '5', '6', 'B', 
    '7', '8', '9', 'C', 
    '*', '0', '#', 'D'
};

void keypad_init(void);
void keypad_enable_int(void);
void keypad_clear_int(void);
void get_keyboard_presses(char *);
char poll_keyboard(char);
