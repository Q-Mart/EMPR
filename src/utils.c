#include "utils.h"

void utils_process_digit_input(int last_key_press, uint16_t* result){
    if (last_key_press >= 0){
        (*result) = ((*result) * 10) + last_key_press;
    } else if (last_key_press == -1){
        (*result) /= 10;//Remove the last digit. 
        //Works because C truncates towards 0.
    }
}
