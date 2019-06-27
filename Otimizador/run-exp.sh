##########################################################################
#                                                                        #
# run-exp.sh                                                             #
#                                                                        #
# Executa o experimento do simulated annealing com o CANOe com a quanti- #
# -dade de repeticoes especificadas. Observe que o arquivo de saida ira  #
# ser concatenado a cada resultado, para inspecao final.                 #
#                                                                        #
##########################################################################

# Quantidade de repeticoes desejada
NUM_REPETICOES=1

# Nome dos arquivos de entrada. Ajuste para customizar o script
ARQ_CONFIG="config.txt"
ARQ_TMP_ODBC="Tempos.dat"
ARQ_RESULTADOS="Result.dat"
ARQ_LOGOBJ='LogOBJ.dat'
ARQ_BEST='LogBEST'

Path_Bin='../Bin'
Path_Result='Results'
Path_LOGOBJ='LogOBJ'
Path_BEST='LogBest'

# Executa a aplicacao a quantidade de repeticoes especificada
CONTADOR=0
until [ $CONTADOR -ge $NUM_REPETICOES ];
do
	data=$(date +%s)
	echo "Execucao No. $CONTADOR"
	echo
	$Path_Bin/otimizador $ARQ_CONFIG $ARQ_TMP_ODBC $Path_Result/$data-$ARQ_RESULTADOS $Path_LOGOBJ/$data-$ARQ_LOGOBJ $Path_BEST/$data-$ARQ_BEST
	# $Path_Bin/otimizador $ARQ_CONFIG $ARQ_TMP_ODBC $Path_Result/$data-$ARQ_RESULTADOS $Path_LOGOBJ/$data-$ARQ_LOGOBJ
	#$Path_Bin/otimizador $ARQ_CONFIG $ARQ_TMP_ODBC $Path_Result/$data-$ARQ_RESULTADOS $Path_BEST/$data-$ARQ_BEST
	# $Path_Bin/otimizador $ARQ_CONFIG $ARQ_TMP_ODBC $Path_Result/$data-$ARQ_RESULTADOS
	let CONTADOR=CONTADOR+1
done
