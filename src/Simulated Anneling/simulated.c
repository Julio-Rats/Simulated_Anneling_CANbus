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
		printf("\n================================================================================\n");
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
			case 'n':
			{
				/* Le a quantidade de mensagens esperada no arquivo */
				fscanf(pArq, "%hd", &SaNumMsgCan);

				/* Ignora restante da linha */
				do
				{
					fscanf(pArq, "%c", &Lixo);
				}
				while(Lixo != '\n');

				/* Inicializa a estrutura de dados correspondente aos parametros das */
				/* mensagens CAN */
				pSaMsgParArray = (StSaMsgPar*) malloc(sizeof(StSaMsgPar) * SaNumMsgCan);

				/* Caso a alocacao nao tenha ocorrido com exito, avisar */
				if(pSaMsgParArray == NULL)
				{
					printf("\n================================================================================");
					printf("\n[ERRO] falha na alocacao de memoria em SaAbreArquivoConfiguracao()\n\n");
					printf("\n================================================================================\n");
					exit(SA_ERRO_MEMORIA);
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

				/* Inicializa cada slot do array de controle de parametros com valores default */
				for(i = 0; i < SaNumMsgCan; i++)
				{
					/* Copia ID e tempo maximo do ciclo */
					pSaMsgParArray[i].Id = SA_ID_NONE;
					pSaMsgParArray[i].MaxCiclo = DBL_MAX;

					/* Aloca memoria para o nome da mensagem */
					pSaMsgParArray[i].pNome = (char *) malloc(sizeof(char) * SA_MAX_CHAR_NOME_CAN_MESG);
					if(pSaMsgParArray[i].pNome == NULL)
					{
						printf("\n================================================================================");
						printf("\n[ERRO] falha na alocacao de memoria em SaAbreArquivoConfiguracao()\n\n");
						printf("\n================================================================================\n");
						exit(SA_ERRO_MEMORIA);
					}

					/* Copia o nome da mensagem */
					strcpy(pSaMsgParArray[i].pNome, "NO NAME");
				}

				break;
			}

			/* Caso seja lido uma entrada de configuracao de mensagem */
			case 'm':
			{
				/* Verifica se existe espaco para uma nova entrada no array de parametros */
				if(IdxCtrlEntrada < SaNumMsgCan)
				{
					/* Obtem os dados do arquivo correspondente aos parametros da */
					/* mensagem em questao */
					fscanf(pArq, "%hd", &Id);
					fscanf(pArq, "%lf", &MaxCiclo);
					fgets(NomeMsg, SA_MAX_CHAR_NOME_CAN_MESG, pArq);

					/* Preenche os dados obtidos do arquivo */
					pSaMsgParArray[IdxCtrlEntrada].Id = Id;
					pSaMsgParArray[IdxCtrlEntrada].MaxCiclo = MaxCiclo;
					strcpy(pSaMsgParArray[IdxCtrlEntrada].pNome, NomeMsg);

					/* Atualiza o contador de slots preenchidos */
					IdxCtrlEntrada++;
				}
				else
				{
					printf("\n================================================================================");
					printf("\n[ERRO] Existem mais mensagens no arquivo de configuracao do que o que foi especificado\n\n");
					printf("\n================================================================================\n");
					exit(SA_ERRO_PARSING_CONFIG);
				}

				break;
			}

			/* Marcador correspondente ao 'End of File', nada mais será lido a seguir */
			case 'e':
			{
				/* Marca flag de termino de leitura do arquivo */
				Sair = TRUE;

				break;
			}

			/* Marcador de numero de vizinhos pesquisados na busca local do Simulated Annealing */
			case 'v':
			{
				/* Le a quantidade de vizinhos do arquivo */
				fscanf(pArq, "%hd", &SaNumVizinhos);

				/* Ignora restante da linha */
				do
				{
					fscanf(pArq, "%c", &Lixo);
				}
				while(Lixo != '\n');

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
					fscanf(pArq, "%hhu", &SaNumIteracao);

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
								printf("\n================================================================================\n");
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
				printf("\n================================================================================\n");
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
		printf("Cycle : %lf\n", pSaMsgParArray[i].MaxCiclo);
		printf("Name  : %s\n",  pSaMsgParArray[i].pNome);
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
	pSolucao->WCRT       = DBL_MAX;
	pSolucao->Time_Queue = DBL_MAX;
	pSolucao->Busload    = DBL_MAX;
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
		pSolucao->pSol[i].Id          = pSaMsgParArray[i].Id;
		pSolucao->pSol[i].TamCiclo    = pSaMsgParArray[i].MaxCiclo;
		pSolucao->pSol[i].StartDelay  = DBL_MIN;
		pSolucao->pSol[i].ProbSelecao = (1.0 / pow(pSaMsgParArray[i].Id, 0.5)) / SomaID;
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
	pCopia->Time_Queue = pBase->Time_Queue;

	/* Copia os slots da solucao base para a solucao clonada */
	for(i = 0; i <SaNumMsgCan; i++)
	{
		/* Copia dados dos parametros, quando pertinente */
		pCopia->pSol[i].Id         = pBase->pSol[i].Id;
		pCopia->pSol[i].TamCiclo   = pBase->pSol[i].TamCiclo;
		pCopia->pSol[i].StartDelay = pBase->pSol[i].StartDelay;
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
	printf("\n[INFO] Solution '%s'. WCRT(%%) = %lf\t TIME_QUEUE = %lf\n", pNome, pSolucao->WCRT, pSolucao->Time_Queue);
	printf("================================================================================\n");


	#if SA_VERBOSE
			printf("\n[DEBUG] Solution Timers:\n\n");
			for(i = 0; i < SaNumMsgCan; i++)
			{
				printf("Message Id        : %ld\n",      pSolucao->pSol[i].Id);
				printf("Message Cycle Time: %.0lf\n",    pSolucao->pSol[i].TamCiclo);
				printf("Message StartDelay: %.0lf\n",    pSolucao->pSol[i].StartDelay);
			}
	#endif

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

		#if SA_VERBOSE
			printf("\n\t\t\t[DEBUG] PosTroca @ %d: %d", i, PosTroca);
		#endif

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
				case SA_CNF_PERT_RAND_DELAY:
					{
							if (pSolucao->pSol[PosTroca].StartDelay == 0){
									double prob = ((double) rand() / RAND_MAX);
									if (prob <= 0.5)
										 	pSolucao->pSol[PosTroca].StartDelay = (double) (rand() % ((u_int16_t)(pSolucao->pSol[PosTroca].TamCiclo*PORC_START_DELAY)));

									break;
							}
							pSolucao->pSol[PosTroca].StartDelay = (double) (rand() % ((u_int16_t) pSolucao->pSol[PosTroca].StartDelay));
							break;
					}
			}

			/* CODIGO ABAIXO É A LIMITAÇÃO DE 7 MS PARA O DELAY */

			// if ((pSolucao->pSol[PosTroca].StartDelay > 0)&&(pSolucao->pSol[PosTroca].StartDelay <= 6)){
			// 		double prob = ((double) rand() / RAND_MAX);
			// 		if (prob <= 0.5)
			// 				pSolucao->pSol[PosTroca].StartDelay = 7.0;
			// 		else
			// 				pSolucao->pSol[PosTroca].StartDelay = 0.0;
			// }
			//
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
		fprintf(pArq, "%ld\t%lf\t%lf\n", pSolucao->pSol[i].Id, pSolucao->pSol[i].TamCiclo, pSolucao->pSol[i].StartDelay);
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
	pSolucao->Time_Queue = time_max_queue;
	pSolucao->Busload    = busload_simulated;
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

	/* Abre o arquivo de saida para informar os tempos propostos para a simulação no CANOe */
	pArq = fopen(Nome, "r");

	/* Verificar se o arquivo foi aberto. Se sim, abre para append, caso contrario cria antes */
	if(pArq == NULL)
		 	/* Cria arquivo do zero */
			pArq = fopen(Nome, "w+");
	else
			/* Abre arquivo para 'append' */
			pArq = fopen(Nome, "a");

	/* Verifica se o arquivo foi criado com sucesso */
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
	fprintf(pArq, "\n\nRESULTS @ %s\n", asctime(timeinfo));
	fprintf(pArq, "Função Objetiva \t%lf\n", SaCalculaObjetiva(pSolucao));
	fprintf(pArq, "WCRT\t\t%lf\n", pSolucao->WCRT);
	fprintf(pArq, "TIME BURST\t%lf\n\n", pSolucao->Time_Queue);
	fprintf(pArq, "ID_MSG\tCYCLE\t\tSTART_DLY\n");

	/* Para cada mensagem contida na solucao, informa os seus tempos */
	for(i = 0; i< SaNumMsgCan; i++)
	{
		fprintf(pArq, "%ld\t%lf\t%lf\n", pSolucao->pSol[i].Id, pSolucao->pSol[i].TamCiclo, pSolucao->pSol[i].StartDelay);
	}

	/* Fecha arquivo de saida */
	fclose(pArq);
}

/*****************************************************************************************/
/*                                                                                       */
/*    Calcula Função Objetiva                                                            */
/*                                                                                       */
/*****************************************************************************************/

double SaCalculaObjetiva(StSaSolucao* pSolucao){
		double objetiva;

		objetiva = pSolucao->WCRT*ESCALAR_WCRT;
		for(u_int16_t i = 0; i < SaNumMsgCan; i++)
				objetiva += pSolucao->pSol[i].StartDelay*ESCALAR_DELAY;

		return objetiva;
}
/*****************************************************************************************/
/*                                                                                       */
/* Grava solução current em arquivo                                                      */
/*                                                                                       */
/*****************************************************************************************/

void SaGravaSolucaoCurrent(StSaSolucao* pSolucao, u_int32_t iterador){
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

void SaGravaSolucaoBest(StSaSolucao* solucao){

	char path[SA_MAX_CHAR_COMMAND_LINE+5];

	static u_int32_t cont = 1;

	sprintf(path,"%s%ld", SaArqBest, cont++);

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

	#if SA_GRAVA_BEST
			char path_file_best[SA_MAX_CHAR_COMMAND_LINE+5];
			sprintf(path_file_best, "%s-%ld.dat", SaArqBest, log_frame++);
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
	#endif

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

	#if SA_VERBOSE_PROB
		/* Exibe a solucao corrente apos construcao */
		SaDbgExibeSolucao(pSaCorrente, "pSaInicial");
	#endif

	/* Define parametros para avanco do metodo */
	Temperatura = SaTempInicial;

	u_int8_t num_reaquecimento = 0;

	/* Laco principal do Simulated Annealing */
	do
	{
		printf("\n================================================================================");
		printf("\n[INFO] Temperature @ %.2lf Degrees, reheating %d of %d times\n\n", Temperatura, num_reaquecimento, SaNumReaquecimento);
		printf("================================================================================\n");

		for (int j = 1; j <= SaNumIteracao; j++)
		{
				/*iterador do SA*/
				iterador++;
				/* Cria uma copia do vizinho a partir da melhor solucao corrente */
				SaClonaSolucao(pSaVizinho, pSaCorrente);
				/* Perturba a solucao vizinho, de forma a gerar nova temporizacao proposta */
				SaPerturbaSolucaoVizinhancaUniforme(pSaVizinho);
				/* Avalia o Busload da solucao usando simulacao */
				SaEstimaBusloadViaSimulacao(pSaVizinho);

				#if SA_VERBOSE_PROB
						sprintf(SolNome, "\n[INFO] pSaVizinho # %d @ %lf", i, Temperatura);
						SaDbgExibeSolucao(pSaVizinho, SolNome);
				#endif

				/* Embora seja uma solucao com busload pior, pode atualizar se passar no criterio */
				/* de aceitacao de Boltzman */
				Delta = (SaCalculaObjetiva(pSaVizinho)-SaCalculaObjetiva(pSaCorrente));

				/* Caso o melhor dos vizinhos seja melhor do que a melhor solucao conhecida, atualiza */
				if(Delta <= 0.0)
				{
						#if SA_VERBOSE_PROB
							printf("[INFO] Encontrado melhor do que corrente: %lf contra %lf\n", SaCalculaObjetiva(pSaMelhorVizinho), SaCalculaObjetiva(pSaCorrente));
						#endif

						/* Atualiza a solucao corrente e a 'overall' */
						SaClonaSolucao(pSaCorrente, pSaVizinho);

						#if SA_VERBOSE_PROB
							SaDbgExibeSolucao(pSaMelhor, "pSaMelhor");
						#endif

						/* Verifica se o melhor vizinho é melhor do que a 'overall solution' */
						if(SaCalculaObjetiva(pSaVizinho) < SaCalculaObjetiva(pSaMelhor))
						{
								/* Atualiza a melhor de todas as solucoes */
								SaClonaSolucao(pSaMelhor, pSaVizinho);

								#if SA_VERBOSE_PROB
									printf("\n[INFO] Novo OVERALL encontrado: %lf\n", SaCalculaObjetiva(pSaMelhor));
								#endif
								#if SA_GRAVA_BEST
										sprintf(path_file_best, "%s-%ld.dat", SaArqBest, log_frame++);
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
								#endif
						}
				}
				else
				{
					/* Calcula a probabilidade de aceitacao */
					Probabilidade = exp(-Delta/Temperatura);

					/* Sorteia um numero aleatorio entre 0 e 1 */
					Sorteio = ((double) rand() / RAND_MAX);

					#if SA_VERBOSE_PROB
						printf("\n[INFO] Prob: %lf, Temp: %lf, Sorteio: %lf, Viz %lf, Cur %lf)\n", Probabilidade, Temperatura, Sorteio, pSaVizinho->WCRT, pSaCorrente->WCRT);
					#endif

					/* Se valor sorteado esta dentro do criterio, atualiza solucao corrente */
					if(Sorteio <= Probabilidade)
					{
						/* Atualiza apenas a solucao corrente, nao a 'overall' */
						SaClonaSolucao(pSaCorrente, pSaVizinho);

						#if SA_VERBOSE_PROB
							SaDbgExibeSolucao(pSaCorrente, "pSaCorrente (Passou Criterio Boltzman)");
						#endif
					}
				}
			  #if SA_GRAVA_OBJ
				 	 // usleep(300);
					 /*Grava em arquivo solução corrente*/
					 SaGravaSolucaoCurrent(pSaCorrente, iterador);
			  #endif
		}
		/* Decai a temperatura de acordo com o valor de Alpha */
		Temperatura = Temperatura*SaAlpha;

		/*Verifica se temperatura chegou no limite para ajusta o numero de reaquecimento*/
		if (Temperatura <= SaTempFinal){
			 Temperatura = SaTempInicial;
			 num_reaquecimento++;
			 if (num_reaquecimento <= SaNumReaquecimento){
					 printf("\n\n================================================================================");
			 	 	 printf("\n[INFO] Number of reheating %d times\n\n", num_reaquecimento);
			 		 printf("================================================================================\n\n");
		 	 }
		}

	}while((Temperatura > SaTempFinal)&&(num_reaquecimento <= SaNumReaquecimento));

	/* Efetua o log da melhor solucao encontrada */
	SaLogResultado(pSaMelhor, SaArqSaida);

	/* Substitui o arquivo DBC com o tempo da melhor solucao encontrada pelo SA */
	SaGeraArquivoEntradaParaODBC(SaArqTempos, pSaMelhor);

	#if SA_VERBOSE_PROB
		/* Exibe a melhor solucao */
		SaDbgExibeSolucao(pSaMelhor, "pSaMelhor");
	#endif

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
	for(i = 0; i < SaNumMsgCan; i++)
	{
		/* Libera memoria usada pela string da mensagem CAN */
		free(pSaMsgParArray[i].pNome);
	}

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
u_int8_t main(u_int8_t argc, char **argv)
{
	/* Inicializa gerador de numeros aleatorios com semente dada pelo timestamp do sistema */
	srand(time(NULL));

	void erro_args(int op){
			printf("\n================================================================================\n\n");
			switch (op) {
				case 0:
						printf("[ERRO] Sintaxe incorreta. Usar:\n\n%s <arq. Config> <arq. Tempos> <arq. Result>\n", argv[0]);
				break;
				case 1:
						printf("[ERRO] Sintaxe incorreta. Usar:\n\n%s <arq. Config> <arq. Tempos> <arq. Result> <arq. LogOBJ>\n", argv[0]);
				break;
				case 2:
						printf("[ERRO] Sintaxe incorreta. Usar:\n\n%s <arq. Config> <arq. Tempos> <arq. Result> <arq. LogBest>\n", argv[0]);
				break;
				case 3:
						printf("[ERRO] Sintaxe incorreta. Usar:\n\n%s <arq. Config> <arq. Tempos> <arq. Result> <arq. LogOBJ> <arq. LogBest>\n", argv[0]);
				break;
			}
			printf("\n================================================================================\n\n");
			exit(SA_ERRO_ARGS_INVALIDOS);
	}

	/* Verifica a quantidade de argumentos informada */
	if (SA_GRAVA_BEST && SA_GRAVA_OBJ){
		if (argc != 6)
			erro_args(3);
	}else if (SA_GRAVA_BEST){
			if (argc != 5)
				erro_args(2);
	}else if (SA_GRAVA_OBJ){
			if (argc != 5)
				erro_args(1);
	}else if (argc != 4)
			erro_args(0);

	/* Guarda o nome dos arquivos informados */
	strcpy(SaArqConfiguracao, argv[1]);
	strcpy(SaArqTempos,      argv[2]);
	strcpy(SaArqSaida,       argv[3]);
	if (SA_GRAVA_BEST && SA_GRAVA_OBJ){
			strcpy(SaArqLogEvolOBJ,  argv[4]);
			strcpy(SaArqBest,        argv[5]);
	}else if (SA_GRAVA_BEST){
			strcpy(SaArqBest,        argv[4]);
	}else if (SA_GRAVA_OBJ)
			strcpy(SaArqLogEvolOBJ,  argv[4]);

	/* Carrega o conteudo do arquivo de configuracao informado */
	SaAbreArquivoConfiguracao(SaArqConfiguracao);

	#if SA_GRAVA_OBJ
			Arq_OBJ = fopen(SaArqLogEvolOBJ, "w");
			if (!Arq_OBJ){
				printf("\n================================================================================");
				printf("\n[ERRO] arquivo '%s' não pode ser criado em main()\n\n", SaArqLogEvolOBJ);
				printf("\n================================================================================\n");
				exit(SA_ERRO_IO);
			}
	#endif

	/* Exibe os parametros do método */
	printf("\n[INFO] Simulated Annealing Parameters:\n\n");
	printf("# of Neighbors      : %ld\n", SaNumVizinhos);
	printf("# of Iteration      : %u\n",  SaNumIteracao);
	printf("# of reheating      : %d\n",  SaNumReaquecimento);
	printf("# of Perturbations  : %ld\n", SaNumSlotsPerturbacao);
	printf("Initial Temperature : %lf\n", SaTempInicial);
	printf("Final Temperature   : %lf\n", SaTempFinal);
	printf("Cooling Factor      : %lf\n", SaAlpha);
	printf("\n\n");

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
		printf("\n[INFO] Runtime SA Only (ms): %f\n\n", TmrObtemTemporizador(SA_TIMER_SIMULATED));
	#endif

	/* Libera estruturas de dados usadas nos computos */
	SaLiberaMemoria();

	#if SA_GRAVA_OBJ
			fclose(Arq_OBJ);
	#endif

	/* Informa ao sistema operacional que tudo ocorreu conforme previsto */
	return(SA_ERRO_NONE);
}
