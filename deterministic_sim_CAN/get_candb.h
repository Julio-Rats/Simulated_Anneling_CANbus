#ifndef CANDB
#define  CANDB

#include "fifo.h"
#include <math.h>

#define SIMULATION_OK 0
#define ERROR_IO      1
#define ERROR_ARGS    2

#define BITS_FRAMES   70  // BITS 47 ->frame + 10 -> bit_stuff + 13 bit time
#define PAYLOAD_FRAME 64  // BITS
#define SPEED_BIT (double)(pow(2,20)/1000)

void   input_file(char* path);
double get_duration_frame(u_int8_t payload);

#endif
