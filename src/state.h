#ifndef STATE_H
#define STATE_H

typedef enum {
    CALIBRATE,
    SCAN,
    MEASURE,
    MULTI,
    PLATFORM,

    CALIBRATE_NEAR,
    CALIBRATE_NEAR_DONE,
    CALIBRATE_DONE,

    /* SCAN_PARAMETERS
     *  sent when going between SCAN and selecting
     *  which paramater to be chosen
     */
    SCAN_PARAMETERS,
    SCAN_PARAMETER_1, /* sweep 'speed' */
    SCAN_PARAMETER_2, /* start position */
    SCAN_PARAMETER_3, /* stop position */
    SCAN_PARAMETER_4, /* unused */
    SCAN_DO,

    MEASURE_DO,

    PLATFORM_SCAN, /* sweeping and scanning */
    PLATFORM_WAIT, /* wait for user to turn object */

    PLATFORM_DONE, /* Finished Scan so, idle */
    /* Special Ignore State */ ANY
} state_t;

void change_state(state_t state);
#endif
