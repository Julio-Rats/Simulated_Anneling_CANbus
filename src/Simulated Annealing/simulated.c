/*****************************************************************************************/
/*                                                                                       */
/* simulated.c                                                                           */
/*                                                                                       */
/* Implementa o método Simulated Annealing para ajuste de temporização de ciclo e delay  */
/* em arquivos ODBC do CANOe                                                             */
/*                                                                                       */
/*****************************************************************************************/
#include "simulated.h"

/*****************************************************************************************/
/*                                                                                       */
/*                         Variaveis Locais Globais                                      */
/*                                                                                       */
/*****************************************************************************************/
/* Numero de mensagens consideradas pelo arquivo de configuracao */
u_int16_t   SaNumMsgCan    				 = 0L;

/* Array de parametros de mensagens */
StSaMsgTmrSlot* pSaMsgParArray 		 = NULL;

/* Solucoes usadas durante o processamento do Simulated Annealing */
StSaSolucao* pSaMelhor             = NULL;
StSaSolucao* pSaCorrente           = NULL;
StSaSolucao* pSaVizinho            = NULL;
StSaSolucao* pSaMelhorVizinho      = NULL;

/* Incrementadores para geração de multi arquivos */
static u_int32_t log_frame          = 1;
u_int32_t        cont_wcrt          = 1;

/* Controla posicoes sorteadas pelo SA */
u_int8_t* pSaBitPosicao;

/*****************************************************************************************/
/*                                                                                       */
/*      valores 'default' para os parametros do metodo Simulated Annealing               */
/*                          Variaveis EXTERN                                             */
/*                                                                                       */
/*****************************************************************************************/
double       SaTempInicial         = 5000.0;
double       SaTempFinal           = 10.0;
double       SaAlpha               = 0.95;
u_int16_t    SaNumSlotsPerturbacao = 1;
u_int16_t    SaNumIteracao         = 50;
u_int8_t     SaNumReaquecimento    = 1;
u_int8_t     SaMetodoBusca         = SA_CNF_SELECAO_UNIFORME;
u_int8_t     SaMetodoPert          = SA_CNF_PERT_INCRIMENT;
u_int8_t     SaMetodoInicial       = SA_CNF_START_ZERO;
double       TIME_CAN_SIMULATED    = 120000;
bool         SA_VERBOSE            = FALSE;
bool         SA_VERBOSE_FULL       = FALSE;
bool         SA_ONLY_SIM           = FALSE;
bool         SA_GRAVA_LOGSIM 	     = FALSE;
bool         SA_GRAVA_OBJ  	       = FALSE;
bool         SA_GRAVA_WCRT 	       = FALSE;
bool         SET_LIMIT_DELAY       = TRUE;
bool         SA_HAVE_CONF          = FALSE;
bool         SA_RESULT             = TRUE;
double       LIMIT_DELAY           = 20;

/* Strings com os nomes dos arquivos informados via linha de comando */
char SaArqConfiguracao[SA_MAX_CHAR_COMMAND_LINE];
char SaArqLogEvolOBJ[SA_MAX_CHAR_COMMAND_LINE];
char path_file_best[SA_MAX_CHAR_COMMAND_LINE+5];
char path_file_wcrt[SA_MAX_CHAR_COMMAND_LINE+5];
char SaArqTempos[SA_MAX_CHAR_COMMAND_LINE] = "/tmp/sa_time.txt";
char SaCANDBFile[SA_MAX_CHAR_COMMAND_LINE];
char SaArqSaida[SA_MAX_CHAR_COMMAND_LINE];
char SaWCRTFile[SA_MAX_CHAR_COMMAND_LINE];
char SaArqLogSim[SA_MAX_CHAR_COMMAND_LINE];
char SaSimFile[SA_MAX_CHAR_COMMAND_LINE];

/*Descritores de arquvos, para os logs, modos verbose*/
FILE*    	   Arq_OBJ                = NULL;
FILE*    	   Arq_Best               = NULL;
FILE*    	   Arq_WCRT               = NULL;

void start_can_simulated(char* path_arq, double time_simulation)
{
      char* long_path = (char*)malloc(sizeof(char)*128);
      if(!long_path){
          printf("\n================================================================================");
      		printf("\n[ERRO] Erro malloc() variavel '%s' falha ao tentar alocar %d bytes, função start_can_simulated()\n\n", "long_path", sizeof(char)*128);
      		printf("\n================================================================================\n");
      		exit(SA_ERRO_MEMORIA);
      }
      strcpy(long_path,PATH_SA);
      strcat(long_path, path_arq);

      main_simulated(path_arq, time_simulation);
}

