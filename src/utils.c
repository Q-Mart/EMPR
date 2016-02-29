#include "utils.h"
#include "ultrasound.h"

void utils_process_digit_input(int last_key_press, uint32_t* result){
    if (last_key_press >= 0){
        (*result) = ((*result) * 10) + last_key_press;
    } else if (last_key_press == -1){
        (*result) /= 10;//Remove the last digit.
        //Works because C truncates towards 0.
    }
}

uint32_t utils_get_ir_and_ultrasound_distance() {
    ultrasound_send_test_pulse();
    uint32_t raw_ir = ir_sensor_get_raw_data();
    raw_ir = ir_convert_to_distance(raw_ir);
    uint32_t raw_us = ultrasound_get_distance();
    uint32_t raw = (raw_ir + raw_us) /2;
    return raw;
}

uint32_t utils_get_ir_and_ultrasound_median() {}
