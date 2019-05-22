/***********************************************************************************************/
/*                                                                                             */
/* TIMER.h                                                                                 */
/*                                                                                             */
/* Implementa metodos para contabilizar o tempo de processamento de trechos de codigo          */
/*                                                                                             */
/***********************************************************************************************/

#ifndef TIMER_H
#define TIMER_H

#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/resource.h>

#define   TRUE  1
#define   FALSE 0

/***********************************************************************************************/
/*                                                                                             */
/* Tipos de dados definido pelo usuario                                                        */
/*                                                                                             */
/***********************************************************************************************/

typedef unsigned char           BYTE;

/***********************************************************************************************/
/*                                                                                             */
/* Constantes do modulo                                                                        */
/*                                                                                             */
/***********************************************************************************************/

/* Numero maximo de temporizadores */
#define TMR_MAX_TEMPORIZADORES  10

/***********************************************************************************************/
/*                                                                                             */
/* Estrutura de dados de controle                                                              */
/*                                                                                             */
/***********************************************************************************************/
typedef struct
{
	int   Id;             /* Identificacao do temporizador                            */
	BYTE  EmUso;          /* Indica que o temporizador esta' em processo de contagem */
	BYTE  Reservado;      /* Indica que o temporizador esta' reservado               */
	char  Descricao[80];  /* Texto descritivo                                        */
	float  Total;          /* Totaliza os tempos                                      */
	float  Parcial;        /* Timestamp parcial usado nos calculos                    */
	float  UltimaLeitura;  /* Ultima leitura de tempo via mecanismo start/stop        */
}TmrStTemporizador;

/***********************************************************************************************/
/*                                                                                             */
/* Variaveis Globais                                                                           */
/*                                                                                             */
/***********************************************************************************************/
TmrStTemporizador TmrTemporizadores[TMR_MAX_TEMPORIZADORES];

/***********************************************************************************************/
/*                                                                                             */
/* Funcoes externas que podem ser usadas por outros modulos                                    */
/*                                                                                             */
/***********************************************************************************************/

/* Inicia os temporizadores */
void  TmrInicia(void);

/* Retorna o 'timestamp' durante a execucao do codigo. A diferenca entre dois timestamps que   */
/* sejam consecutivos retorna o tempo gasto no trecho.                                         */
float TmrTimeStamp(void);

/* Gerencia de temporizadores */
void  TmrAlocaTemporizador(int Timer, BYTE Status, char *pDescricao);
void  TmrStart(int Timer);
void  TmrStop(int Timer);
float  TmrObtemTemporizador(int Timer);
void  TmrPrintTemporizadores(void);

#endif
