#ifndef SIMULADOR
#define  SIMULADOR

#include <limits.h>
#include <float.h>
#include <stdlib.h>
#include "get_candb.h" // include to structs.h, fifo.h

/* Definindo um tipo bool, se comportando como 0-1*/
typedef u_int8_t bool;
/* defines para tipos booleanos*/
#define FALSE                 0
#define false                 0
#define TRUE                  1
#define true                  1

#define K                     5

/* Variavel de controle para debug passo a passo do simulador CANbus*/
extern bool  DEBUG;
/* Variavel de controle para habilitar saida no terminal das frames lidas do arquivo*/
extern bool  PRINT_FRAMES;
/* Variavel de controle para saida de analises no terminal*/
extern bool  RESULTS;

/* Variavel de controle para escrita do arquivo de log BEST solution*/
extern bool logframes;

/* Arquivo de saida para Log de Bests gerado no S.A */
FILE*  Arq_Log_Best;

// Se existe fila agora.
static bool   length_queue = false;
// Se existiu fila na passada.
static bool   last_queue   = false;
// Time em que começou atual fila.
static double start_time_queue        = 0;
// Acumulado de cada fila
static u_int16_t current_length_queue = 0;


double     wcrt;
double     time_min_queue;
double     time_mean_queue;
double     time_max_queue;
double     busload_simulated;
double     time_current_simulation;

u_int32_t  frames_write;
u_int16_t  msg_deadline;
u_int32_t  number_of_queue;
u_int16_t  min_length_queue;
double     mean_length_queue;
u_int16_t  max_length_queue;
double     avg_length_queue;
double     acumul_time_queue;
double     acumul_length_queue;
double     acumul_length_queue_square;

fifo_t*    get_priority_frame();
double    get_mean_wcrt();
void      get_wcrt();
void      verific_wcrt();
void      verific_queue();
void      free_recurses();
void      verific_deadlines();
void      gravaLogFramesCab();
void      add_time_lost_arbitrage();
void      realloc_event(fifo_t* event);
void      gravaLogFrames(event_t event);
void      start_simulation(double time_of_simulation);
int       main_simulated(char* path, double time_simulation);


#endif
