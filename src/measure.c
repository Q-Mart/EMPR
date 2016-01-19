#include "measure.h"
#include "lcd.h"
#include "ir_sensor.h"
#include "debug.h"
void any_to_measure(){
    lcd_send_str(0, "Measure Mode");
}
void measure_loop(){
    int measure_byte = 0x02;
    lcd_clear_display();
    float dist = ir_sensor_get_distance();
    debug_send_arb((char*) &measure_byte, 1);
    debug_send_arb((char*) &dist, 4);
    lcd_send_strf(0, "%f", dist);
}