/*****************************************************************************************/
/*                                                                                       */
/* Carrega um arquivo de configuracao informado e armazena os parametros de cada msg CAN */
/*                                                                                       */
/*****************************************************************************************/
void SaAbreArquivoConfiguracao(char* Nome)
{
	FILE *pArq;
	char Controle;
	char Lixo;
	char Sair = FALSE;
	u_int8_t  Linha = 0;
	u_int8_t  IdxCtrlEntrada = 0;
	u_int16_t i;

	u_int16_t Id;
	double    MaxCiclo;
	char      NomeMsg[SA_MAX_CHAR_NOME_CAN_MESG];

	/* Abre o arquivo com o nome informado */
	pArq = fopen(Nome, "r");

	/* Se arquivo nao existe, exibir mensagem de erro */
	if(pArq == NULL)
	{
		printf("\n================================================================================");
		printf("\n[ERRO] Arquivo '%s' nao existe\n\n", Nome);
		printf("\n================================================================================\n\n");
		exit(SA_ERRO_IO);
	}

	/* Carrega os dados do arquivo de acordo com o caracter de controle */
	while(!Sair && !feof(pArq))
	{
		/* Obtem o caracter de controle */
		fscanf(pArq, "%c", &Controle);

		/* Incrementa o numero da linha */
		Linha++;

		/* Processa de acordo com o caracter de controle */
		switch(Controle)
		{
			/* Caso seja lido um comentario */
			case '#':
			{
				/* Ignora o restante da linha */
				do
				{
					fscanf(pArq, "%c", &Lixo);
				}
				while(Lixo != '\n');

				break;
			}

			/* Caso seja lido um ENTER */
			case '\n':
			{
				/* Ignora a linha. Usado apenas para organizar melhor o arquivo de config. */
				break;
			}

			/* Caso seja lido o numero total de mensagens */
			// case 'n':
			// {
			// 	/* Le a quantidade de mensagens esperada no arquivo */
			// 	fscanf(pArq, "%hd", &SaNumMsgCan);
			//
			// 	/* Ignora restante da linha */
			// 	do
			// 	{
			// 		fscanf(pArq, "%c", &Lixo);
			// 	}
			// 	while(Lixo != '\n');
			//
			// 	/* Inicializa a estrutura de dados correspondente aos parametros das */
			// 	/* mensagens CAN */
			// 	pSaMsgParArray = (StSaMsgPar*) malloc(sizeof(StSaMsgPar) * SaNumMsgCan);
			//
			// 	/* Caso a alocacao nao tenha ocorrido com exito, avisar */
			// 	if(pSaMsgParArray == NULL)
			// 	{
			// 		printf("\n================================================================================");
			// 		printf("\n[ERRO] falha na alocacao de memoria em SaAbreArquivoConfiguracao()\n\n");
			// 		printf("\n================================================================================\n");
			// 		exit(SA_ERRO_MEMORIA);
			// 	}
			//
			// 	/* Inicializa vetor de controle de posicoes sorteadas */
			// 	pSaBitPosicao = (unsigned char *) malloc(sizeof(unsigned char) * SaNumMsgCan);
			//
			// 	/* Caso a alocacao nao tenha ocorrido com exito */
			// 	if(pSaBitPosicao == NULL)
			// 	{
			// 		printf("\n================================================================================");
			// 		printf("\n[ERRO] falha na alocacao de memoria em SaAbreArquivoConfiguracao()\n\n");
			// 		printf("\n================================================================================\n");
			// 		exit(SA_ERRO_MEMORIA);
			// 	}
			//
			// 	/* Inicializa cada slot do array de controle de parametros com valores default */
			// 	for(i = 0; i < SaNumMsgCan; i++)
			// 	{
			// 		/* Copia ID e tempo maximo do ciclo */
			// 		pSaMsgParArray[i].Id = SA_ID_NONE;
			// 		pSaMsgParArray[i].MaxCiclo = DBL_MAX;
			//
			// 		/* Aloca memoria para o nome da mensagem */
			// 		pSaMsgParArray[i].pNome = (char *) malloc(sizeof(char) * SA_MAX_CHAR_NOME_CAN_MESG);
			// 		if(pSaMsgParArray[i].pNome == NULL)
			// 		{
			// 			printf("\n================================================================================");
			// 			printf("\n[ERRO] falha na alocacao de memoria em SaAbreArquivoConfiguracao()\n\n");
			// 			printf("\n================================================================================\n");
			// 			exit(SA_ERRO_MEMORIA);
			// 		}
			//
			// 		/* Copia o nome da mensagem */
			// 		strcpy(pSaMsgParArray[i].pNome, "NO NAME");
			// 	}
			//
			// 	break;
			// }
			//
			// /* Caso seja lido uma entrada de configuracao de mensagem */
			// case 'm':
			// {
			// 	/* Verifica se existe espaco para uma nova entrada no array de parametros */
			// 	if(IdxCtrlEntrada < SaNumMsgCan)
			// 	{
			// 		/* Obtem os dados do arquivo correspondente aos parametros da */
			// 		/* mensagem em questao */
			// 		fscanf(pArq, "%hd", &Id);
			// 		fscanf(pArq, "%lf", &MaxCiclo);
			// 		fgets(NomeMsg, SA_MAX_CHAR_NOME_CAN_MESG, pArq);
			//
			// 		/* Preenche os dados obtidos do arquivo */
			// 		pSaMsgParArray[IdxCtrlEntrada].Id = Id;
			// 		pSaMsgParArray[IdxCtrlEntrada].MaxCiclo = MaxCiclo;
			// 		strcpy(pSaMsgParArray[IdxCtrlEntrada].pNome, NomeMsg);
			//
			// 		/* Atualiza o contador de slots preenchidos */
			// 		IdxCtrlEntrada++;
			// 	}
			// 	else
			// 	{
			// 		printf("\n================================================================================");
			// 		printf("\n[ERRO] Existem mais mensagens no arquivo de configuracao do que o que foi especificado\n\n");
			// 		printf("\n================================================================================\n");
			// 		exit(SA_ERRO_PARSING_CONFIG);
			// 	}
			//
			// 	break;
			// }

			/* Marcador correspondente ao 'End of File', nada mais será lido a seguir */
			case 'e':
			{
				/* Marca flag de termino de leitura do arquivo */
				Sair = TRUE;

				break;
			}

			/* Marcador de temperatura inicial do SA */
			case 's':
			{
				/* Le a temperatura inicial do metodo */
				fscanf(pArq, "%lf", &SaTempInicial);

				/* Ignora restante da linha */
				do
				{
					fscanf(pArq, "%c", &Lixo);
				}
				while(Lixo != '\n');

				break;
			}

			/* Marcador de temperatura final do SA */
			case 'f':
			{
				/* Le a temperatura final */
				fscanf(pArq, "%lf", &SaTempFinal);

				/* Ignora restante da linha */
				do
				{
					fscanf(pArq, "%c", &Lixo);
				}
				while(Lixo != '\n');

				break;
			}

			/* Marcador de fator de resfriamento Alpha do SA */
			case 'a':
			{
				/* Le a quantidade de vizinhos do arquivo */
				fscanf(pArq, "%lf", &SaAlpha);

				/* Ignora restante da linha */
				do
				{
					fscanf(pArq, "%c", &Lixo);
				}
				while(Lixo != '\n');

				break;
			}

			/* Marcador de numero de perturbacoes nos slots de tempo do SA */
			case 'p':
			{
				/* Le a quantidade de perturbacoes a aplicar */
				fscanf(pArq, "%hd", &SaNumSlotsPerturbacao);

				/* Ignora restante da linha */
				do
				{
					fscanf(pArq, "%c", &Lixo);
				}
				while(Lixo != '\n');

				break;
			}
			//(Julio)
			/* Numero de iterações antes de um decaimento de temperatura (default == 1)*/
			case 'i':
				{
					/* Pega o numero do iterador */
					fscanf(pArq, "%u", &SaNumIteracao);

					/* Ignora restante da linha */
					do
					{
						fscanf(pArq, "%c", &Lixo);
					}
					while(Lixo != '\n');

					break;
				}
			/*Pega numero de reaquecimentos (default == 0)*/
			case 'r':
				{
					/* Pega o numero de reaquecimento */
					fscanf(pArq, "%hhu", &SaNumReaquecimento);

					/* Ignora restante da linha */
					do
					{
						fscanf(pArq, "%c", &Lixo);
					}
					while(Lixo != '\n');

					break;
				}
				/*Metodo do vizinho inicial */
				case 'c':
				{
						char solucao_start;
						fscanf(pArq, "%c", &Lixo);
						fscanf(pArq, "%c", &solucao_start);

						switch (solucao_start) {
								case 'a':
								{
										SaMetodoInicial = SA_CNF_START_RANDOM;
										break;
								}
								case 'z':
								{
										SaMetodoInicial = SA_CNF_START_ZERO;
										break;
								}
								default:
								{
									printf("\n================================================================================");
									printf("\n[ERRO] Entrada para vizinho inicial desconhecida '%c' na linha %d do arquivo '%s'\n\n", solucao_start, Linha, Nome);
									printf("\n================================================================================\n\n");
									exit(SA_ERRO_PARSING_CONFIG);
									break;
								}
						}
						/* Ignora restante da linha */
						do
						{
							fscanf(pArq, "%c", &Lixo);
						}
						while(Lixo != '\n');

						break;
				}
				/*Metodo de busca vizinhanca*/
				case 'b':
					{
						char busca;
						char pertub;

						fscanf(pArq, "%c", &Lixo);
						fscanf(pArq, "%c", &busca);
						fscanf(pArq, "%c", &Lixo);
						fscanf(pArq, "%c", &pertub);

						switch (busca)
						{
							case 'u':
							{
								SaMetodoBusca = SA_CNF_SELECAO_UNIFORME;
								break;
							}
							case 'i':
							{
								SaMetodoBusca = SA_CNF_SELECAO_PROPORCIONAL_ID;
								break;
							}
							default:
							{
								printf("\n================================================================================");
								printf("\n[ERRO] Entrada para busca de vizinhança desconhecida '%c' na linha %d do arquivo '%s'\n\n", busca, Linha, Nome);
								printf("\n================================================================================\n\n");
								exit(SA_ERRO_PARSING_CONFIG);
								break;
							}
						}
						switch (pertub)
						{
							case 'r':
							{
								SaMetodoPert = SA_CNF_PERT_RANDOM;
								break;
							}
							case 'i':
							{
								SaMetodoPert = SA_CNF_PERT_INCRIMENT;
								break;
							}
							case 'c':
							{
								SaMetodoPert = SA_CNF_PERT_RAND_DELAY;
								break;
							}
							default:
							{
								printf("\n================================================================================");
								printf("\n[ERRO] Entrada para pertubação de vizinhança desconhecida '%c' na linha %d do arquivo '%s'\n\n", pertub, Linha, Nome);
								printf("\n================================================================================\n");
								exit(SA_ERRO_PARSING_CONFIG);
								break;
							}
						}
						/* Ignora restante da linha */
						do
						{
							fscanf(pArq, "%c", &Lixo);
						}
						while(Lixo != '\n');

						break;
					}
			//
			/* Caso outro caracter seja lido, informar o erro */
			default:
			{
				printf("\n================================================================================");
				printf("\n[ERRO] Caracter de controle '%c' nao identificado na linha %d do arquivo '%s'\n\n", Controle, Linha, Nome);
				printf("\n================================================================================\n\n");
				exit(SA_ERRO_PARSING_CONFIG);

				break;
			}
		}
	}

	/* Fecha o arquivo */
	fclose(pArq);
}


