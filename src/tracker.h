#include <stdio.h>
#include <stdlib.h>
#include "lpc17xx_timer.h"
#include "lcd.h"
#include "debug.h"
#include "gpio.h"
#include "timer.h"
#include "ultrasound.h"
#include "servo.h"
#include "ir_sensor.h"

void tracker(void);
int tracker_full_scan(int, int, int, uint32_t*, uint32_t*);
void tracker_narrow_sweep();
void tracker_narrow_sweep_scan(int, int, uint32_t*, uint32_t*);
int tracker_process_range_table(uint32_t*, int);
int tracker_find_smallest_index(uint32_t*, int, int);
void tracker_set_bound();
int tracker_compare_indices(int, int);
void any_to_track(void);