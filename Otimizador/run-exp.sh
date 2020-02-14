##########################################################################
#                                                                        #
# run-exp.sh                                                             #
#                                                                        #
# Executa o experimento do simulated annealing com o Simulador com 			 #
# 		quantidade de repeticoes especificadas. 												   #
#                                                                        #
##########################################################################

#Verifica argumentos
if [ $# -lt 2 ]; then
	 echo -e "\nSintaxe de uso do script:\n\n\tsh $0 [Input] [Nº Replicações]\n"
	 echo -e "\nArgumentos:\n\n"
	 echo -e "[Input]: Arquivo DBC para simulação ou otimização."
	 echo -e "[Nº Replicações]: Numero de vezes que o script ira executar o processo principal.\n"
	 exit
fi

# Quantidade de repeticoes desejada
NUM_REPETICOES=$2

# Nome dos arquivos de entrada. Ajuste para customizar o script
ARQ_CONFIG='config_parametros.txt'
ARQ_RESULTADOS='result_parametros.dat'
ARQ_LOGOBJ='LogOBJ.dat'
ARQ_BEST='LogBEST'
ARQ_WCRT='LogWCRT'

Path_Bin='../Bin'
Path_Result='Results/resultados/sae_23'
Path_LOGOBJ='LogOBJ'
Path_BEST='LogBest'
Path_wcrt='wcrt_ids'

# Executa a aplicacao a quantidade de repeticoes especificada
CONTADOR=1
until [ $CONTADOR -gt $NUM_REPETICOES ];
do
	data=$(date +%s)
	echo -e "\nExecução Nº: $CONTADOR de $NUM_REPETICOES Execuções \t Time Start = $data"
	echo

	$Path_Bin/otimizador -c $ARQ_CONFIG -d $1 -r $Path_Result/$1-$data-$ARQ_RESULTADOS

	CONTADOR=$(($CONTADOR+1))
done
