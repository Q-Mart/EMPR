#include "measure.h"
#include "lcd.h"
#include "ir_sensor.h"
#include "debug.h"
void any_to_measure(){
    lcd_send_line(LINE1, "Measure Mode");
}
void measure_loop(){
    int measure_byte = 0x02;
    uint32_t raw = ir_sensor_get_raw_data();
    debug_send_arb((char*) &measure_byte, 1);
    debug_send_arb((char*) &raw, 4);
    lcd_send_line(LINE2, "%f", ir_convert_to_distance(raw));
}
