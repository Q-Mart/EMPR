#include <stdarg.h>
#include "state.h"

void network_send(state_t state_header, ...) {
    /* Send a packet to the PC for external-data-display
    with header state_t type 'state_header' and varargs for 
    each data packet

    for instance `network_send(MEASURE_DO, 4, &data)`
        sends the MEASURE_DO byte followed by 4 bytes of the 
        *uint8_t 'data' (even if data was of type uint32_t)
    */

    uint8_t bb = 0;
    uint8_t *b = &bb;
    debug_send_arb(&state_header, 1);

    int n = 0;

    switch (*state_header)
    {
        case CALIBRATE_NEAR_DONE:
            n = 1;
        default:
            break;
    }

    va_list ap;
    va_start(ap, b);

    int i;
    for (i = 0; i < 2*n; i += 2) {
        b = va_arg(ap, (uint8_t *));
        uint8_t k = *b;
        
        b = va_arg(ap, (uint8_t *));
        debug_send_arb(b, k);
    }

    va_end(ap);
}
