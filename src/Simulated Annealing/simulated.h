#ifndef  SIMUL_SA
#define  SIMUL_SA

/*****************************************************************************************/
/*                                                                                       */
/* simulated.h                                                                           */
/*                                                                                       */
/* Implementa o método Simulated Annealing para ajuste de temporização de delay          */
/* em arquivos CANDB do CAN2.0A                                                          */
/*                                                                                       */
/*****************************************************************************************/

/*****************************************************************************************/
/*                                                                                       */
/*                  Bibliotecas da linguagem e definidas pelo usuario                    */
/*                                                                                       */
/*****************************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <float.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include "timer.h"
#include "../Simulador deterministico/simulador.h"


/*****************************************************************************************/
/*                                                                                       */
/*                                    Constantes                                         */
/*                                                                                       */
/*****************************************************************************************/
/* Numero maximo de caracteres no nome da mensagem, incluindo o \0 */
#define SA_MAX_CHAR_NOME_CAN_MESG  256

/* Numero maximo de caracteres no nome de arquivos e linha de comando */
#define SA_MAX_CHAR_COMMAND_LINE   512

/* Identifica um slot de parametros ou dados reservado, mas nao usado */
#define SA_ID_NONE                 USHRT_MAX

/* Constantes para verdadeiro ou falso */
#define TRUE                       1
#define FALSE                      0

#define PATH_SA           					"../Simulated Anneling/"

/*****************************************************************************************/
/*                                                                                       */
/*  Configuracoes especiais de modulos que podem ser habilitados em tempo de compilacao  */
/*  e parametros associados                                                              */
/*                                                                                       */
/*****************************************************************************************/
#define PORC_START_DELAY           1       /* Maior porpoção escolhida para StartDelay*/
#define ESCALAR_WCRT               1       /* Escalar F()Objetiva para WCRT*/
#define ESCALAR_TQUEUE             0       /* Escalar F()Objetiva para tempo medio da fila*/
#define ESCALAR_LQUEUE             0       /* Escalar F()Objetiva para tamanho medio mais desvio da fila*/
#define ESCALAR_DELAY              0       /* Escalar F()Objetiva para StartDelay*/
/* Habilita ou desabilita o modo de gravação do vizinho */
#define SA_TIMER                   1
#define SA_TIMER_SIMULATED         1

/* Habilita ou desabilita os modos verbose */
extern bool SA_VERBOSE;
extern bool SA_VERBOSE_FULL;

/* Habilita somente simulação*/
extern bool SA_ONLY_SIM;

/* Habilita escrita em arquivos de Logs */
extern bool SA_GRAVA_LOGSIM;
extern bool SA_GRAVA_OBJ;
extern bool SA_GRAVA_WCRT;

/* Indica a existencia do arquivo de config, parametro -c */
extern bool SA_HAVE_CONF;

/* Habilita limite minimo de StartDelay*/
extern bool SET_LIMIT_DELAY;

/* Printa resultados na tela, caso omitido -r */
extern bool SA_RESULT;

/* Tempo minimo para um StartDelay */
extern double LIMIT_DELAY;


/* Parametros para funcionamento do metodo Simulated Annealing */
extern double   		SaTempInicial;
extern double   		SaTempFinal;
extern double   		SaAlpha;
extern u_int16_t		SaNumSlotsPerturbacao;
extern u_int16_t		SaNumIteracao;
extern u_int8_t 		SaNumReaquecimento;
extern u_int8_t 		SaMetodoBusca;
extern u_int8_t 		SaMetodoPert;
extern u_int8_t 		SaMetodoInicial;
extern double 			TIME_CAN_SIMULATED;

/* Strings com os nomes dos arquivos informados via linha de comando */
extern char 				SaArqConfiguracao[SA_MAX_CHAR_COMMAND_LINE];
extern char 				SaArqLogEvolOBJ[SA_MAX_CHAR_COMMAND_LINE];
extern char 				path_file_best[SA_MAX_CHAR_COMMAND_LINE+5];
extern char 				path_file_wcrt[SA_MAX_CHAR_COMMAND_LINE+5];
extern char 				SaArqTempos[SA_MAX_CHAR_COMMAND_LINE];
extern char 				SaCANDBFile[SA_MAX_CHAR_COMMAND_LINE];
extern char 				SaArqSaida[SA_MAX_CHAR_COMMAND_LINE];
extern char 				SaWCRTFile[SA_MAX_CHAR_COMMAND_LINE];
extern char 				SaArqLogSim[SA_MAX_CHAR_COMMAND_LINE];
extern char 				SaSimFile[SA_MAX_CHAR_COMMAND_LINE];
extern char 				temp_time[SA_MAX_CHAR_COMMAND_LINE];