/*****************************************************************************************/
/*                                                                                       */
/* Imprime WCRT de cada ID                                                */
/*                                                                                       */
/*****************************************************************************************/
void print_wcrt(char* arch_name)
{
		FILE *arch = NULL;
		arch = fopen(arch_name,"w");

		if (!(arch))
		{
				printf("\n================================================================================");
				printf("\n[ERRO] Ao abrir o arquivo \"%s\"\n\n", arch_name);
				printf("\n================================================================================\n\n");
				exit(ERROR_IO);
		}

		fprintf(arch, "ID\tWCRT\n");
		for(fifo_t* aux=list_event->first; aux; aux=aux->next_event)
		{
				fprintf(arch, "%d\t%lf\n", aux->event.frame.id, aux->event.frame.wcrt);
		}
		fclose(arch);
}

/*****************************************************************************************/
/*                                                                                       */
/* Lê o CANDB passado para otimização.                                                */
/*                                                                                       */
/*****************************************************************************************/
void get_candb(char* arch_name)
{
		FILE *arch = NULL;
		arch = fopen(arch_name,"r");

		if (!(arch))
		{
				printf("\n================================================================================");
				printf("\n[ERRO] Ao abrir o arquivo \"%s\"\n\n", arch_name);
				printf("\n================================================================================\n\n");
				exit(ERROR_IO);
		}

		event_t         evento;
		double          id;
		double          cycle_time;
		double          delay_start_time;
		double          deadline_time;
		u_int8_t        payload;

		SaNumMsgCan	= 0;
		rewind(arch);

		while (fscanf(arch,"%lf\t%lf\t%lf\t%lf\t%u\n", &id, &cycle_time, &deadline_time,&delay_start_time, &payload) != EOF)
		{
					if (pSaMsgParArray)
					{
							pSaMsgParArray = (StSaMsgTmrSlot*)realloc(pSaMsgParArray,sizeof(StSaMsgTmrSlot)*(++SaNumMsgCan));
							pSaMsgParArray[SaNumMsgCan-1].Id               = id;
							pSaMsgParArray[SaNumMsgCan-1].TamCiclo         = (double)(cycle_time);
							pSaMsgParArray[SaNumMsgCan-1].StartDelay       = (double)(delay_start_time);
							pSaMsgParArray[SaNumMsgCan-1].deadline_time    = deadline_time;
							pSaMsgParArray[SaNumMsgCan-1].payload          = payload;
					}
					else
					{
							pSaMsgParArray = (StSaMsgTmrSlot*)malloc(sizeof(StSaMsgTmrSlot)*(++SaNumMsgCan));
							pSaMsgParArray[SaNumMsgCan-1].Id               = id;
							pSaMsgParArray[SaNumMsgCan-1].TamCiclo         = (double)(cycle_time);
							pSaMsgParArray[SaNumMsgCan-1].StartDelay       = (double)(delay_start_time);
							pSaMsgParArray[SaNumMsgCan-1].deadline_time    = deadline_time;
							pSaMsgParArray[SaNumMsgCan-1].payload          = payload;
					}
		}
		/* Inicializa vetor de controle de posicoes sorteadas */
		pSaBitPosicao = (unsigned char *) malloc(sizeof(unsigned char) * SaNumMsgCan);

		/* Caso a alocacao nao tenha ocorrido com exito */
		if(pSaBitPosicao == NULL)
		{
				printf("\n================================================================================");
				printf("\n[ERRO] falha na alocacao de memoria em SaAbreArquivoConfiguracao()\n\n");
				printf("\n================================================================================\n");
				exit(SA_ERRO_MEMORIA);
		}
		fclose(arch);
}


/*****************************************************************************************/
/*                                                                                       */
/* Imprime os parametros lidos do arquivo                                                */
/*                                                                                       */
/*****************************************************************************************/
void SaDbgPrintParametros(void)
{
	u_int16_t i;

	printf("\n================================================================================\n");
	printf("[INFO] Input Parameters (%d):\n", SaNumMsgCan);
	printf("================================================================================\n\n");
	for(i = 0; i < SaNumMsgCan; i++)
	{
		printf("\nSlot # %d\n", i);
		printf("Id    : %ld\n", pSaMsgParArray[i].Id);
		printf("Cycle : %lf\n", pSaMsgParArray[i].TamCiclo);
	}
	printf("\n\n");
}

