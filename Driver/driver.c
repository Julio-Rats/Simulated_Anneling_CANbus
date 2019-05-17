#include "driver.h"

void start_can_simulated(char* path_arq, double time_simulation){
      char* long_path = (char*)malloc(sizeof(char)*64);
      // strcpy(long_path,"");
      strcpy(long_path,PATH_SA);
      strcat(long_path, path_arq);

      main_simulated(path_arq, time_simulation);
}

double get_wcrt_simulation(){
      return wcrt;
}

double get_busload_simulation(){
      return busload_simulated;
}

double get_max_time_queue_simulation(){
      return time_max_queue;
}
