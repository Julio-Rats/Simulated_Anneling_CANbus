#include "simulador.h"

int main_simulated(char* path, double time_simulation) {

    input_file(path);
    start_simulation(time_simulation);

    return SIMULATION_OK;
}

void start_simulation(double time_end_simulation){

    time_min_queue   = DBL_MAX;
    min_length_queue = USHRT_MAX;
    wcrt             = 0;
    number_of_queue  = 0;
    time_mean_queue  = 0;
    msg_deadline     = 0;
    frames_write     = 0;
    time_max_queue   = 0;
    length_queue     = 0;
    msg_deadline     = 0;
    max_length_queue = 0;
    fifo_t* aux       = NULL;
    acumul_length_queue     = 0;
    time_current_simulation = list_event->first->event.time_happen;

    #if LOGFRAMES
        ArqLog = fopen("LogFrames.txt", "w");
        if (!ArqLog){
            printf("\n================================================================================");
        		printf("\n[ERRO] Arquivo '%s' falhou em abrir\n\n", "LogFrames");
        		printf("\n================================================================================\n");
        		exit(ERROR_IO);
        }
        fprintf(ArqLog, "ID\tCYCLE\tTIME_CORRECT\tTIME_LATE\tDELAYED\tDURATION\n");
    #endif

    while(time_current_simulation < time_end_simulation){
          frames_write++;
          aux = get_priority_frame();
          #if LOGFRAMES
              gravaLogFrames(aux->event);
          #endif
          add_time_lost_arbitrage(aux->event.duration);
          realloc_event(aux);
          verific_queue();
          verific_deadlines();
          verific_wcrt();

          #if DEBUG
              system("clear");
              for(fifo_t* aux=list_event->first;aux;aux=aux->next_event){
                  printf("ID = %lu\t", aux->event.frame.id);
                  printf("cycle = %lf\t", aux->event.frame.cycle_time);
                  printf("Current_time = %lf\t", aux->event.time_current);
                  printf("Happen_time = %lf\n", aux->event.time_happen);
              }
              printf("\nWRITE ID %d\tStart Time ", aux->event.frame.id);
              printf("%lf\n\n", time_current_simulation);
              printf("tamanho da fila \t%d\n", length_queue);
              printf("tamanho max fila\t%d\n", max_length_queue);
              printf("tempo inicial da fila\t%lf\n", start_time_queue);
              printf("tempo final da fila\t%lf\n", end_time_queue);
              printf("tempo max da fila\t%lf\n", time_max_queue);
              getchar();
          #endif

          time_current_simulation = small_time(time_end_simulation);
          check_time();
    } // while

    #if PRINT_FRAMES
        printf("\n");
        for(fifo_t* aux=list_event->first;aux;aux=aux->next_event){
          printf("ID      = %lu\n", aux->event.frame.id);
          printf("T_Cycle = %lf\n", aux->event.frame.cycle_time);
          printf("S_Delay = %lf\n\n", aux->event.frame.delay_start_time);
        }
        printf("\n");
    #endif

    get_wcrt();
    double mean_wcrt  = get_mean_wcrt();
    busload_simulated = (((frames_write*(BITS_FRAMES+PAYLOAD_FRAME))/SPEED_BIT)/10)*(1000/time_current_simulation);
    free_recurses();

    #if RESULTS
        double print_mean_queue_length = 0;
        double print_mean_queue_time   = 0;

        if (number_of_queue == 0){
           min_length_queue = 0;
           time_min_queue   = 0;
        }else{
           print_mean_queue_length = ((double)acumul_length_queue/number_of_queue);
           print_mean_queue_time   = ((double)time_mean_queue/number_of_queue);
        }

        printf("\nFrames escritos     \t %d (Frames)\n",  frames_write);
        printf("Numero de deadlines   \t %d (Frames)\n\n", msg_deadline);

        printf("Numero de filas        \t %d (Unid.)\n",    number_of_queue);
        printf("Frames em fila acumul. \t %d (Unid.)\n\n",  acumul_length_queue);

        printf("Tamanho min de fila    \t %d (Frames)\n",   min_length_queue);
        printf("Tamanho medio de filas \t %lf (Frames)\n",  print_mean_queue_length);
        printf("Tamanho max de fila    \t %d (Frames)\n\n", max_length_queue);

        printf("Tempo min de fila      \t %lf (ms)\n",   time_min_queue);
        printf("Tempo medio de filas   \t %lf (ms)\n",   print_mean_queue_time);
        printf("Tempo max de fila      \t %lf (ms)\n\n", time_max_queue);

        printf("Soma de todos os WRCT  \t %lf (ms)\n",   wcrt);
        printf("Media WCRT             \t %lf (ms)\n\n", mean_wcrt);

        printf("Busload                \t %lf (%)\n",    busload_simulated);
        printf("Tempo de simulação     \t %lf (ms)\n\n", time_current_simulation);
    #endif

    #if LOGFRAMES
        fclose(ArqLog);
    #endif
}


