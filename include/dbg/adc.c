#include "empr.h"
#include <stdio.h>
#include <stddef.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>

#include "adc.h"
#include "dbg.h"
#include "network.h"
#include "debug.h"

#define ADC_CLOCKRATE 200000

QUEUE* IR_QUEUE;
QUEUE* KEYPAD_QUEUE;
QUEUE* ULTRASOUND_QUEUE;
QUEUE* SERVO_QUEUE;

int SOCK;

void adc_enable(void) {
    /* Load QUEUEs from file */
    debug_sendf("adc_enable\r\n");

    IR_QUEUE = new_queue(REPLAY_SAMPLES);
    KEYPAD_QUEUE = new_queue(REPLAY_SAMPLES);
    ULTRASOUND_QUEUE = new_queue(REPLAY_SAMPLES);
    SERVO_QUEUE = new_queue(REPLAY_SAMPLES);
    debug_sendf("made queues\r\n");

    FILE* f;
    debug_sendf("made file ptr\r\n");
    f = fopen(RECORD_FILE, "r");
    debug_sendf("fopen'd\r\n");
    if (f != NULL) 
    {
        debug_sendf("Reading from file\r\n");
        uint8_t c[1] = {0};
        while (fread(c, sizeof(uint8_t), 1, f))
        {
            void* ptr;
            switch (*c)
            {
                case IR_HEADER:
                    ptr = malloc(sizeof(uint32_t));
                    fread(ptr, sizeof(uint32_t), 1, f);
                    enqueue(IR_QUEUE, ptr);
                    break;
                case KEYPAD_HEADER:
                    ptr = malloc(16 * sizeof(uint8_t));
                    fread(ptr, sizeof(char), 16, f);
                    enqueue(KEYPAD_QUEUE, ptr);
                    break;
                case ULTRASOUND_HEADER:
                    ptr = malloc(sizeof(uint32_t));
                    fread(ptr, sizeof(uint32_t), 1, f);
                    enqueue(KEYPAD_QUEUE, ptr);
                    break;
                case SERVO_HEADER:
                    ptr = malloc(sizeof(uint32_t));
                    fread(ptr, sizeof(uint32_t), 1, f);
                    enqueue(SERVO_QUEUE, ptr);
                    break;
            }
        }

        fclose(f);
        debug_sendf("Read all Queues\r\n");
    }
    else
    {
        perror(RECORD_FILE);
        exit(0);
    }

    SOCK = socket(AF_UNIX, SOCK_DGRAM, 0);
    struct sockaddr_un addr;
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCK_ADDR, sizeof(addr.sun_path) - 1);
    connect(SOCK, (struct sockaddr*)&addr, sizeof(addr));
}

void adc_enable_channel(int channel) {}

int adc_get_channel_data(int channel) {
    return channel;
}
