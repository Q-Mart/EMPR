#include "empr.h"
#include "ir_sensor.h"

void ir_sensor_init() {}
void ir_sensor_set_near_point(uint32_t x) {}
void ir_sensor_set_far_point(uint32_t x) {}
void ir_sensor_calibrate() {}

uint32_t ir_sensor_get_distance() {
    return 0;
}

uint32_t ir_sensor_get_raw_data(){
    return 0;
}

uint32_t ir_convert_to_distance(uint32_t raw){
    return 0;
}
