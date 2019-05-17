#ifndef DRIVER
#define  DRIVER

#include <string.h>
#include "./../deterministic_sim_CAN/simulador.h"

#define PATH_SA "../Simulated_anneling/"

void   start_can_simulated(char* path_arq, double time_simulation);
double get_wcrt_simulation();
double get_busload_simulation();
double get_max_time_queue_simulation();

#endif // DRIVER
