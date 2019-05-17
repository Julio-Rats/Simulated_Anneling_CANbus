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
#include "../Driver/driver.h"


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
#define SA_ID_NONE                 ULONG_MAX

/* Constantes para verdadeiro ou falso */
#define TRUE                       1
#define FALSE                      0

/*****************************************************************************************/
/*                                                                                       */
/*  Configuracoes especiais de modulos que podem ser habilitados em tempo de compilacao  */
/*  e parametros associados                                                              */
/*                                                                                       */
/*****************************************************************************************/
//(Julio)
#define TIME_CAN_SIMULATED         120000  /* Tempo que o simulador ira emular*/
#define RAND_START_DELAY           1       /* Maior porpoção escolhida para StartDelay*/
#define ESCALAR_WCRT               1000    /* Escalar F()Objetiva para WCRT*/
#define ESCALAR_QUEUE              1000    /* Escalar F()Objetiva para tempo max da fila*/
#define ESCALAR_DELAY              10      /* Escalar F()Objetiva para StartDelay*/
//

/* Habilita ou desabilita o modo verbose */
#define SA_VERBOSE                 FALSE
#define SA_VERBOSE_PROB            FALSE
#define SA_VERBOSE_IO              TRUE

/* Habilita ou desabilita o modo de gravação do vizinho */
#define SA_TIMER                   0
#define SA_TIMER_SIMULATED         0

#define SA_LOG_ITERADOR            1

/*****************************************************************************************/
/*                                                                                       */
/*  Codigos de erro de retorno para o sistema operacional, para uso com shell scripting  */
/*                                                                                       */
/*****************************************************************************************/

/* Codigo de erros para as operacoes criticas realizadas pela aplicacao */
#define SA_ERRO_NONE               0
#define SA_ERRO_IO                 1
#define SA_ERRO_MEMORIA            2
#define SA_ERRO_ARGS_INVALIDOS     3
#define SA_ERRO_PARSING_CONFIG     4
#define SA_ERRO_NO_VALUE           5
#define SA_ERRO_ESTADO             6
#define SA_ERRO_CONFIGURACAO       7

/*****************************************************************************************/
/*                                                                                       */
/*                                Variaveis Globais                                      */
/*                                                                                       */
/*****************************************************************************************/

/* Arranjo com os numeros primos menores que 1000 */
#define PRIMES_SIZE 169
const int primes[PRIMES_SIZE] = {2,3,5,7,11,13,17,19,23,29,31,37,41,43,47,53,59,61,67,71,73,79,83,89,97,101,103,107,109,113,127,131,137,139,149,151,157,163,167,173,179,181,191,193,197,199,211,223,227,229,233,239,241,251,257,263,269,271,277,281,283,293,307,311,313,317,331,337,347,349,353,359,367,373,379,383,389,397,401,409,419,421,431,433,439,443,449,457,461,463,467,479,487,491,499,503,509,521,523,541,547,557,563,569,571,577,587,593,599,601,607,613,617,619,631,641,643,647,653,659,661,673,677,683,691,701,709,719,727,733,739,743,751,757,761,769,773,787,797,809,811,821,823,827,829,839,853,857,859,863,877,881,883,887,907,911,919,929,937,941,947,953,967,971,977,983,991,997,1009};

/* Strings com os nomes dos arquivos informados via linha de comando */
char SaArqConfiguracao[SA_MAX_CHAR_COMMAND_LINE];
char SaArqTempos[SA_MAX_CHAR_COMMAND_LINE];
char SaArqSaida[SA_MAX_CHAR_COMMAND_LINE];
char SaArqLogEvolOBJ[SA_MAX_CHAR_COMMAND_LINE];
char SaArqBest[SA_MAX_CHAR_COMMAND_LINE];

/* Armazenam parametros sobre cada mensagem */
typedef struct
{
	u_int16_t Id;	       /* ID da mensagem   */
	double    MaxCiclo;  /* Intermitencia maxima da mensagem, em ms */
	char*     pNome;     /* String com nome da mensagem */
}StSaMsgPar;