/*****************************************************************************************/
/*                                                                                       */
/* Aloca memoria e inicializa parte dos parametros de uma solucao para o problema        */
/*                                                                                       */
/*****************************************************************************************/
StSaSolucao* SaAlocaSolucao(void)
{
	u_int16_t     i;
	StSaSolucao* pSolucao;
	double       SomaID;

	/* Aloca memoria para a estrutura de dados da solucao */
	pSolucao = (StSaSolucao *) malloc(sizeof(StSaSolucao));

	/* Caso a memoria tenha sido alocada com sucesso, prossegue */
	if(pSolucao == NULL)
	{
		printf("\n================================================================================");
		printf("\n[ERRO] falha na alocacao de memoria em SaAlocaSolucao()\n\n");
		printf("\n================================================================================\n");
		exit(SA_ERRO_MEMORIA);
	}

	/* Inicializa solucao */
	pSolucao->WCRT       = 0;
	pSolucao->burst_time = 0;
	pSolucao->burst_size = 0;
	pSolucao->pSol       = (StSaMsgTmrSlot *) malloc(sizeof(StSaMsgTmrSlot) * SaNumMsgCan);

	if(pSolucao->pSol == NULL)
	{
		printf("\n================================================================================");
		printf("\n[ERRO] falha na alocacao de memoria em SaAlocaSolucao()\n\n");
		printf("\n================================================================================\n");
		exit(SA_ERRO_MEMORIA);
	}

	/* Calcula o somatorio dos IDs para calculo de proporcionalidade inversa ao ID (selecao de mensagens no SA) */
	SomaID = 0L;
	for(i = 0; i < SaNumMsgCan; i++)
	{
		/* Acumula os IDs */
		SomaID = SomaID + (1.0 / (pow(pSaMsgParArray[i].Id, 0.5)));
	}
	/* Aloca e inicializa os slots de proposta de parametros para uma solucao */
	for(i = 0; i < SaNumMsgCan; i++)
	{
		pSolucao->pSol[i].Id            = pSaMsgParArray[i].Id;
		pSolucao->pSol[i].payload       = pSaMsgParArray[i].payload;
		pSolucao->pSol[i].TamCiclo      = pSaMsgParArray[i].TamCiclo;
		pSolucao->pSol[i].StartDelay    = pSaMsgParArray[i].StartDelay;
		pSolucao->pSol[i].deadline_time = pSaMsgParArray[i].deadline_time;
		pSolucao->pSol[i].ProbSelecao   = (1.0 / pow(pSaMsgParArray[i].Id,   0.5)) / SomaID;
	}

	/* Devolve o endereco onde a solucao foi alocada */
	return(pSolucao);
}

/*****************************************************************************************/
/*                                                                                       */
/* Desaloca memoria para uma solucao do problema                                         */
/*                                                                                       */
/*****************************************************************************************/
void SaDesalocaSolucao(StSaSolucao *pSolucao)
{
	/* Libera a memoria do vetor com propostas de temporizacao da solucao atual */
	if(pSolucao->pSol != NULL)
	{
		free(pSolucao->pSol);
		pSolucao->pSol = NULL;
	}

	/* Libera memoria da solucao atual */
	if(pSolucao != NULL)
	{
		free(pSolucao);
		pSolucao = NULL;
	}
}

/*****************************************************************************************/
/*                                                                                       */
/* Clona uma solucao a partir de uma solucao base informada                              */
/*                                                                                       */
/*****************************************************************************************/
void SaClonaSolucao(StSaSolucao* pCopia, StSaSolucao* pBase)
{
	u_int8_t i;

	/* Copia os parametros diretos */
	pCopia->WCRT       = pBase->WCRT;
	// pCopia->NumMsg     = pBase->NumMsg;
	pCopia->burst_time = pBase->burst_time;
	// pCopia->NumMsg     = pBase->NumMsg;
	pCopia->burst_size = pBase->burst_size;

	/* Copia os slots da solucao base para a solucao clonada */
	for(i = 0; i <SaNumMsgCan; i++)
	{
		/* Copia dados dos parametros, quando pertinente */
		pCopia->pSol[i].Id            = pBase->pSol[i].Id;
		pCopia->pSol[i].TamCiclo      = pBase->pSol[i].TamCiclo;
		pCopia->pSol[i].StartDelay    = pBase->pSol[i].StartDelay;
		pCopia->pSol[i].deadline_time = pBase->pSol[i].deadline_time;
		pCopia->pSol[i].payload       = pBase->pSol[i].payload;
	}
}

/*****************************************************************************************/
/*                                                                                       */
/* Cria uma solucao aleatoria para inicio da operacao do Simulated Annealing             */
/*                                                                                       */
/*****************************************************************************************/
void SaCriaSolucaoAleatoria(StSaSolucao* pSolucao)
{
	u_int16_t  i;
	/* Define aleatoriamente uma alocacao de TamCiclo = (Ciclo + Delay) */
	for(i = 0; i < SaNumMsgCan; i++)
	{
		pSolucao->pSol[i].StartDelay = (double) (rand() % (int)(pSolucao->pSol[i].TamCiclo*PORC_START_DELAY));
	}
}

/*****************************************************************************************/
/*                                                                                       */
/* Exibe a solucao informada                                                             */
/*                                                                                       */
/*****************************************************************************************/
void SaDbgExibeSolucao(StSaSolucao* pSolucao, char* pNome)
{
	u_int16_t i;

	/* Exibe os dados da solucao */
	printf("\n================================================================================");
	printf("\n[INFO] Solution '%s'. WCRT(%%) = %lf\t burst_time = %lf\n", pNome, pSolucao->WCRT, pSolucao->burst_time);
	printf("================================================================================\n");


	if (SA_VERBOSE_FULL)
	{
			printf("\n[DEBUG] Solution Timers:\n\n");
			for(i = 0; i < SaNumMsgCan; i++)
			{
				printf("Message Id        : %ld\n",      pSolucao->pSol[i].Id);
				printf("Message Cycle Time: %.0lf\n",    pSolucao->pSol[i].TamCiclo);
				printf("Message StartDelay: %.0lf\n",    pSolucao->pSol[i].StartDelay);
			}
	}

}

