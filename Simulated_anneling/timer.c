/***********************************************************************************************/
/*                                                                                             */
/* TIMER.C                                                                                     */
/*                                                                                             */
/* Implementa metodos para contabilizar o tempo de processamento de trechos de codigo          */
/*                                                                                             */
/***********************************************************************************************/
#include "timer.h"
/***********************************************************************************************/
/*                                                                                             */
/* Inicializa os temporizadores                                                                */
/*                                                                                             */
/***********************************************************************************************/
void TmrInicia(void)
{
	int i;

	/* Varre todos os temporizadores, inicializando-os */
	for(i = 0; i < TMR_MAX_TEMPORIZADORES; i++)
	{
		/* Formata os temporizadores, para uso posterior */
		TmrTemporizadores[i].Id            = i;
		TmrTemporizadores[i].EmUso         = FALSE;
		TmrTemporizadores[i].Reservado     = FALSE;
		TmrTemporizadores[i].Total         = 0.0;
		TmrTemporizadores[i].Parcial       = FLT_MIN;
		TmrTemporizadores[i].UltimaLeitura = FLT_MIN;
		strcpy(TmrTemporizadores[i].Descricao, "Nao Inicializado");
	}
}

/***********************************************************************************************/
/*                                                                                             */
/* Aloca um temporizador para uso                                                              */
/*                                                                                             */
/***********************************************************************************************/
void TmrAlocaTemporizador(int Timer, BYTE Status, char *pDescricao)
{
	/* Verifica se o temporizador e' valido */
	if(Timer < 0 || Timer >= TMR_MAX_TEMPORIZADORES)
	{
		printf("\n\nErro: tentando alocar temporizador fora dos limites (id = %d)\n", Timer);
		exit(1000);
	}

	/* Verifica se o temporizado ja' se encontra reservado por alguem */
	if(TmrTemporizadores[Timer].Reservado == TRUE)
	{
		printf("\n\nErro: tentando alocar temporizador ja' reservado (id = %d)\n", Timer);
		exit(1000);
	}

	/* Reserva o temporizador para uso */
	TmrTemporizadores[Timer].Reservado = TRUE;
	TmrTemporizadores[Timer].EmUso     = Status;
	strcpy(TmrTemporizadores[Timer].Descricao, pDescricao);
}

/***********************************************************************************************/
/*                                                                                             */
/* Inicia contagem de tempo neste temporizador                                                 */
/*                                                                                             */
/***********************************************************************************************/
void TmrStart(int Timer)
{
	/* Verifica se o temporizador e' valido */
	if(Timer < 0 || Timer >= TMR_MAX_TEMPORIZADORES)
	{
		printf("\n\nErro: tentando iniciar temporizador fora dos limites (id = %d)\n", Timer);
		exit(1000);
	}

	/* Verifica se o temporizado ja' se encontra reservado por alguem */
	if(TmrTemporizadores[Timer].Reservado == FALSE)
	{
		printf("\n\nErro: tentando iniciar temporizador nao reservado (id = %d)\n", Timer);
		exit(1000);
	}

	/* Verifica se o temporizado ja' se encontra iniciado via start */
	if(TmrTemporizadores[Timer].EmUso == TRUE)
	{
		/* Marca o timestamp de inicio para o timer informado */
		TmrTemporizadores[Timer].Parcial       = TmrTimeStamp();
		TmrTemporizadores[Timer].UltimaLeitura = FLT_MIN;
	}
}


/***********************************************************************************************/
/*                                                                                             */
/* Finaliza contagem de tempo neste temporizador                                               */
/*                                                                                             */
/***********************************************************************************************/
void TmrStop(int Timer)
{
	/* Verifica se o temporizador e' valido */
	if(Timer < 0 || Timer >= TMR_MAX_TEMPORIZADORES)
	{
		printf("\n\nErro: tentando iniciar temporizador fora dos limites (id = %d)\n", Timer);
		exit(1000);
	}

	/* Verifica se o temporizado ja' se encontra reservado por alguem */
	if(TmrTemporizadores[Timer].Reservado == FALSE)
	{
		printf("\n\nErro: tentando iniciar temporizador nao reservado (id = %d)\n", Timer);
		exit(1000);
	}

	/* Verifica se o temporizador esta' em uso, contando tempo */
	if(TmrTemporizadores[Timer].EmUso == TRUE)
	{
		/* Marca o timestamp de inicio para o timer informado */
		TmrTemporizadores[Timer].UltimaLeitura = TmrTimeStamp() - TmrTemporizadores[Timer].Parcial;
		TmrTemporizadores[Timer].Total         = TmrTemporizadores[Timer].Total + TmrTemporizadores[Timer].UltimaLeitura;
		TmrTemporizadores[Timer].Parcial       = FLT_MIN;
	}
}

/***********************************************************************************************/
/*                                                                                             */
/* Retorna o 'timestamp' durante a execucao do codigo. A diferenca entre dois timestamps que   */
/* sejam consecutivos retorna o tempo gasto no trecho.                                         */
/*                                                                                             */
/***********************************************************************************************/
float TmrTimeStamp(void)
{
 	struct rusage r;

	/* Obtem estrutura de dados que contem o tempo atual */
	getrusage(0, &r);

	/* Retorna a quantidade de segundos */
	return (float)(r.ru_utime.tv_sec + r.ru_utime.tv_usec / (float)1000000);
}

/***********************************************************************************************/
/*                                                                                             */
/* Obtem o valor acumulado de tempos para o temporizador informado                             */
/*                                                                                             */
/***********************************************************************************************/
float TmrObtemTemporizador(int Timer)
{
	/* Verifica se o temporizador e' valido */
	if(Timer < 0 || Timer >= TMR_MAX_TEMPORIZADORES)
	{
		printf("\n\nErro: tentando iniciar temporizador fora dos limites (id = %d)\n", Timer);
		exit(1000);
	}

	/* Verifica se o temporizado ja' se encontra reservado por alguem */
	if(TmrTemporizadores[Timer].Reservado == FALSE)
	{
		printf("\n\nErro: tentando iniciar temporizador nao reservado (id = %d)\n", Timer);
		exit(1000);
	}

	/* Retorna o temporizador acumulado */
	return(TmrTemporizadores[Timer].Total);
}

/***********************************************************************************************/
/*                                                                                             */
/* Imprime o valor dos timers acumulados, apenas para depuracao                                */
/*                                                                                             */
/***********************************************************************************************/
void TmrPrintTemporizadores(void)
{
	int i;

	for(i = 0; i < TMR_MAX_TEMPORIZADORES; i++)
	{
		if(TmrTemporizadores[i].Reservado && TmrTemporizadores[i].EmUso)
		{
			printf("\n\n");
			printf("Descricao   : %s (%d)\n", TmrTemporizadores[i].Descricao, TmrTemporizadores[i].Id);
			printf("EmUso       : %s\n", ((TmrTemporizadores[i].EmUso) ? "SIM" : "NAO"));
			printf("Total       : %f\n", TmrTemporizadores[i].Total);
			printf("Parcial     : %f\n", TmrTemporizadores[i].Parcial);
			printf("Ult. Leitura: %f\n", TmrTemporizadores[i].UltimaLeitura);
		}
	}
}
