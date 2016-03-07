#include "queue.h"

#define REPLAY_SAMPLES 100000
#define RECORD_FILE "records/record"

extern QUEUE* IR_QUEUE;
extern QUEUE* KEYPAD_QUEUE;
extern QUEUE* ULTRASOUND_QUEUE;
extern QUEUE* SERVO_QUEUE;

#define SOCK_ADDR_LCD "/tmp/empr_ipc_socket_lcd"
#define SOCK_ADDR_NETWORK "/tmp/empr_ipc_socket_network"
extern int SOCK_LCD;
extern int SOCK_NETWORK;
