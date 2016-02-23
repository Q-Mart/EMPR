#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>

#include "network.h"
#include "debug.h"
#include "state.h"

void network_send(state_t state_header, ...) {
    /* Send a packet to the PC for external-data-display
       with header state_t type 'state_header' and varargs for
       each data packet

       for instance `network_send(MEASURE_DO, &data, 4)`
       sends the MEASURE_DO byte followed by 4 bytes of the
       *uint8_t 'data' (even if data was of type uint32_t)

       Terminate varargs with NULL
    */

#ifndef RECORD
    uint8_t bb = 0;
    uint8_t *b = &bb;

    va_list ap;
    va_start(ap, b);

    debug_send_arb((char *)&state_header, 1);

    while (1) {
        b = va_arg(ap, uint8_t*);
        if (b == NULL) {
            break;
        }

        uint8_t k = va_arg(ap, int);
        debug_send_arb(b, k);
    }

    va_end (ap);
#endif
}

void record(char header, ...) {
#ifdef RECORD
    uint8_t bb = 0;
    uint8_t *b = &bb;

    va_list ap;
    va_start(ap, b);
    debug_send_arb(&header, 1);

    while (1) {
        b = va_arg(ap, uint8_t*);
        if (b == NULL) {
            break;
        }

        uint8_t k = va_arg(ap, int);
        debug_send_arb(b, k);
    }

    va_end (ap);
#endif
}
