#include "main.h"

int main(int argc, char *argv[])
{
    /* Inicializa gerador de numeros aleatorios com semente dada pelo timestamp do sistema */
    srand(time(NULL));

    if (argc == 1)
  	{
  			printf("Fazer o help\n");
  			exit(1);
  	}
  	RESULTS=FALSE;
  	decod_menu(argc, argv);

  	if (SA_ONLY_SIM)
  	{
        if (SA_GRAVA_LOGSIM)
        {
            Arq_Log_Best = fopen(SaArqLogSim, "w+");
            logframes = true;
        }
        if (SA_GRAVA_OBJ)
        {
            Arq_OBJ = fopen(SaArqLogEvolOBJ, "w");
      			if (!Arq_OBJ)
            {
        				printf("\n================================================================================");
        				printf("\n[ERRO] arquivo '%s' não pode ser criado em main()\n\n", SaArqLogEvolOBJ);
        				printf("\n================================================================================\n\n");
        				exit(SA_ERRO_IO);
      			}
        }
        
        #if SA_TIMER
        		/* Contabiliza tempo de execucao */
        		TmrAlocaTemporizador(SA_TIMER_SIMULATED, TRUE, "Principal SA");
        		TmrStart(SA_TIMER_SIMULATED);
      	#endif

  			main_simulated(SaSimFile, TIME_CAN_SIMULATED);

        #if SA_TIMER
        		/* Contabiliza tempo de execucao */
        		TmrStop(SA_TIMER_SIMULATED);
        		printf("\n[INFO] Runtime Simulation Only (s): %f\n\n", TmrObtemTemporizador(SA_TIMER_SIMULATED));
      	#endif

  			if (SA_GRAVA_WCRT)
  				 print_wcrt(SaWCRTFile);
  			exit(0);
  	}

    if (SA_HAVE_CONF)
    	 /* Carrega o conteudo do arquivo de configuracao informado */
  	   SaAbreArquivoConfiguracao(SaArqConfiguracao);

  	get_candb(SaCANDBFile);

  	if (SA_GRAVA_OBJ)
  	{
  			Arq_OBJ = fopen(SaArqLogEvolOBJ, "w");
  			if (!Arq_OBJ)
        {
    				printf("\n================================================================================");
    				printf("\n[ERRO] arquivo '%s' não pode ser criado em main()\n\n", SaArqLogEvolOBJ);
    				printf("\n================================================================================\n\n");
    				exit(SA_ERRO_IO);
  			}
  	}

  	/* Exibe os parametros do método */
  	printf("\n[INFO] Simulated Annealing Parameters:\n\n");
  	printf("# of Neighbors      : %u\n",  SaNumIteracao);
  	printf("# of reheating      : %d\n",  SaNumReaquecimento);
  	printf("# of Perturbations  : %ld\n", SaNumSlotsPerturbacao);
  	printf("Initial Temperature : %lf\n", SaTempInicial);
  	printf("Final Temperature   : %lf\n", SaTempFinal);
  	printf("Cooling Factor      : %lf\n", SaAlpha);
  	printf("\n\n");

    main_simulated_annealing(argc, argv);

    return 0;
}

/*****************************************************************************************/
/*                                                                                       */
/* Funcao para decodificação dos argumentos do menu                                       */
/*                                                                                       */
/*****************************************************************************************/
void decod_menu(u_int8_t argc, char *argv[])
{
		char opt;
    bool dbc = TRUE;
		while((opt = getopt(argc, argv, ":p:c:d:g:r:s:t:l:v:w:")) != -1)
    {
        switch(opt)
        {
            case 'p': // LOG_SIM
                strcpy(SaArqLogSim, optarg);
								SA_GRAVA_LOGSIM = TRUE;
                break;
            case 'c': // CONFIG
                SA_HAVE_CONF = TRUE;
                strcpy(SaArqConfiguracao, optarg);
                break;
						case 'd': // CANDB
                dbc = FALSE;
								strcpy(SaCANDBFile, optarg);
								break;
						case 'g': // Log FRAMES
	              strcpy(SaArqLogEvolOBJ, optarg);
								SA_GRAVA_OBJ = TRUE;
	              break;
						case 'r': // Result
                SA_RESULT = FALSE;
	              strcpy(SaArqSaida, optarg);
	              break;
						case 'v': // verbose prob
                if ((strcmp(optarg,"v"))==0)
                    SA_VERBOSE_FULL = TRUE;
                else
                {
                    printf("Opção invalida para parametro -v\n\nExperimente usar -vv");
                    exit(1);
                }
	              break;
						case 't': // Time
								TIME_CAN_SIMULATED = atof(optarg);
	              break;
						case 's': // Simulation
								strcpy(SaSimFile, optarg);
								RESULTS 		= TRUE;
								SA_ONLY_SIM = TRUE;
	              break;
						case 'l': // Limit delay
								LIMIT_DELAY 		= atof(optarg);
	              SET_LIMIT_DELAY = TRUE;
	              break;
						case 'w': // print wcrt cada ID
								strcpy(SaWCRTFile, optarg);
	              SA_GRAVA_WCRT  = TRUE;
	              break;
            case ':':
                if (optopt == 'v')
                {
                    SA_VERBOSE = TRUE;
                    break;
                }
                printf("Opção necessita de um valor: %c\n", optopt);
								exit(1);
                break; // kkk melhor break
            case '?':
                printf("Opção invalida: %c\n", optopt);
								exit(1);
                break; // kkk melhor break
        }
    }
    if ((!SA_ONLY_SIM) && dbc)
    {
          printf("\nArquivo dbc tabular não informado para otimização, forneça utilizando -d [arquvio] \n\n");
          exit(1);
    }
}