/*Descritores de arquvos, para os logs, modos verbose*/
extern FILE*  			Arq_OBJ;
extern FILE*  			Arq_Best;
extern FILE*  			Arq_WCRT;

/*****************************************************************************************/
/*                                                                                       */
/*  Defines para busca e perturbação dos vizinhos                                        */
/*                                                                                       */
/*****************************************************************************************/
#define SA_CNF_SELECAO_UNIFORME        0x00
#define SA_CNF_SELECAO_PROPORCIONAL_ID 0x01
#define SA_CNF_PERT_INCRIMENT          0x00
#define SA_CNF_PERT_RAND_DELAY         0x01
#define SA_CNF_PERT_RANDOM             0x02
#define SA_CNF_START_RANDOM            0x01
#define SA_CNF_START_ZERO              0x02


/*****************************************************************************************/
/*                                                                                       */
/*  Codigos de erro de retorno para o sistema operacional															   */
/*                                                                                       */
/*****************************************************************************************/
/* Codigo de erros para as operacoes criticas realizadas pela aplicacao */
#define SA_ERRO_NONE                      0
#define SA_ERRO_IO                        1
#define SA_ERRO_MEMORIA                   2
#define SA_ERRO_ARGS_INVALIDOS            3
#define SA_ERRO_PARSING_CONFIG            4
#define SA_ERRO_NO_VALUE                  5
#define SA_ERRO_ESTADO                    6
#define SA_ERRO_CONFIGURACAO              7

/* Slot com proposta de temporizacao para uma mensagem CAN */
typedef struct
{
	u_int16_t 		  Id:11;	     	 /* ID da mensagem   */
	double    		  TamCiclo;    	 /* Tamanho do ciclo atual */
	double    		  StartDelay;  	 /* Valor de offset inicial proposto */
	double    		  deadline_time; /* Valor do  tempo de deadline */
	u_int8_t  		  payload;       /* Valor do  tempo de deadline */
	double    		  ProbSelecao;   /* Probabilidade de selecao da mensagem caso a perturbacao seja proporcional ao ID */
}StSaMsgTmrSlot;

typedef struct
{
	double          WCRT;        /* Valor da busload associado com a temporizacao proposta */
	double          burst_time;  /* Tempo maximo de fila dessa solução*/
	double          burst_size;  /* busload dessa solução*/
	StSaMsgTmrSlot* pSol;        /* Vetor de slots com parametros propostos para solucao */
}StSaSolucao;

/*****************************************************************************************/
/*                                                                                       */
/*         Prototipos e implementacao das funcoes do modulo de ajuste de tempos          */
/*                                                                                       */
/*****************************************************************************************/
void         SaLiberaMemoria(void);
void         SaSimulatedAnnealing(void);
void         SaDbgPrintParametros(void);
void         get_candb(char* arch_name);
void 				 print_wcrt(char* arch_name);
void         SaAbreArquivoConfiguracao(char* Nome);
void 				 SaPrintResultado(StSaSolucao* pSolucao);
void         SaGravaSolucaoBest(StSaSolucao* solucao);
void         SaDesalocaSolucao(StSaSolucao *pSolucao);
void         SaCriaSolucaoAleatoria(StSaSolucao *pSolucao);
void         SaLogResultado(StSaSolucao* pSolucao, char* Nome);
void         SaEstimaBusloadViaSimulacao(StSaSolucao* pSolucao);
void         SaDbgExibeSolucao(StSaSolucao* pSolucao, char* pNome);
void         SaClonaSolucao(StSaSolucao *pCopia, StSaSolucao *pBase);
void         SaPerturbaSolucaoVizinhancaUniforme(StSaSolucao* pSolucao);
void   			 start_can_simulated(char* path_arq, double time_simulation);
void         SaGravaSolucaoCurrent(StSaSolucao* pSolucao, u_int32_t iterador);
u_int8_t     getIndexOfPrimeLesserThan(u_int8_t value);
u_int8_t 		 main_simulated_annealing(u_int8_t argc, char *argv[]);
u_int16_t    SaSelecionaSlotUniforme(StSaSolucao* pSolucao);
u_int16_t    SaSelecionaSlotProporcionalAoID(StSaSolucao* pSolucao);
StSaSolucao* SaAlocaSolucao(void);
double       SaCalculaObjetiva(StSaSolucao* pSolucao);

#endif
