#include "adc.h"
#ifndef IR_SENSOR
#define IR_SENSOR
void ir_sensor_init();
void ir_sensor_set_far_point(float x); //20cm
void ir_sensor_set_near_point(float x); //10cm
void ir_sensor_calibrate(); //Uses near and far values to calibrate
float ir_sensor_get_distance();
float ir_sensor_get_raw_data();
#endif
