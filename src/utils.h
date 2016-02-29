#include "empr.h"
#include "ir_sensor.h"
#include "ultrasound.h"

#define SAMPLE_SIZE 3

void utils_process_digit_input(int last_key_press, uint32_t* result);
int utils_compare_values(const void * elem1, const void * elem2);
uint32_t utils_get_ir_and_ultrasound_distance();
uint32_t utils_get_ir_and_ultrasound_median_distance();
