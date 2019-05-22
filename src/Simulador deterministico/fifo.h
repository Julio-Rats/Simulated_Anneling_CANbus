#ifndef FIFO
#define  FIFO

#include "structs.h"
#include <stdio.h>
/*
   fifo_t struct Lista encadeada, contendo os evento ordenados no tempo de execução.
   atributos:
    event      = evento com frame e tempos para simular.
    next_event = ponteiro para proximo da lista.
    prev_event = ponteiro para anterior da lista.

*/

typedef struct fifo_t fifo_t;
struct fifo_t{

  event_t  event;
  fifo_t*   next_event;
  fifo_t*   prev_event;

};

typedef struct{

  fifo_t* first;

}head_t;

head_t* list_event; // list_event = ponteiro da lista de eventos.

head_t* init_list();
void    add_list(event_t new_event);
void    rem_list(fifo_t* event);
void    organize_list();


#endif
