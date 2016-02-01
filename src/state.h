typedef enum {
        CALIBRATE,
        SCAN,
        MEASURE,
        MULTI,

        CALIBRATE_NEAR_DONE,
        CALIBRATE_DONE,

        SCAN_PARAMETERS,
        SCAN_PARAMATER_1,
        SCAN_PARAMATER_2,
        SCAN_PARAMATER_3,
        SCAN_DO,

        MEASURE_PARAMETERS,
        MEASURE_PARAMATER_1,
        MEASURE_PARAMATER_2,
        MEASURE_PARAMATER_3,
        MEASURE_DO,

        MULTI_PARAMETERS,
        MULTI_DO_STAGE_1,
        MULTI_DO_STAGE_2,
        MULTI_DO_STAGE_3,
        MULTI_DO_STAGE_4,
        MULTI_DONE,
        ANY
} state_t;
