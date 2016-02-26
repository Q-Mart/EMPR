#ifndef STATE_H
#define STATE_H

typedef enum {
    CALIBRATE,
    //SCAN,
    MEASURE,
    MULTI,
    TRACK,

    CALIBRATE_NEAR,
    CALIBRATE_NEAR_DONE,
    CALIBRATE_DONE,

    /* SCAN_PARAMETERS
     *  sent when going between SCAN and selecting
     *  which paramater to be chosen
     */
    //SCAN_PARAMETERS,
    //SCAN_PARAMETER_1, /* sweep 'speed' */
    //SCAN_PARAMETER_2, /* start position */
    //SCAN_PARAMETER_3, /* stop position */
    //SCAN_PARAMETER_4, /* unused */
    //SCAN_DO,

    MEASURE_DO,

    MULTI_PARAMETERS,
    /* MULTI_PARAMETERS goes to a copy of SCAN_PARAMETERS
     * with return to MULTI_DO
     */

    MULTI_SWEEP, /* sweeping and scanning */
    MULTI_WAIT, /* wait for user to turn object */
    MULTI_SWEEP_NUMBER, /* settings: specify sweep number */
    MULTI_MIN_ANGLE, /* settings: specify minimum sweep angle */
    MULTI_MAX_ANGLE, /* settings: specify maximum sweep angle */

    MULTI_DONE, /* Finished Scan so, idle */
    /* Special Ignore State */ ANY
} state_t;

void change_state(state_t state);
#endif