/*****************************************************************************************/
/*                                                                                       */
/* Seleciona as mensagens em proporcionalidade com o ID escolhido                        */
/*                                                                                       */
/*****************************************************************************************/
u_int16_t SaSelecionaSlotProporcionalAoID(StSaSolucao* pSolucao)
{
	u_int16_t Posicao   = 0L;
	double    Acumulado = 0.0;
	double    PercSelecionada;
	u_int16_t i;

	/* Escolhe um cursor com viés de U(0,1) */
	PercSelecionada = ((double) rand() / RAND_MAX);

	/* Calcula o valor acumulado ate que encontre o slot */
	for(Posicao = 0; Posicao < SaNumMsgCan; Posicao++)
	{
		/* Verifica se encontramos o slot */
		if((PercSelecionada >= Acumulado) && (PercSelecionada < Acumulado + pSolucao->pSol[Posicao].ProbSelecao))
		{
			break;
		}

		/* Incrementa e acumula */
		Acumulado = Acumulado + pSolucao->pSol[Posicao].ProbSelecao;
	}

	/* Retorna a posicao selecionada */
	return(Posicao);
}
/*****************************************************************************************/
/*                                                                                       */
/* Seleciona mensagem de acordo com a mesma probabilidade                                */
/*                                                                                       */
/*****************************************************************************************/
u_int16_t SaSelecionaSlotUniforme(StSaSolucao* pSolucao)
{
	return(rand() % SaNumMsgCan);
}

/*****************************************************************************************/
/*                                                                                       */
/* Realiza perturbacoes na solucao informada, alterando a temporizacao das mensagens     */
/*                                                                                       */
/*****************************************************************************************/

void SaPerturbaSolucaoVizinhancaUniforme(StSaSolucao* pSolucao)
{
	u_int16_t PosTroca;
	double    Tempo1;
	double    Tempo2;
	double    Fator;
	u_int16_t i;
	u_int8_t  primeIndex;

	/* Reseta o vetor de controle de posicoes sorteadas */
	for(i = 0; i < SaNumMsgCan; i++)
	{
		pSaBitPosicao[i] = FALSE;
	}
	/* Realiza o numero de trocas propostas para as mensagens CANs na vizinhanca da solucao */
	for(i = 1; i <= SaNumSlotsPerturbacao; i++)
	{
		/* Sorteia enquanto nao encontrar posicao diferente */
		do
		{
				/* Sorteia qual posicao da solucao ofertada sofrera' troca de valores */
			switch(SaMetodoBusca)
				{
						/* Mensagens sao selecionadas de maneira uniforme, com mesma probabilidade */
						case SA_CNF_SELECAO_UNIFORME:
						{
							PosTroca = SaSelecionaSlotUniforme(pSolucao);
							break;
						}

						/* Mensagens sao selecionadas de maneira proporcional ao seu ID */
						case SA_CNF_SELECAO_PROPORCIONAL_ID:
						{
							PosTroca = SaSelecionaSlotProporcionalAoID(pSolucao);
							break;
						}
				}
		}while(pSaBitPosicao[PosTroca] != FALSE);

		/* Seta o controle de sorteio */
		pSaBitPosicao[PosTroca] = TRUE;

		if (SA_VERBOSE_FULL)
			printf("\n\t\t\t[DEBUG] PosTroca @ %d: %d", i, PosTroca);


		/************************************************************************/
		/* EVERTHON: testando a alternativa de atribuir valores primos ao ciclo */

		// aq primeIndex = getIndexOfPrimeLesserThan( (u_int8_t) pSolucao->pSol[PosTroca].TamCiclo );

		//sorteia dentre os 3 primos menores que TamCiclo
		// aq pSolucao->pSol[PosTroca].Ciclo = (double) primes[ rand() % primeIndex + ( (primeIndex>=3) ? (primeIndex-3) : (0) ) ];
		//BUGFIX
		// aq if ( (pSolucao->pSol[PosTroca].Ciclo > pSolucao->pSol[PosTroca].TamCiclo) || (pSolucao->pSol[PosTroca].Ciclo <= 0) )
		// aq	pSolucao->pSol[PosTroca].Ciclo = primes[primeIndex];

			//TODO parametrizar isso
		//sorteia um STertDelay entre os primos menores o ciclo máximo dentre as mensagens
		// aq pSolucao->pSol[PosTroca].StartDelay = (double) primes[ getIndexOfPrimeLesserThan(50) + rand() % getIndexOfPrimeLesserThan(1000) ];

		//pSolucao->pSol[PosTroca].Delay = (double) primes[ rand() % primeIndex/2 ];

		/************************************************************************/

		/* EVERTHON: desabilitei estes testes que fizemos na última vez */
		// #if 0
		// /* Modifica o valor de tempo de ciclo e offset para o slot sorteado */
		// pSolucao->pSol[PosTroca].Ciclo = round(((double) rand() / RAND_MAX) * pSolucao->pSol[PosTroca].TamCiclo);
		// pSolucao->pSol[PosTroca].Delay = pSolucao->pSol[PosTroca].TamCiclo - pSolucao->pSol[PosTroca].Ciclo;
		// #endif

		// do
		// {
			// /* Reduz o ciclo total nominal em até 7% do valor */
			// Fator = (1.0 - 0.07 * ((double) rand() / RAND_MAX));
			/* Fator = (1.0 + 0.07 * ((double) rand() / RAND_MAX)); */
			/* Fator = 1.0; */

			/* XXX ideia: sortear o fator entre de -7% a +7% (1.0 +- 0.07) */

			// /* Modifica o valor de tempo de ciclo e offset para o slot sorteado */
			// Tempo1 = round(((double) rand() / RAND_MAX) * (Fator * pSolucao->pSol[PosTroca].TamCiclo));
			// Tempo2 = (Fator * pSolucao->pSol[PosTroca].TamCiclo) - Tempo1;

			// /* Atribui o maior tempo ao ciclo, priorizando este atributo */
			// pSolucao->pSol[PosTroca].Ciclo = ((Tempo1 > Tempo2) ? Tempo1 : Tempo2);
			// pSolucao->pSol[PosTroca].Delay = ((Tempo1 > Tempo2) ? Tempo2 : Tempo1);
			/* pSolucao->pSol[PosTroca].Delay = 0.0; */
		// }
		// while(pSolucao->pSol[PosTroca].Ciclo == 0.0);
		//(JUlIO)
			#define MIN_RAND     -5
			#define MAX_RAND      5
			switch (SaMetodoPert)
			{
				case SA_CNF_PERT_INCRIMENT:
			   	{
							pSolucao->pSol[PosTroca].StartDelay += (double) ((rand()%(MAX_RAND-MIN_RAND+1))+MIN_RAND);
							if (pSolucao->pSol[PosTroca].StartDelay < 0){
									pSolucao->pSol[PosTroca].StartDelay = 0;
							}
							if (pSolucao->pSol[PosTroca].StartDelay > pSolucao->pSol[PosTroca].TamCiclo*PORC_START_DELAY){
									pSolucao->pSol[PosTroca].StartDelay = pSolucao->pSol[PosTroca].TamCiclo*PORC_START_DELAY;
							}
							break;
					}
				case SA_CNF_PERT_RANDOM:
					{
							pSolucao->pSol[PosTroca].StartDelay = (double) (rand() % ((u_int32_t)(pSolucao->pSol[PosTroca].TamCiclo*PORC_START_DELAY)));
							break;
					}
			}

			/* CODIGO ABAIXO É A LIMITAÇÃO DO START DELAY */
			if (SET_LIMIT_DELAY)
					if ((pSolucao->pSol[PosTroca].StartDelay > 0)&&(pSolucao->pSol[PosTroca].StartDelay < LIMIT_DELAY))
          {
							double prob = ((double) rand() / RAND_MAX);
							if (prob <= 0.5)
                  if (LIMIT_DELAY < pSolucao->pSol[PosTroca].deadline_time)
                      pSolucao->pSol[PosTroca].StartDelay = LIMIT_DELAY;
                  else
                      pSolucao->pSol[PosTroca].StartDelay = 0.0;
							else
									pSolucao->pSol[PosTroca].StartDelay = 0.0;
					}
	  }
}

