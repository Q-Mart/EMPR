#include "sound.h"
#include "dac.h"
#include "timer.h"
#include <math.h>
#include "debug.h"

#define PI 3.14159265359

static volatile int count;
static const int max = 256;
static const float max_amplitude = 511.0f;
static const int amplitude_offset = 512;

void sound_init(void){
        count = 0;
        dac_init();
        timer_set_systick_function(&sound_tick);
        timer_enable_systick();
}

void sound_tick(void){
        int val = (sin((((double)count)/((double)max))*PI*2) * max_amplitude) + amplitude_offset;
        dac_update(val);
        count = (count + 1) % max;
}
