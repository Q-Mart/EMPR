#include "utils.h"
uint32_t utils_last_ir = 0;
uint32_t utils_last_us = 0;

void utils_process_digit_input(int last_key_press, uint32_t* result){
    if (last_key_press >= 0){
        (*result) = ((*result) * 10) + last_key_press;
    } else if (last_key_press == -1){
        (*result) /= 10;//Remove the last digit.
        //Works because C truncates towards 0.
    }
}

int utils_compare_values(const void * elem1, const void * elem2) {

    int a = *((uint32_t*)elem1);
    int b = *((uint32_t*)elem2);
    
    if (a > b) return 1;
    if (b < a) return -1;

    return 0;
}

uint32_t utils_get_ir_and_ultrasound_distance() {
    ultrasound_send_test_pulse();
    uint32_t raw_ir = ir_sensor_get_raw_data();
    raw_ir = ir_convert_to_distance(raw_ir);
    uint32_t raw_us = ultrasound_get_distance();
    uint32_t raw = (raw_ir + raw_us) /2;
    return raw;
}
uint32_t utils_get_last_ir_reading(){
    return utils_last_ir;
}
uint32_t utils_get_last_us_reading(){
    return utils_last_us;
}

uint32_t utils_get_ir_and_ultrasound_median_distance() {
    int i;
    uint32_t ultrasound_samples[SAMPLE_SIZE];
    uint32_t ir_samples[SAMPLE_SIZE];
    uint32_t raw_ir;
    uint32_t raw_us;
    uint32_t result;

    ultrasound_send_test_pulse();

    for(i=0;i<SAMPLE_SIZE;i++) {
        ultrasound_samples[i] = ultrasound_get_distance();

        raw_ir = ir_sensor_get_raw_data();
        ir_samples[i] = ir_convert_to_distance(raw_ir);
    }

    qsort(ultrasound_samples, SAMPLE_SIZE, sizeof(uint32_t), utils_compare_values);
    qsort(ir_samples, SAMPLE_SIZE, sizeof(uint32_t), utils_compare_values);

    //Get median values
    raw_us = ultrasound_samples[SAMPLE_SIZE/2];
    raw_ir = ir_samples[SAMPLE_SIZE/2];
    utils_last_us = raw_us;
    utils_last_ir = raw_ir;

    result = (raw_ir + raw_us) / 2;

    return result;
}
