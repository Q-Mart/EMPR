#include "queue.h"

#define REPLAY_SAMPLES 3000
#define RECORD_FILE "records/record"

extern QUEUE* IR_QUEUE;
extern QUEUE* KEYPAD_QUEUE;
extern QUEUE* ULTRASOUND_QUEUE;
extern QUEUE* SERVO_QUEUE;

#define SOCK_ADDR "/tmp/empr_ipc_socket"
extern int SOCK;
