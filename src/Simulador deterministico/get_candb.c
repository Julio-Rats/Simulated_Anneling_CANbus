#include "get_candb.h"

void input_file(char* path){

		FILE *arch = NULL;
		arch = fopen(path,"r");

		if (!(arch)){
				perror("Archive not found");
				exit(ERROR_IO);
		}

    list_event = init_list();
    frame_t   frame;
    event_t   evento;
		double    id;
		double    cycle_time;
		double    delay_start_time;
		double    deadline_time;
		u_int8_t  payload;
		rewind(arch);

		while (fscanf(arch,"%lf\t%lf\t%lf\t%lf\t%u\n", &id, &cycle_time, &deadline_time,&delay_start_time, &payload) != EOF){
          frame.id               = id;
          frame.cycle_time       = (double)(cycle_time);
          frame.delay_start_time = (double)(delay_start_time);
          frame.deadline_time    = deadline_time;
          frame.payload          = (payload*8)+BITS_FRAMES;
          evento.frame           = frame;
					evento.duration        = get_duration_frame(frame.payload);
          evento.time_current    = frame.delay_start_time;
          evento.time_happen     = frame.delay_start_time;
          add_list(evento);
		}
		fclose(arch);
}


double get_duration_frame(u_int8_t payload){
    return (double)((double)payload/SPEED_BIT);
}
