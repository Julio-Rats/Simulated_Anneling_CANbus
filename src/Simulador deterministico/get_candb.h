#ifndef CANDB
#define  CANDB

#include "fifo.h"
#include <math.h>

void   input_file(char* path);
double get_duration_frame(u_int8_t payload);

#endif
