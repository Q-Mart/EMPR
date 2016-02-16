#include "empr.h"

#include "ir_sensor.h"
#include "network.h"


static uint32_t ir_sensor_m;
static uint32_t ir_sensor_c;
static uint32_t ir_near_point;
static uint32_t ir_far_point;
static int ir_sensor_adc_channel = 1;
void ir_sensor_init(){
    adc_enable_channel(ir_sensor_adc_channel);
    ir_sensor_m = 0.0f;
    ir_sensor_c = 0.0f;
}

void ir_sensor_set_near_point(uint32_t x){
    ir_near_point = x;
}

void ir_sensor_set_far_point(uint32_t x){
    ir_far_point = x;
    ir_sensor_calibrate();
}

void ir_sensor_calibrate(){
    ir_sensor_m = (ir_near_point - ir_far_point) / ((1/150000.0f) - (1/300000.0f));
    ir_sensor_c = ir_far_point - (ir_sensor_m/300000.0f);
}

uint32_t ir_sensor_get_distance(){
    /* returns the distance in cm */
    uint32_t val = adc_get_channel_data(ir_sensor_adc_channel);
    return ir_convert_to_distance(val);
}
uint32_t ir_sensor_get_raw_data(){
	uint32_t data = adc_get_channel_data(ir_sensor_adc_channel);
#ifdef RECORD
	network_send(IR_HEADER, &data, sizeof(uint32_t), NULL);
#endif
    return data;
}
uint32_t ir_convert_to_distance(uint32_t raw){
  if (raw != ir_sensor_c){
      return (ir_sensor_m)/(raw - ir_sensor_c);
  } else {
    return ir_sensor_m;
  }
}