/*****************************************************************************************/
/*                                                                                       */
/* Gera arquivo de entrada para o script que faz o merge com estes valores e o formato   */
/* OBDC usado pelo CANOe                                                                 */
/*                                                                                       */
/*****************************************************************************************/
void SaGeraArquivoEntradaParaODBC(char* pSaida, StSaSolucao* pSolucao)
{
	FILE      *pArq;
	u_int16_t i;

		/* Abre o arquivo de saida para informar os tempos propostos para a simulação no CANOe */
	pArq = fopen(pSaida, "w");

	/* Verifica se o arquivo foi criado com sucesso */
	if(pArq == NULL)
	{
		printf("\n================================================================================");
		printf("\n[ERRO] arquivo '%s' não pode ser criado em SaGeraArquivoEntradaParaODBC()\n\n", pSaida);
		printf("\n================================================================================\n");
		exit(SA_ERRO_IO);
	}

	/* Escreve o cabecalho do arquivo, que contera as strings usadas pelo script AWK para */
	/* replace no arquivo ODBC (deve respeitar os nomes usados no script) */

	/*fprintf(pArq, "MsgIDtCycleTime\tDelayTime\tStartDelayTime\n");*/

	/* Para cada mensagem contida na solucao, informa os seus tempos */
	for(i = 0; i < SaNumMsgCan; i++)
	{
		fprintf(pArq, "%u\t%lf\t%lf\t%lf\t%u\n", pSolucao->pSol[i].Id, pSolucao->pSol[i].TamCiclo, pSolucao->pSol[i].deadline_time,pSolucao->pSol[i].StartDelay, pSolucao->pSol[i].payload);
	}

	/* Fecha o arquivo informado */
	fclose(pArq);
}

/*****************************************************************************************/
/*                                                                                       */
/* Estima o Busload teorico da temporizacao informada mediante simulacao no CANOe        */
/*                                                                                       */
/*****************************************************************************************/
void SaEstimaBusloadViaSimulacao(StSaSolucao* pSolucao)
{
	/* Grava arquivo para o simulador poder executar-lo */
	SaGeraArquivoEntradaParaODBC(SaArqTempos, pSolucao);

	/* Pega WCRT e o max tempo de fila após executar no simulador */
	start_can_simulated(SaArqTempos, TIME_CAN_SIMULATED);

		pSolucao->WCRT       = wcrt;
		pSolucao->burst_time = time_mean_queue;
		pSolucao->burst_size = avg_length_queue;
}

/*****************************************************************************************/
/*                                                                                       */
/*          Printa na tela a melhor solução encontrada parametro -r omitido              */
/*                                                                                       */
/*****************************************************************************************/
void SaPrintResultado(StSaSolucao* pSolucao)
{
	time_t     rawtime;
 	struct tm* timeinfo;

	/* Obtem a data/hora atual */
	time (&rawtime);
  timeinfo = localtime(&rawtime);

	/* Cria o cabecalho do arquivo de saida final */
	printf("RESULTS @ %s\n", asctime(timeinfo));
	printf("Função Objetiva \t%lf\n", SaCalculaObjetiva(pSolucao));
	printf("WCRT\t\t%lf\n", pSolucao->WCRT);
	printf("TIME BURST\t%lf\n", pSolucao->burst_time);
	printf("SIZE BURST\t%lf\n\n", pSolucao->burst_size);
	printf("ID_MSG\tCYCLE\tDEADLINE\tSTART_DLY\tPAYLOAD\n");

	/* Para cada mensagem contida na solucao, informa os seus tempos */
	for(u_int16_t i = 0; i< SaNumMsgCan; i++)
	{
		printf("%ld\t%lf\t%lf\t%lf\t%u\n", pSolucao->pSol[i].Id, pSolucao->pSol[i].TamCiclo, pSolucao->pSol[i].deadline_time,pSolucao->pSol[i].StartDelay, pSolucao->pSol[i].payload);
	}

}

/*****************************************************************************************/
/*                                                                                       */
/* Gera arquivo de saida com a solucao encontrada                                        */
/*                                                                                       */
/*****************************************************************************************/
void SaLogResultado(StSaSolucao* pSolucao, char* Nome)
{
	FILE*      pArq;
	u_int16_t  i;
	time_t     rawtime;
 	struct tm* timeinfo;

  pArq = fopen(Nome, "w+");

	if(pArq == NULL)
	{
		printf("\n================================================================================");
		printf("\n[ERRO] arquivo '%s' não pode ser criado em SaLogResultado()\n\n", Nome);
		printf("\n================================================================================\n");
		exit(SA_ERRO_IO);
	}

	rewind(pArq);

	/* Obtem a data/hora atual */
	time (&rawtime);
  	timeinfo = localtime(&rawtime);

	/* Cria o cabecalho do arquivo de saida final */
	fprintf(pArq, "RESULTS @ %s\n", asctime(timeinfo));
	fprintf(pArq, "Função Objetiva \t%lf\n", SaCalculaObjetiva(pSolucao));
	fprintf(pArq, "WCRT\t\t%lf\n", pSolucao->WCRT);
	fprintf(pArq, "TIME BURST\t%lf\n", pSolucao->burst_time);
	fprintf(pArq, "SIZE BURST\t%lf\n\n", pSolucao->burst_size);
	fprintf(pArq, "ID_MSG\tCYCLE\tDEADLINE\tSTART_DLY\tPAYLOAD\n");

	/* Para cada mensagem contida na solucao, informa os seus tempos */
	for(i = 0; i< SaNumMsgCan; i++)
	{
		fprintf(pArq, "%ld\t%lf\t%lf\t%lf\t%u\n", pSolucao->pSol[i].Id, pSolucao->pSol[i].TamCiclo, pSolucao->pSol[i].deadline_time,pSolucao->pSol[i].StartDelay, pSolucao->pSol[i].payload);
	}

	/* Fecha arquivo de saida */
	fclose(pArq);
}

