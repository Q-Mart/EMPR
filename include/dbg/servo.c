#include "servo.h"

#include "dbg.h"

void servo_init() {}

int servo_set_pos(int angle){
    return angle;
}

int servo_get_pos(){
    return *(uint32_t *)dequeue(SERVO_QUEUE);
}
