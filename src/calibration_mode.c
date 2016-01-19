#include "calibration_mode.h"
#include "lcd.h"
#include "ir_sensor.h"


void calib_to_near_calib(){
    //Record values from both sensors
    //save values somewhere
    ir_sensor_set_near_point(ir_sensor_get_raw_data());
    lcd_send_lines("Place object","at 30cm");
}
void near_calib_to_done(){
    //Record values
    //act on previously save values to calculate
    //constants for both sensors.
    ir_sensor_set_far_point(ir_sensor_get_raw_data());
    ir_sensor_calibrate();
    lcd_send_lines("Calibration", "Complete");
}
void any_to_calib(){
    lcd_send_str(0,"Calibration Mode");
    lcd_send_str(0x40, "Place at 15cm");
}
