/*  Networking Protocol
 *   For sending data to the PC for external-data-display
 */

#include "state.h"

/* #define RECORD */

/* Header for recording */
#define ADC_HEADER 0x01
#define I2C_HEADER 0x02
#define KEYPAD_HEADER 0x03
#define IR_HEADER 0x04
#define SERVO_HEADER 0x05
#define ULTRASOUND_HEADER 0x06

void network_send(state_t state_header, ...);
