#include "sound.h"
#include "dac.h"
#include "timer.h"
#include <math.h>
#include "debug.h"
#include "leds.h"

#define PI 3.14159265359

static volatile uint16_t sound_count;
static const uint16_t sound_max = 256;
static const float sound_max_amplitude = 511.0f;
static const uint16_t sound_amplitude_offset = 512;
static uint16_t sound_count_rate = 1;
static int sound_enabled = 0;
static int sound_tone = 0;
static const uint32_t sound_window_max = 4096;
static uint32_t sound_window_val = 256;
static const uint16_t sound_tone0_count_rate = 0;
static const uint16_t sound_tone1_count_rate = 64;
static uint32_t sound_window_count = 0;

void sound_init(void){
        sound_count = 0;
        init_gpio(LED1 | LED2 | LED3 | LED4);
        dac_init();
        timer_set_systick_function(&sound_tick);
        timer_enable_systick();
}
void sound_enable(){
    sound_enabled = 1;
}
void sound_disable(){
    sound_enabled = 0;
}

void sound_tick(void){
    if(sound_window_count > sound_window_val){
        sound_change_tone(1);
    } else {
        sound_change_tone(0);
    }
    sound_window_count = (sound_window_count + 1) % sound_window_max;
    if(sound_enabled){
        int val = (sin((((double)sound_count)/((double)sound_max))*PI*2) * sound_max_amplitude) + sound_amplitude_offset;
        dac_update(val);
        sound_count = (sound_count + sound_count_rate) % sound_max;

        //Flash the lights at the same rate as the beeping.
        if(sound_count >= (sound_max - sound_count_rate)){
            if(sound_window_val < 1024)
                set_gpio(LED1, 1);
            else if(sound_window_val < 2048)
                set_gpio(LED1 | LED2, 1);
            else if(sound_window_val < 3072)
                set_gpio(LED1 | LED2 | LED3, 1);
            else
                set_gpio(LED1 | LED2 | LED3 | LED4, 1);
        }else{
            set_gpio(LED1 | LED2 | LED3 | LED4, 0);
        }
    }
}
void sound_change_count_rate(uint16_t new_value){
	sound_count_rate = new_value;
}
void sound_change_tone(int tone){
    if(tone == 0){
        sound_count_rate = sound_tone0_count_rate;
    } else {
        sound_count_rate = sound_tone1_count_rate;
    }
}
void sound_change_tone0_window_size(uint32_t size){
    sound_window_val = size;
}
uint32_t sound_get_tone0_window_size(){
    return sound_window_max;
}
uint16_t sound_get_count_rate(void){
	return sound_count_rate;
}
