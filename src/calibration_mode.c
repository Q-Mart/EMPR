#include "calibration_mode.h"
#include "lcd.h"

void calib_to_near_calib(){
    //Record values from both sensors
    //save values somewhere
    lcd_send_lines("Place object","at 30cm");
}
void near_calib_to_done(){
    //Record values
    //act on previously save values to calculate
    //constants for both sensors.
    lcd_send_str(0, "Calibration Complete");
}
void any_to_calib(){
    lcd_send_str(0, "Calibration Mode");
}
