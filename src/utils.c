#include "utils.h"

void utils_process_digit_input(int last_key_press, uint16_t* result){
    if (last_key_press >= 0){
        (*result) = ((*result) * 10) + last_key_press;
    } else if (last_key_press == -1){
        (*result) /= 10;//Remove the last digit.
        //Works because C truncates towards 0.
    }
}

uint32_t utils_get_ir_and_ultrasound_distance() {
    uint32_t raw = ir_sensor_get_raw_data();
    raw = ir_convert_to_distance(raw);
    uint32_t raw_us = ultrasound_get_distance();
    raw = (raw + raw_us) /2;
    return raw;
}
