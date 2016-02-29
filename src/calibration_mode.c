#include "calibration_mode.h"
#include "state.h"
#include "network.h"
#include "lcd.h"
#include "ir_sensor.h"
#include "ultrasound.h"


void calib_to_near_calib(){
    //Record values from both sensors
    //save values somewhere
    ir_sensor_set_near_point(ir_sensor_get_raw_data());
    ultrasound_set_near_point();
    lcd_send_line(LINE1, "Place object");
    lcd_send_line(LINE2, "at 30cm");
}

void near_calib_to_done(){
    //Record values
    //act on previously save values to calculate
    //constants for both sensors.
    ir_sensor_set_far_point(ir_sensor_get_raw_data());
    ir_sensor_calibrate();
    ultrasound_set_far_point();
    ultrasound_calibrate();
    lcd_send_line(LINE1, "Calibration");
    lcd_send_line(LINE2, "Complete");
}
void any_to_calib(){
    lcd_send_line(LINE1,"Calibration Mode");
    lcd_send_line(LINE2, "Place at 15cm");
    network_send(CALIBRATE, NULL);
}