/*****************************************************************************************/
/*                                                                                       */
/*    Calcula Função Objetiva                                                            */
/*                                                                                       */
/*****************************************************************************************/

double SaCalculaObjetiva(StSaSolucao* pSolucao)
{
		double objetiva;

		objetiva =  pSolucao->WCRT*ESCALAR_WCRT;
    objetiva += pSolucao->burst_time*ESCALAR_TQUEUE;
    objetiva += pSolucao->burst_size*ESCALAR_LQUEUE;
		for(u_int16_t i = 0; i < SaNumMsgCan; i++)
				objetiva += pSolucao->pSol[i].StartDelay*ESCALAR_DELAY;

		return objetiva;
}
/*****************************************************************************************/
/*                                                                                       */
/* Grava solução current em arquivo                                                      */
/*                                                                                       */
/*****************************************************************************************/

void SaGravaSolucaoCurrent(StSaSolucao* pSolucao, u_int32_t iterador)
{
		double sum_delay = 0;

		for (u_int16_t i = 0; i < SaNumMsgCan; i++){
				sum_delay += pSolucao->pSol[i].StartDelay;
		}

		fprintf(Arq_OBJ, "%ld\t%lf\t%lf\t%lf\n", iterador, SaCalculaObjetiva(pSolucao), pSolucao->WCRT, sum_delay);
}

/*****************************************************************************************/
/*                                                                                       */
/* Grava solução best em arquivo                                                      */
/*                                                                                       */
/*****************************************************************************************/

void SaGravaSolucaoBest(StSaSolucao* solucao)
{

	char path[SA_MAX_CHAR_COMMAND_LINE+5];

	static u_int32_t cont = 1;

	sprintf(path,"%s%ld", SaArqLogSim, cont++);

	Arq_Best = fopen(path, "w");

	if (!Arq_Best){
			printf("\n================================================================================");
			printf("\n[ERRO] falha na alocacao de memoria em SaGravaSolucaoBest()\n\n");
			printf("\n================================================================================\n");
			exit(SA_ERRO_MEMORIA);
	}

	fprintf(Arq_Best, "ID\tCYCLE\tOFFSET\tDURATION\n");
	for (u_int16_t i=0; i < SaNumMsgCan; i++){
			fprintf(Arq_Best, "%ld\t%lf\t%lf\t0.1277\n", solucao->pSol[i].Id, solucao->pSol[i].TamCiclo, solucao->pSol[i].StartDelay);
	}

	fclose(Arq_Best);
}

/*****************************************************************************************/
/*                                                                                       */
/* Implementa o metodo principal do Simulated Annealing                                  */
/*                                                                                       */
/*****************************************************************************************/

#define SA_CNF_INICIAL_ALEATORIA 0x00
#define SA_CNF_INICIO_ZERADO     0x01

