#include "servo.h"

#include "dbg.h"

void servo_init() {}

uint16_t servo_set_pos(int angle){
    return angle;
}

uint16_t servo_get_pos(){
    return dequeue(SERVO_QUEUE, uint16_t);
}
