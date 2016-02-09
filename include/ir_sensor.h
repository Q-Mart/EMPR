#include "adc.h"
#include <stdint.h>
#ifndef IR_SENSOR
#define IR_SENSOR
void ir_sensor_init();
void ir_sensor_set_far_point(uint32_t x); //20cm
void ir_sensor_set_near_point(uint32_t x); //10cm
void ir_sensor_calibrate(); //Uses near and far values to calibrate
uint32_t ir_sensor_get_distance();
uint32_t ir_sensor_get_raw_data();
uint32_t ir_convert_to_distance(uint32_t raw);
#endif
