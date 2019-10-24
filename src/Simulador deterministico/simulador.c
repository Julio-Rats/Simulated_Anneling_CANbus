#include "simulador.h"

bool  logframes    = FALSE;
bool  DEBUG        = FALSE;
bool  RESULTS      = TRUE;
bool  PRINT_FRAMES = FALSE;

int main_simulated(char* path, double time_simulation)
{

    input_file(path);
    start_simulation(time_simulation);

    return SIMULATION_OK;
}

void start_simulation(double time_end_simulation)
{
    time_min_queue              = DBL_MAX;
    acumul_time_queue           = 0;
    time_max_queue              = 0;
    frames_write                = 0;
    number_of_queue             = 0;
    min_length_queue            = USHRT_MAX;
    max_length_queue            = 0;
    avg_length_queue            = 0L;
    wcrt                        = 0;
    acumul_length_queue         = 0L;
    acumul_length_queue_square  = 0L;
    msg_deadline                = 0;
    fifo_t* prioritary_frame     = NULL;
    time_current_simulation     = list_event->first->event.time_happen;
    length_queue                = false;
    last_queue                  = false;
    start_time_queue            = 0;
    current_length_queue        = 0;

    if (logframes)
        gravaLogFramesCab();

    while(time_current_simulation <= time_end_simulation)
    {
          frames_write++;

          prioritary_frame = get_priority_frame();
          time_current_simulation = prioritary_frame->event.time_happen + prioritary_frame->event.duration;
          if (time_current_simulation > time_end_simulation)
              break;
          if (DEBUG)
          {
              system("clear");
              for(fifo_t* prioritary_frame=list_event->first;prioritary_frame;prioritary_frame=prioritary_frame->next_event)
              {
                    printf("ID = %lu\t", prioritary_frame->event.frame.id);
                    printf("cycle = %lf\t", prioritary_frame->event.frame.cycle_time);
                    printf("Current_time = %lf\t", prioritary_frame->event.time_current);
                    printf("Happen_time = %lf\t", prioritary_frame->event.time_happen);
                    printf("WCRT = %lf\n",  prioritary_frame->event.frame.wcrt);
              }
              printf("\nWRITE ID %d\tStart Time ", prioritary_frame->event.frame.id);
              printf("%lf\n\n", time_current_simulation);
              printf("tamanho da fila \t%lf\n", acumul_length_queue);
              printf("tamanho max fila\t%d\n", max_length_queue);
              printf("tempo max da fila\t%lf\n", time_max_queue);
              getchar();
          }

          if (logframes)
              gravaLogFrames(prioritary_frame->event);

          add_time_lost_arbitrage();
          realloc_event(prioritary_frame);
          verific_queue();
          verific_deadlines();
          verific_wcrt();
    } // while principal

    if (PRINT_FRAMES)
    {
        printf("\n");
        for(fifo_t* prioritary_frame=list_event->first;prioritary_frame;prioritary_frame=prioritary_frame->next_event)
        {
            printf("ID      = %lu\n",   prioritary_frame->event.frame.id);
            printf("T_Cycle = %lf\n",   prioritary_frame->event.frame.cycle_time);
            printf("S_Delay = %lf\n\n", prioritary_frame->event.frame.delay_start_time);
        }
        printf("\n");
    }

    get_wcrt();
    double mean_wcrt  = get_mean_wcrt();
    busload_simulated = (((frames_write*(BITS_FRAMES+PAYLOAD_FRAME))/SPEED_BIT)/10)*(1000/time_current_simulation);
    // free_recurses();

    double desvio = 0;

    if (number_of_queue == 0)
    {
         time_mean_queue   = 0;
         mean_length_queue = 0;
         min_length_queue  = 0;
         time_min_queue    = 0;
    }
    else
    {
         mean_length_queue = ((double)acumul_length_queue/(double)number_of_queue);
         time_mean_queue   = ((double)acumul_time_queue/(double)number_of_queue);
         desvio            = (double)(acumul_length_queue_square/(double)number_of_queue)-(mean_length_queue*mean_length_queue);

         avg_length_queue = mean_length_queue + (5*desvio);
    }

    if (RESULTS)
    {
        printf("\nFrames escritos     \t %d (Frames)\n",  frames_write);
        printf("Numero de deadlines   \t %d (Frames)\n\n", msg_deadline);

        printf("Numero de filas        \t %d (Unid.)\n",    number_of_queue);
        printf("Frames em fila acumul. \t %0.lf (Unid.)\n\n",  acumul_length_queue);

        printf("Tamanho min de fila    \t %d (Frames)\n",     min_length_queue);
        printf("Tamanho medio de filas \t %lf (Frames)\n",    mean_length_queue);
        printf("Tamanho max de fila    \t %d (Frames)\n",     max_length_queue);
        printf("Desvio tamanho de fila \t %lf (Frames)\n",    desvio);
        printf("Media mais k desvios  \t %lf (Frames)\n\n", avg_length_queue);

        printf("Tempo min de fila      \t %lf (ms)\n",   time_min_queue);
        printf("Tempo medio de filas   \t %lf (ms)\n",   time_mean_queue);
        printf("Tempo max de fila      \t %lf (ms)\n\n", time_max_queue);

        printf("Soma de todos os WRCT  \t %lf (ms)\n",   wcrt);
        printf("Media do WCRT          \t %lf (ms)\n\n", mean_wcrt);

        printf("Busload                \t %lf (%)\n",    busload_simulated);
        printf("Tempo de simulação     \t %lf (ms)\n\n", time_current_simulation);
    }
}

