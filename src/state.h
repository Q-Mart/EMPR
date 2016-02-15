#ifndef STATE_H
#define STATE_H

typedef enum {
    CALIBRATE,
    SCAN,
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
    SCAN_PARAMETERS,
    SCAN_PARAMETER_1, /* sweep 'speed' */
    SCAN_PARAMETER_2, /* no of samples per sweep */
    SCAN_PARAMETER_3, /* start position */
    SCAN_PARAMETER_4, /* stop position */
    SCAN_DO,

    MEASURE_DO,

    MULTI_PARAMETERS,
    /* MULTI_PARAMETERS goes to a copy of SCAN_PARAMETERS
     * with return to MULTI_DO
     */

    MULTI_DO,
    MULTI_DO_STAGE_1, /* First Sweep */
    MULTI_DO_STAGE_2, /* Second Sweep */
    MULTI_DO_STAGE_3, /* Third Sweep */
    MULTI_DO_STAGE_4, /* Fourth Sweep */

    MULTI_DONE, /* Finished Scan so, idle */
    /* Special Ignore State */ ANY
} state_t;

#endif
