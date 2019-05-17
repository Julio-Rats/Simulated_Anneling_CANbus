#ifndef SIMULADOR
#define  SIMULADOR

#include "get_candb.h" // include to structs.h, fifo.h

#define DEBUG        0
#define PRINT_FRAMES 0
#define RESULTS      0

double wcrt;
double end_time_queue;
double time_min_queue;
double time_max_queue;
double time_mean_queue;
double start_time_queue;
double busload_simulated;
double time_current_simulation;

u_int32_t frames_write;
u_int16_t length_queue;
u_int16_t msg_deadline;
u_int16_t number_of_queue;
u_int16_t max_length_queue;
u_int16_t min_length_queue;
u_int16_t acumul_length_queue;
u_int16_t current_length_queue;

fifo_t* get_priority_frame();
double small_time(double time);
double get_mean_wcrt();
void   get_wcrt();
void   check_time();
void   verific_wcrt();
void   verific_queue();
void   free_recurses();
void   verific_deadlines();
void   realloc_event(fifo_t* event);
void   add_time_lost_arbitrage(double time);
void   start_simulation(double time_of_simulation);
int    main_simulated(char* path, double time_simulation);


#endif