void gravaLogFramesCab()
{
    if (!Arq_Log_Best){
        printf("\n================================================================================");
        printf("\n[ERRO] Arquivo '%s' falhou em abrir, função gravaLogFramesCab()\n\n", "LogFrames");
        printf("\n================================================================================\n");
        exit(ERROR_IO);
    }
    fprintf(Arq_Log_Best, "ID\tCYCLE\tTIMEREAL\tTIMESTAMP\tDURATION\tDELAY\n");

}

void gravaLogFrames(event_t event)
{
    fprintf(Arq_Log_Best, "%ld\t%lf\t%lf\t%lf\t%lf\t%lf\n", event.frame.id, event.frame.cycle_time, event.time_current
                                                    , event.time_happen, event.duration, event.time_happen-event.time_current);
}

fifo_t* get_priority_frame()
{

    fifo_t*    event_priority = list_event->first;

    for(fifo_t* aux=list_event->first->next_event; aux; aux=aux->next_event)
    {
        if (aux->event.time_happen > event_priority->event.time_happen)
            break; // supondo que o vetor esteja sempre ordenado.
         if (aux->event.frame.id < event_priority->event.frame.id)
            event_priority = aux;

    }
    return event_priority;
}

void add_time_lost_arbitrage()
{

    for(fifo_t* aux=list_event->first; aux; aux=aux->next_event)
        if (aux->event.time_happen < time_current_simulation)
            aux->event.time_happen = time_current_simulation;
        else
            break; // supondo que o vetor estaja sempre ordenado.
}

void realloc_event(fifo_t* event)
{

    if (event->event.frame.cycle_time < 0)
    {
        rem_list(event);
        return;
    }
    event->event.time_happen  = time_current_simulation  /* - event->event.duration */ + event->event.frame.cycle_time;
    event->event.time_current = event->event.time_happen;
    event_t aux = event->event;
    rem_list(event);
    add_list(aux);
}

void verific_queue()
{
    length_queue = false;
    for(fifo_t* aux=list_event->first; aux; aux=aux->next_event)
        if (aux->event.time_current != aux->event.time_happen)
        {
            length_queue = true;
            break;
        }

    if (length_queue)
    {
        if (!last_queue)
        {
            start_time_queue = time_current_simulation;
            last_queue       = true;
        }

        current_length_queue++;
    }
    else if (last_queue)
    {
        if ((time_current_simulation - start_time_queue) > time_max_queue)
            time_max_queue = time_current_simulation - start_time_queue;

        if ((time_current_simulation - start_time_queue) < time_min_queue)
            time_min_queue = time_current_simulation - start_time_queue;

        if (min_length_queue > current_length_queue)
            min_length_queue = current_length_queue;

        if (max_length_queue < current_length_queue)
            max_length_queue = current_length_queue;


        number_of_queue++;

        acumul_time_queue          += (time_current_simulation - start_time_queue);

        acumul_length_queue        += current_length_queue;
        acumul_length_queue_square += (current_length_queue*current_length_queue);

        current_length_queue        = 0;
        last_queue                  = false;
    }
}

void verific_deadlines()
{

    for(fifo_t* aux=list_event->first; aux; aux=aux->next_event)
       if (aux->event.time_happen >= (aux->event.frame.cycle_time+aux->event.time_current))
       {
          msg_deadline++;
          realloc_event(aux);
       }
}

void verific_wcrt()
{

    for(fifo_t* aux=list_event->first; aux; aux=aux->next_event)
      if (aux->event.frame.wcrt < (aux->event.time_happen - aux->event.time_current))
         aux->event.frame.wcrt = (aux->event.time_happen - aux->event.time_current);
}

void get_wcrt()
{
    wcrt = 0;
    for(fifo_t* aux=list_event->first; aux; aux=aux->next_event)
          wcrt += aux->event.frame.wcrt;
}

double get_mean_wcrt()
{
    double count = 0;

    for(fifo_t* aux=list_event->first; aux; aux=aux->next_event)
        count++;

    return (double)(wcrt/count);
}

void free_recurses()
{

    for(fifo_t* aux=list_event->first; aux; aux=aux->next_event)
       if (aux->prev_event)
          rem_list(aux->prev_event);
    rem_list(list_event->first);
}
