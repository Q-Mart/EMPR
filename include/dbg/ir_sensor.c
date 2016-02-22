#include "empr.h"
#include "dbg.h"

#include "ir_sensor.h"

static uint32_t ir_sensor_m;
static uint32_t ir_sensor_c;
static uint32_t ir_near_point;
static uint32_t ir_far_point;

void ir_sensor_init()
{
    ir_sensor_m = 0.0f;
    ir_sensor_c = 0.0f;
}

void ir_sensor_set_near_point(uint32_t x)
{
    ir_near_point = x;
}

void ir_sensor_set_far_point(uint32_t x)
{
    ir_far_point = x;
    ir_sensor_calibrate();
}

void ir_sensor_calibrate()
{
    ir_sensor_m = (ir_near_point - ir_far_point) / ((1/15000.0f) - (1/30000.0f));
    ir_sensor_c = ir_far_point - (ir_sensor_m/30000.0f);
}

uint32_t ir_sensor_get_distance()
{
    /* returns the distance in cm */
    uint32_t val = ir_sensor_get_raw_data();
    return ir_convert_to_distance(val);
}

uint32_t ir_sensor_get_raw_data()
{
    return dequeue(IR_QUEUE, uint32_t);
}

uint32_t ir_convert_to_distance(uint32_t raw)
{
  if (raw != ir_sensor_c){
      return (ir_sensor_m)/(raw - ir_sensor_c);
  } else {
    return ir_sensor_m;
  }
}
