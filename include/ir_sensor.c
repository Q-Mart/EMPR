#include "ir_sensor.h"

static float ir_sensor_m;
static float ir_sensor_c;
static float ir_near_point;
static float ir_far_point;
static int ir_sensor_adc_channel = 1;
void ir_sensorInit(){
    adc_enable_channel(ir_sensor_adc_channel);
    ir_sensor_m = 0.0f;
    ir_sensor_c = 0.0f;
}

void ir_sensor_set_near_point(float x){
    ir_near_point = x;
    ir_sensor_calibrate();
}

void ir_sensor_set_far_point(float x){
    ir_far_point = x;
    ir_sensor_calibrate();
}

void ir_sensor_calibrate(){
    ir_sensor_m = (ir_near_point - ir_far_point) / ((1.0f/15.0f) - (1.0f/30.0f));
    ir_sensor_c = ir_far_point - (ir_sensor_m/30.0f);
}

float ir_sensor_get_distance(){
    /* returns the distance in cm */
    float val = adc_get_channel_data(ir_sensor_adc_channel);
    return ir_sensor_m/(val - ir_sensor_c);
}
int ir_sensor_get_raw_data(){
    return adc_get_channel_data(ir_sensor_adc_channel);
}
float ir_convert_to_distance(int raw){
    return ir_sensor_m/(raw - ir_sensor_c);
}
