#ifndef DRIVER
#define  DRIVER

#include <string.h>
#include "./../Simulador deterministico/simulador.h"

#define PATH_SA           "../Simulated Anneling/"
#define LEN_STR           128
#define DR_ERRO_MEMORY    1

#define GET_WCRT()              &wcrt
#define GET_BUSLOAD()           &busload_simulated
#define GET_MAX_TIME_QUEUE()    &time_max_queue

// #define SYNC_FLAG_BEST()        logframes    = &log_flag;

void   start_can_simulated(char* path_arq, double time_simulation);

#endif // DRIVER
