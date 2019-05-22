#include "driver.h"

void start_can_simulated(char* path_arq, double time_simulation){
      char* long_path = (char*)malloc(sizeof(char)*LEN_STR);
      if(!long_path){
          printf("\n================================================================================");
      		printf("\n[ERRO] Erro malloc() variavel '%s' falha ao tentar alocar %d bytes, função start_can_simulated()\n\n", "long_path", sizeof(char)*LEN_STR);
      		printf("\n================================================================================\n");
      		exit(DR_ERRO_MEMORY);
      }
      strcpy(long_path,PATH_SA);
      strcat(long_path, path_arq);

      main_simulated(path_arq, time_simulation);
}