void SaSimulatedAnnealing(void)
{
	double 			Temperatura;
	double 			Delta;
	double 			Probabilidade;
	double 			Sorteio;
	u_int8_t		i;
	u_int8_t		SolNome[512];

	u_int32_t   iterador = 0;
	u_int8_t    Metodo   = SA_CNF_INICIO_ZERADO;

	/* Aloca memoria para a solucao inicial e vizinha */
	pSaCorrente      = SaAlocaSolucao();
	pSaMelhor        = SaAlocaSolucao();
	pSaVizinho       = SaAlocaSolucao();
	pSaMelhorVizinho = SaAlocaSolucao();

	if (SA_GRAVA_LOGSIM)
	{
			sprintf(path_file_best, "%s-%ld.dat", SaArqLogSim, log_frame++);
			Arq_Log_Best = fopen(path_file_best, "w");
			if (!Arq_Log_Best){
					printf("\n================================================================================");
					printf("\n[ERRO] arquivo '%s' não pode ser criado em SaSimulatedAnnealing()\n\n", path_file_best);
					printf("\n================================================================================\n");
					exit(SA_ERRO_IO);
			}
			logframes    = TRUE;
			SaEstimaBusloadViaSimulacao(pSaMelhor);
			logframes    = FALSE;
			fclose(Arq_Log_Best);
	}

	/* Inicializa uma solucao com tempos aleatorios */
	switch(Metodo)
	{
		/* Constroi aleatoriamente o slot de cada mensagem dividindo o ciclo total */
		/* em dois pedacos: ciclo e delay */
		case SA_CNF_INICIAL_ALEATORIA:
		{
			SaCriaSolucaoAleatoria(pSaCorrente);
			break;
		}
		case SA_CNF_INICIO_ZERADO:
		{
			/* ja começa zerado os start_delays */
			break;
		}
		/* Configuracao invalida */
		default:
		{
			printf("\n================================================================================");
			printf("\n[ERRO] metodo de construcao inicial desconhecido: %d\n", Metodo);
			printf("\n================================================================================\n");
			exit(SA_ERRO_CONFIGURACAO);

			break;
		}
	}

	/* Avalia o busload da solucao inicial */
	SaEstimaBusloadViaSimulacao(pSaCorrente);
	/* Copia a solucao melhor como sendo a corrente */
	SaClonaSolucao(pSaMelhor, pSaCorrente);

	if (SA_VERBOSE_FULL)
		/* Exibe a solucao corrente apos construcao */
		SaDbgExibeSolucao(pSaCorrente, "pSaInicial");

	/* Define parametros para avanco do metodo */
	Temperatura = SaTempInicial;

	u_int8_t num_reaquecimento = 0;
	/* Laco principal do Simulated Annealing */
	do
	{
    if (SA_VERBOSE)
    {
        printf("\n================================================================================\n");
        printf("\n[INFO] Temperature @ %.2lf Degrees, reheating %d of %d times\n\n", Temperatura, num_reaquecimento, SaNumReaquecimento);
        printf("================================================================================\n");
    }

		for (int j = 1; j <= SaNumIteracao; j++)
		{
				/*iterador do SA*/
				// iterador++;
				/* Cria uma copia do vizinho a partir da melhor solucao corrente */
				SaClonaSolucao(pSaVizinho, pSaCorrente);
				/* Perturba a solucao vizinho, de forma a gerar nova temporizacao proposta */
				SaPerturbaSolucaoVizinhancaUniforme(pSaVizinho);
				/* Avalia o Busload da solucao usando simulacao */
				SaEstimaBusloadViaSimulacao(pSaVizinho);

				if (SA_VERBOSE_FULL)
				{
						sprintf(SolNome, "\n[INFO] pSaVizinho # %d @ %lf", i, Temperatura);
						SaDbgExibeSolucao(pSaVizinho, SolNome);
				}

				/* Embora seja uma solucao com busload pior, pode atualizar se passar no criterio */
				/* de aceitacao de Boltzman */
				Delta = (SaCalculaObjetiva(pSaVizinho)-SaCalculaObjetiva(pSaCorrente));

				/* Caso o melhor dos vizinhos seja melhor do que a melhor solucao conhecida, atualiza */
				if(Delta <= 0.0)
				{
						if (SA_VERBOSE_FULL)
							printf("[INFO] Encontrado melhor do que corrente: %lf contra %lf\n", SaCalculaObjetiva(pSaMelhorVizinho), SaCalculaObjetiva(pSaCorrente));

						/* Atualiza a solucao corrente e a 'overall' */
						SaClonaSolucao(pSaCorrente, pSaVizinho);

						if (SA_VERBOSE_FULL)
							SaDbgExibeSolucao(pSaMelhor, "pSaMelhor");


						/* Verifica se o melhor vizinho é melhor do que a 'overall solution' */
						if(SaCalculaObjetiva(pSaVizinho) < SaCalculaObjetiva(pSaMelhor))
						{
								/* Atualiza a melhor de todas as solucoes */
								SaClonaSolucao(pSaMelhor, pSaVizinho);

								if (SA_VERBOSE_FULL)
									printf("\n[INFO] Novo OVERALL encontrado: %lf\n", SaCalculaObjetiva(pSaMelhor));

								if (SA_GRAVA_LOGSIM)
								{
										sprintf(path_file_best, "%s-%ld.dat", SaArqLogSim, log_frame++);
										Arq_Log_Best = fopen(path_file_best, "w");
										if (!Arq_Log_Best){
												printf("\n================================================================================");
												printf("\n[ERRO] arquivo '%s' não pode ser criado em SaSimulatedAnnealing()\n\n", path_file_best);
												printf("\n================================================================================\n");
												exit(SA_ERRO_IO);
										}
										logframes    = TRUE;
										SaEstimaBusloadViaSimulacao(pSaMelhor);
										logframes    = FALSE;
										fclose(Arq_Log_Best);
								}

						}
            if (SA_GRAVA_WCRT)
            {
                sprintf(path_file_wcrt, "%s-%ld.dat", SaWCRTFile, cont_wcrt++);
                print_wcrt(path_file_wcrt);
            }
            if (SA_GRAVA_OBJ)
            /*Grava em arquivo solução corrente*/
            SaGravaSolucaoCurrent(pSaCorrente, iterador++);
				}
				else
				{
					/* Calcula a probabilidade de aceitacao */
					Probabilidade = exp(-Delta/Temperatura);

					/* Sorteia um numero aleatorio entre 0 e 1 */
					Sorteio = ((double) rand() / RAND_MAX);

					if (SA_VERBOSE_FULL)
							printf("\n[INFO] Prob: %lf, Temp: %lf, Sorteio: %lf, Viz %lf, Cur %lf)\n", Probabilidade, Temperatura, Sorteio, pSaVizinho->WCRT, pSaCorrente->WCRT);


					/* Se valor sorteado esta dentro do criterio, atualiza solucao corrente */
					if(Sorteio <= Probabilidade)
					{
						/* Atualiza apenas a solucao corrente, nao a 'overall' */
						SaClonaSolucao(pSaCorrente, pSaVizinho);

						if (SA_VERBOSE_FULL)
							SaDbgExibeSolucao(pSaCorrente, "pSaCorrente (Passou Criterio Boltzman)");

					}
					// if (SA_GRAVA_OBJ)
					// /*Grava em arquivo solução corrente*/
					// SaGravaSolucaoCurrent(pSaCorrente, iterador++);
				}
        // if (SA_GRAVA_OBJ)
        // /*Grava em arquivo solução corrente*/
        // SaGravaSolucaoCurrent(pSaCorrente, iterador++);
		}
		/* Decai a temperatura de acordo com o valor de Alpha */
		Temperatura = Temperatura*SaAlpha;

		/*Verifica se temperatura chegou no limite para ajusta o numero de reaquecimento*/
		if (Temperatura <= SaTempFinal)
		{
			 Temperatura = SaTempInicial;
			 num_reaquecimento++;
			 if (num_reaquecimento <= SaNumReaquecimento)
			 {
					 printf("\n\n================================================================================");
			 	 	 printf("\n[INFO] Number of reheating %d times\n\n", num_reaquecimento);
			 		 printf("================================================================================\n\n");
		 	 }
		}

	}while((Temperatura > SaTempFinal)&&(num_reaquecimento <= SaNumReaquecimento));

	/* Substitui o arquivo DBC com o tempo da melhor solucao encontrada pelo SA */
	SaGeraArquivoEntradaParaODBC(SaArqTempos, pSaMelhor);

	/* Efetua o log da melhor solucao encontrada ou printa na tela */
  if (SA_RESULT)
    SaPrintResultado(pSaMelhor);
  else
    SaLogResultado(pSaMelhor, SaArqSaida);

	/* Libera memoria das solucoes usadas */
	SaDesalocaSolucao(pSaCorrente);
	SaDesalocaSolucao(pSaMelhor);
	SaDesalocaSolucao(pSaVizinho);
	SaDesalocaSolucao(pSaMelhorVizinho);
}

/*****************************************************************************************/
/*                                                                                       */
/* Libera memoria usada para gerenciar as estruturas de dados da aplicacao               */
/*                                                                                       */
/*****************************************************************************************/
void SaLiberaMemoria(void)
{
	u_int16_t i;

	/* Varre as estruturas de parametros das mensagens para liberar memoria ocupada pelas */
	/* mesmas */

	/* Libera memoria do vetor de controle de parametros */
	free(pSaMsgParArray);

	/* Libera memoria do vetor de controle de posicoes sorteadas */
	free(pSaBitPosicao);
}

/*****************************************************************************************/
/*                                                                                       */
/* Funcao principal da aplicacao                                                         */
/*                                                                                       */
/*****************************************************************************************/
u_int8_t main_simulated_annealing(u_int8_t argc, char *argv[])
{
	#if SA_TIMER
  		/* Contabiliza tempo de execucao */
  		TmrAlocaTemporizador(SA_TIMER_SIMULATED, TRUE, "Principal SA");
  		TmrStart(SA_TIMER_SIMULATED);
	#endif

	/* Executa o Simulated Annealing */
	SaSimulatedAnnealing();

	#if SA_TIMER
  		/* Contabiliza tempo de execucao */
  		TmrStop(SA_TIMER_SIMULATED);
  		printf("\n[INFO] Runtime SA Only (s): %f\n\n", TmrObtemTemporizador(SA_TIMER_SIMULATED));
	#endif

	/* Libera estruturas de dados usadas nos computos */
	SaLiberaMemoria();

	if (SA_GRAVA_OBJ)
			fclose(Arq_OBJ);

	/* Informa ao sistema operacional que tudo ocorreu conforme previsto */
	return(SA_ERRO_NONE);
}
