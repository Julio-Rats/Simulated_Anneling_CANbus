##########################################################################
#                                                                        #
# run-exp.sh                                                             #
#                                                                        #
# Executa o experimento do simulated annealing com o CANOe com a quanti- #
# -dade de repeticoes especificadas. Observe que o arquivo de saida ira  #
# ser concatenado a cada resultado, para inspecao final.                 #
#                                                                        #
##########################################################################

#Verifica argumentos
if [ $# -lt 1 ]; then
	 echo -e "\nSintaxe de uso do script:\n\n\tsh $0 [input]"
	 echo -e "\ninput: Arquivo DBC para simulação ou otimização.\n"
	 exit
fi

# Quantidade de repeticoes desejada
NUM_REPETICOES=1

# Nome dos arquivos de entrada. Ajuste para customizar o script
ARQ_CONFIG="config_atual.txt"
ARQ_TMP_ODBC="Tempos.dat"
ARQ_RESULTADOS="Result.dat"
ARQ_LOGOBJ='LogOBJ.dat'
ARQ_BEST='LogBEST'
ARQ_WCRT='LogWCRT'

Path_Bin='../Bin'
Path_Result='Results'
Path_LOGOBJ='LogOBJ'
Path_BEST='LogBest'
Path_wcrt='wcrt_ids'

# Executa a aplicacao a quantidade de repeticoes especificada
CONTADOR=1
until [ $CONTADOR -gt $NUM_REPETICOES ];
do
	data=$(date +%s)
	echo "Execucao No = $CONTADOR       Time Start = $data"
	echo

	mkdir $Path_Result/$data/
	mkdir $Path_LOGOBJ/$data/
	mkdir $Path_BEST/$data/
	mkdir $Path_wcrt/$data/
	#
	$Path_Bin/otimizador -c $ARQ_CONFIG -d $1 -r $Path_Result/$data/$data-$ARQ_RESULTADOS \
	 						-g $Path_LOGOBJ/$data/$data-$ARQ_LOGOBJ -w $Path_wcrt/$data/$data-$ARQ_WCRT
	let CONTADOR++
done
