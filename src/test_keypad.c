#include <stdio.h>
#include <stdlib.h>
#include "keypad.h"
#include "lcd.h"

int c = 0;

int main(void)
{
    keypad_init();
    lcd_init();
    lcd_clear_display();

    char * r_prev = NULL;
    while(1) {
        char * r = malloc(16);
        get_keyboard_presses(r);

        int i;
        for (i = 0; i < 16; ++i) {
            if (r[i] == 1)
            {
                /* prevent same key held over two cycles being rendered
                 * a second time 
                 */
                if (r_prev != NULL)
                {
                    if (r[i] == r_prev[i])
                        continue;
                }
                if ((KEYS[i] >= '0') && (KEYS[i]<='9') && (c<=79)) 
                {
                    lcd_send_char(c, KEYS[i]);
                    c += 1;
                }

                /* jump to second line if top line is filled */
                if (c == 16) {
                    c = 64;
                }
            }
        }

        free(r_prev);
        r_prev = r;
        
        int max = 1 << 18;
        for (i = 0; i < max; ++i);
    }

    free(r_prev);
}
