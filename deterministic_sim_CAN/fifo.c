#include "fifo.h"

head_t* list_event = NULL;

head_t* init_list(){
  head_t* new_list = (head_t*) malloc(sizeof(head_t));
  new_list->first = NULL;
  return new_list;

}


void add_list(event_t new_event){
  if (!(list_event->first)){
      list_event->first = (fifo_t*) malloc(sizeof(fifo_t));
      list_event->first->event      = new_event;
      list_event->first->next_event = NULL;
      list_event->first->prev_event = NULL;
      return;
  }

  fifo_t* aux = list_event->first;
  int8_t flag = 0;

  while((aux->event.time_current <= new_event.time_current)){
      if (!(aux->next_event)){
          flag = 1;
          break;
      }
      aux = aux->next_event;
  }

  if(flag){
      aux->next_event = (fifo_t*) malloc(sizeof(fifo_t));
      aux->next_event->event      = new_event;
      aux->next_event->next_event = NULL;
      aux->next_event->prev_event = aux;
      return;
  }

  fifo_t* intermission = (fifo_t*) malloc(sizeof(fifo_t));
  intermission->event      = new_event;
  intermission->next_event = aux;
  intermission->prev_event = aux->prev_event;

  if (aux->prev_event){
      aux->prev_event->next_event = intermission;
  }else{
      list_event->first = intermission;
  }
  aux->prev_event = intermission;

}

void rem_list(fifo_t* event){
    if(!event->prev_event){
        list_event->first = event->next_event;
    }else{
        event->prev_event->next_event = event->next_event;
    }
    if(event->next_event){
        event->next_event->prev_event = event->prev_event;
    }
    free(event);
}
