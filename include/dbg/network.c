#include "empr.h"
#include "network.h"
#include "dbg.h"

#include <stdarg.h>

void network_send(state_t state_header, ...) {
    uint8_t bb = 0;
    uint8_t *b = &bb;

    va_list ap;
    va_start(ap, b);

    send(SOCK_NETWORK, &state_header, 1, 0);

    while (1) {
        b = va_arg(ap, uint8_t*);
        if (b == NULL) {
            break;
        }

        uint8_t k = va_arg(ap, int);
        send(SOCK_NETWORK, b, k, 0);
    }

    va_end (ap);
}

void record(char header, ...) {}
