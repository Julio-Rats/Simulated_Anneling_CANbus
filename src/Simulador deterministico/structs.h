#ifndef STRUCTS
#define  STRUCTS

#include <stdlib.h>

#define SIMULATION_OK           0
#define ERROR_IO                1
#define ERROR_ARGS              2
#define ERROR_MEMORY            3

#define PATH_LEN                128

#define BITS_FRAMES             70
#define PAYLOAD_FRAME           64
#define SPEED_BIT               (double)(pow(2,20)/1000)


/*
    frame_t struct definindo um quadro CAN2.0A.
    atributos:
     id         = Ident. do quadro CAN <0..2047>.
     cycle_time = tempo de uma nova 'msg' ser escrita no barramento <'-1' para msg não ciclicas>.
     payload    = Numero natural que define quantos Bytes o quadro possui <1..8>.

*/

typedef struct{

  u_int16_t id;
  double    cycle_time;
  double    delay_start_time;
  double    wcrt;
  double    payload;

}frame_t;

/*
    event_t struct de msg querendo acessar o barramento com seus tempos.
    atributos:
     frame        = frame a ser escrito no barramento.
     time_current = tempo de solicitação para escrita no barramento.
     time_happen  = tempo que podera ser escrito se ganhar na arbitragem nesse momento.

*/

typedef struct{

  frame_t frame;
  double  duration;
  double  time_current;
  double  time_happen;

}event_t;


#endif
