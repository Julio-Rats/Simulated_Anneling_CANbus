#include "simulador.h"

int main(int argc, char *argv[]) {

  if (argc < 2 || argc > 3){

        printf("\nSimulador CANbus 2.0A\n\n");
        printf("Utilize a sintaxe:\n\n\t%s <Arq> <Time>\n", argv[0]);
        printf("\n   Arq: Arquivo descritivo com os frames (path)\n");
        printf("   Time: Tempo da simulação (ms)\n");
        exit(ERROR_ARGS);

    }

  return main_simulated(argv[1], atof(argv[2]));

}
