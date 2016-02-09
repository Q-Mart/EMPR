#include "measure.h"
#include "lcd.h"
#include "ir_sensor.h"
#include "debug.h"
#include "state.h"
#include "network.h"

void any_to_measure() {
    lcd_send_line(LINE1, "Measure Mode");
}

void measure_loop() {
    uint8_t measure_byte = MEASURE_DO;
    uint32_t raw = ir_sensor_get_raw_data();

    network_send(MEASURE_DO, 4, (uint8_t *)&raw);
    lcd_send_line(LINE2, "%f", ir_convert_to_distance(raw));
}