/* Array de parametros de mensagens */
StSaMsgPar *pSaMsgParArray = NULL;

/* Numero de mensagens consideradas pelo arquivo de configuracao */
u_int16_t SaNumMsgCan = 0L;

/* Slot com proposta de temporizacao para uma mensagem CAN */
typedef struct
{
	u_int16_t Id;	         /* ID da mensagem   */
	double    TamCiclo;    /* Tamanho do ciclo atual */
	double    StartDelay;  /* Valor de offset inicial proposto */
	double    ProbSelecao; /* Probabilidade de selecao da mensagem caso a perturbacao seja proporcional ao ID */
}StSaMsgTmrSlot;

/* Estrutura de dados de uma solucao */
typedef struct
{
	u_int16_t       NumMsg;     /* Numero de mensagens CAN contidas na solucao */
	double          WCRT;       /* Valor da busload associado com a temporizacao proposta */
	double          Time_Queue; /* Tempo maximo de fila dessa solução*/
	StSaMsgTmrSlot* pSol;       /* Vetor de slots com parametros propostos para solucao */
}StSaSolucao;

/* Solucoes usadas durante o processamento do Simulated Annealing */
StSaSolucao *pSaMelhor             = NULL;
StSaSolucao *pSaCorrente           = NULL;
StSaSolucao *pSaVizinho            = NULL;
StSaSolucao *pSaMelhorVizinho      = NULL;

/* Parametros 'default' para funcionamento do metodo Simulated Annealing */
u_int16_t    SaNumVizinhos         = 50L;
double       SaTempInicial         = 100.0;
double       SaTempFinal           = 20.0;
double       SaAlpha               = 0.95;
u_int16_t    SaNumSlotsPerturbacao = 2L;
//(Julio)
u_int8_t SaNumIteracao             = 1;
u_int8_t SaNumReaquecimento        = 3;
FILE*    Arq                       = NULL;
FILE*    ArqBest                   = NULL;

//

/* Controla posicoes sorteadas pelo SA */
u_int8_t *pSaBitPosicao;

/*****************************************************************************************/
/*                                                                                       */
/*         Prototipos e implementacao das funcoes do modulo de ajuste de tempos          */
/*                                                                                       */
/*****************************************************************************************/
void SaLiberaMemoria(void);
void SaDbgPrintParametros(void);
void SaAbreArquivoConfiguracao(char* Nome);
void SaDbgExibeSolucao(StSaSolucao* pSolucao, char* pNome);
void SaDefineParametros(u_int16_t NumVizinhos, double TempInicial, double TempFinal, double Alpha, u_int16_t NumTrocas);
void SaDesalocaSolucao(StSaSolucao *pSolucao);
void SaClonaSolucao(StSaSolucao *pCopia, StSaSolucao *pBase);
void SaCriaSolucaoAleatoria(StSaSolucao *pSolucao);
void SaCriaSolucaoCicloInformado(StSaSolucao* pSolucao);
void SaPerturbaSolucaoVizinhancaUniforme(StSaSolucao* pSolucao);
void SaGeraArquivoEntradaParaODBC(char* pSaida, StSaSolucao* pSolucao);
void SaEstimaBusloadViaSimulacao(StSaSolucao* pSolucao);
void SaLogResultado(StSaSolucao* pSolucao, char* Nome);
void SaDefineParametros(u_int16_t NumVizinhos, double TempInicial, double TempFinal, double Alpha, u_int16_t NumTrocas);
void SaSimulatedAnnealing(void);
u_int16_t SaSelecionaSlotProporcionalAoID(StSaSolucao* pSolucao);
u_int16_t SaSelecionaMaiorCiclo(StSaSolucao *pSolucao);
u_int16_t SaSelecionaSlotUniforme(StSaSolucao* pSolucao);
u_int8_t getIndexOfPrimeLesserThan(u_int8_t value);
StSaSolucao* SaAlocaSolucao(void);
//(Julio)
double SaCalculaObjetiva(StSaSolucao* pSolucao);
void   SaGravaSolucaoCurrent(StSaSolucao* pSolucao, u_int32_t iterador);
//
#endif
