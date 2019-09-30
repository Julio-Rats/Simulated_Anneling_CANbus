#ifndef SIMUL_SA
#define  SIMUL_SA

/*****************************************************************************************/
/*                                                                                       */
/* simulated.h                                                                           */
/*                                                                                       */
/* Implementa o método Simulated Annealing para ajuste de temporização de ciclo e delay  */
/* em arquivos ODBC do CANOe                                                             */
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

// #define TIME_CAN_SIMULATED         120000  /* Tempo que o simulador ira emular*/
#define PORC_START_DELAY           1       /* Maior porpoção escolhida para StartDelay*/
#define ESCALAR_WCRT               1       /* Escalar F()Objetiva para WCRT*/
#define ESCALAR_QUEUE              0       /* Escalar F()Objetiva para tempo max da fila*/
#define ESCALAR_DELAY              0       /* Escalar F()Objetiva para StartDelay*/

u_int8_t LIMIT_DELAY = 7;       /* Tempo minimo para um StartDelay */
/* Habilita ou desabilita os modos verbose */
bool SA_VERBOSE      = FALSE;
bool SA_VERBOSE_PROB = FALSE;

/* Habilita somente simulação*/
bool SA_ONLY_SIM     = FALSE;


/* Habilita escrita em arquivos de Logs */
bool SA_GRAVA_BEST = FALSE;
bool SA_GRAVA_OBJ  = FALSE;
bool SA_GRAVA_WCRT = FALSE;

/* Habilita limite minimo de StartDelay*/
bool SET_LIMIT_DELAY = FALSE;

/* Habilita ou desabilita o modo de gravação do vizinho */
#define SA_TIMER                   0
#define SA_TIMER_SIMULATED         0


/*****************************************************************************************/
/*                                                                                       */
/*  Defines para busca e perturbação dos vizinhos                                         */
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
/*  Codigos de erro de retorno para o sistema operacional, para uso com shell scripting  */
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

/*****************************************************************************************/
/*                                                                                       */
/*                                Variaveis Globais                                      */
/*                                                                                       */
/*****************************************************************************************/
/* Strings com os nomes dos arquivos informados via linha de comando */
char SaArqConfiguracao[SA_MAX_CHAR_COMMAND_LINE];
char SaArqTempos[SA_MAX_CHAR_COMMAND_LINE];
char SaArqSaida[SA_MAX_CHAR_COMMAND_LINE];
char SaArqLogEvolOBJ[SA_MAX_CHAR_COMMAND_LINE];
char SaArqBest[SA_MAX_CHAR_COMMAND_LINE];
char SaSimFile[SA_MAX_CHAR_COMMAND_LINE];
char SaCANDBFile[SA_MAX_CHAR_COMMAND_LINE];
char SaWCRTFile[SA_MAX_CHAR_COMMAND_LINE];

char path_file_best[SA_MAX_CHAR_COMMAND_LINE+5];
char path_file_wcrt[SA_MAX_CHAR_COMMAND_LINE+5];
char temp_time[SA_MAX_CHAR_COMMAND_LINE] = "/tmp/sa_time.txt";
/* Armazenam parametros sobre cada mensagem */
typedef struct
{
	u_int16_t Id;	       /* ID da mensagem   */
	double    MaxCiclo;  /* Intermitencia maxima da mensagem, em ms */
	char*     pNome;     /* String com nome da mensagem */
}StSaMsgPar;

/* Slot com proposta de temporizacao para uma mensagem CAN */
typedef struct
{
	u_int16_t Id:11;	     /* ID da mensagem   */
	double    TamCiclo;    /* Tamanho do ciclo atual */
	double    StartDelay;  /* Valor de offset inicial proposto */
	double    deadline_time;  /* Valor do  tempo de deadline */
	u_int8_t  payload;  /* Valor do  tempo de deadline */
	double    ProbSelecao; /* Probabilidade de selecao da mensagem caso a perturbacao seja proporcional ao ID */
}StSaMsgTmrSlot;

typedef struct
{
	double          WCRT;        /* Valor da busload associado com a temporizacao proposta */
	double          burst_time;  /* Tempo maximo de fila dessa solução*/
	double          burst_size;     /* busload dessa solução*/
	StSaMsgTmrSlot* pSol;        /* Vetor de slots com parametros propostos para solucao */
}StSaSolucao;

double TIME_CAN_SIMULATED = 120000;

/* Array de parametros de mensagens */
StSaMsgTmrSlot* pSaMsgParArray 				 = NULL;

/* Numero de mensagens consideradas pelo arquivo de configuracao */
u_int16_t   SaNumMsgCan    				 = 0L;

/* Solucoes usadas durante o processamento do Simulated Annealing */
StSaSolucao* pSaMelhor             = NULL;
StSaSolucao* pSaCorrente           = NULL;
StSaSolucao* pSaVizinho            = NULL;
StSaSolucao* pSaMelhorVizinho      = NULL;

/* Parametros 'default' para funcionamento do metodo Simulated Annealing */
u_int16_t    SaNumVizinhos         = 50L;
double       SaTempInicial         = 100.0;
double       SaTempFinal           = 20.0;
double       SaAlpha               = 0.95;
u_int16_t    SaNumSlotsPerturbacao = 1L;

//(Julio)
u_int16_t    SaNumIteracao         = 1;
u_int8_t     SaNumReaquecimento    = 0;
u_int8_t     SaMetodoBusca         = SA_CNF_SELECAO_PROPORCIONAL_ID;
u_int8_t     SaMetodoPert          = SA_CNF_PERT_INCRIMENT;
u_int8_t     SaMetodoInicial       = SA_CNF_START_ZERO;

/*Descritores de arquvos, para os logs, modos verbose*/
FILE*    	   Arq_OBJ                = NULL;
FILE*    	   Arq_Best               = NULL;
FILE*    	   Arq_WCRT               = NULL;

static u_int32_t log_frame          = 1;
u_int32_t        cont_wcrt          = 1;

/* Controla posicoes sorteadas pelo SA */
u_int8_t* pSaBitPosicao;

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
u_int16_t    SaSelecionaSlotUniforme(StSaSolucao* pSolucao);
u_int16_t    SaSelecionaSlotProporcionalAoID(StSaSolucao* pSolucao);
StSaSolucao* SaAlocaSolucao(void);
double       SaCalculaObjetiva(StSaSolucao* pSolucao);

#endif