void gravaLogFrames(event_t event){
    fprintf(ArqLog, "%ld\t%lf\t%lf\t%lf\t%lf\t%lf\n", event.frame.id, event.frame.cycle_time, event.time_current,
                  event.time_happen,event.time_happen-event.time_current, event.duration);
}

fifo_t* get_priority_frame(){

    fifo_t*    event_priority = list_event->first;
    u_int16_t high_priority  = event_priority->event.frame.id;

    for(fifo_t* aux=list_event->first->next_event; aux; aux=aux->next_event)
         if ((aux->event.frame.id < high_priority) && (aux->event.time_happen <= event_priority->event.time_happen)){
              high_priority  = aux->event.frame.id;
              event_priority = aux;
         }
    return event_priority;
}

void add_time_lost_arbitrage(double time){

    for(fifo_t* aux=list_event->first; aux; aux=aux->next_event)
      if (aux->event.time_happen <= time_current_simulation){
         aux->event.time_happen += time;
      }
}

void realloc_event(fifo_t* event){

    if (event->event.frame.cycle_time < 0){
        rem_list(event);
        return;
    }
    event->event.time_happen  = time_current_simulation + event->event.duration + event->event.frame.cycle_time;
    event->event.time_current = event->event.time_happen;
    event_t aux = event->event;
    rem_list(event);
    add_list(aux);
}

double small_time(double time){

    double menor = time;

    for(fifo_t* aux=list_event->first; aux; aux=aux->next_event)
      if (aux->event.time_happen < menor)
          menor = aux->event.time_happen;
    return menor;
}

void check_time(){

    for(fifo_t* aux=list_event->first; aux; aux=aux->next_event)
        if (aux->event.time_current < time_current_simulation)
           aux->event.time_happen = time_current_simulation;
}

void verific_queue(){

    u_int16_t count = 0;

    for(fifo_t* aux=list_event->first; aux; aux=aux->next_event)
        if (aux->event.time_current != aux->event.time_happen)
            count += 1;

    if (count != 0){
        if (length_queue == 0)
            start_time_queue = time_current_simulation;

        if (max_length_queue < count)
            max_length_queue = count;

        if (current_length_queue < count)
            current_length_queue = count;

    }else if (length_queue != 0){
        end_time_queue = time_current_simulation;
        if ((end_time_queue-start_time_queue) > time_max_queue)
            time_max_queue = ((double)end_time_queue-start_time_queue);

        if ((end_time_queue-start_time_queue) < time_min_queue)
            time_min_queue = ((double)end_time_queue-start_time_queue);

        if (min_length_queue > current_length_queue)
        min_length_queue = current_length_queue;

        number_of_queue++;
        acumul_length_queue += current_length_queue;
        time_mean_queue     += ((double)end_time_queue-start_time_queue);

        current_length_queue = 0;
    }
    length_queue = count;
}

void verific_deadlines(){

    u_int16_t count = 0;

    for(fifo_t* aux=list_event->first; aux; aux=aux->next_event)
       if (aux->event.time_happen >= (aux->event.frame.cycle_time+aux->event.time_current)) {
          count += 1;
          realloc_event(aux);
       }

    if (msg_deadline < count)
       msg_deadline = count;
}

void verific_wcrt(){

    for(fifo_t* aux=list_event->first; aux; aux=aux->next_event)
      if (aux->event.frame.wcrt < (aux->event.time_happen-aux->event.time_current))
         aux->event.frame.wcrt = (aux->event.time_happen-aux->event.time_current);

}

void get_wcrt(){
    wcrt = 0;
    for(fifo_t* aux=list_event->first; aux; aux=aux->next_event)
      // if (wcrt<aux->event.frame.wcrt){
          wcrt    += aux->event.frame.wcrt;
          // wcrt_id = aux->event.frame.id;
      // }
}

double get_mean_wcrt(){

    u_int16_t count = 0;
    double    media = 0;

    for(fifo_t* aux=list_event->first; aux; aux=aux->next_event){
        count++;
        media += aux->event.frame.wcrt;
    }
    return (double)(media/(double)count);
}

void free_recurses(){

    for(fifo_t* aux=list_event->first; aux; aux=aux->next_event)
       if (aux->prev_event)
          rem_list(aux->prev_event);
    rem_list(list_event->first);

}
