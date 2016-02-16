#include "sound.h"
#include "dac.h"
#include "timer.h"
#include <math.h>
#include "debug.h"

#define PI 3.14159265359

static volatile uint16_t sound_count;
static const uint16_t sound_max = 256;
static const float sound_max_amplitude = 511.0f;
static const uint16_t sound_amplitude_offset = 512;
static uint16_t sound_count_rate = 1;

void sound_init(void){
        sound_count = 0;
        dac_init();
        timer_set_systick_function(&sound_tick);
        timer_enable_systick();
}

void sound_tick(void){
        int val = (sin((((double)sound_count)/((double)sound_max))*PI*2) * sound_max_amplitude) + sound_amplitude_offset;
        dac_update(val);
        sound_count = (sound_count + sound_count_rate) % sound_max;
}
void sound_change_count_rate(uint16_t new_value){
	sound_count_rate = new_value;
}
uint16_t sound_get_count_rate(void){
	return sound_count_rate;
}
