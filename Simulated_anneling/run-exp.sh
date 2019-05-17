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
ARQ_CONFIG="config.01"
ARQ_TMP_ODBC="tempos.dat"
#ARQ_LOG_CANOE="log-busload.dat"   #"log.dat"
ARQ_RESULTADOS="saida.dat"
ARQ_LOGOBJ='LogOBJ.dat'
ARQ_BEST='LogBEST.dat'

# Recompila o codigo
make clean
make


data=$(date +%s)

# Executa a aplicacao a quantidade de repeticoes especificada
CONTADOR=0
until [ $CONTADOR -ge $NUM_REPETICOES ];
do
	echo "Execucao No. $CONTADOR"
	echo
	./simulated $ARQ_CONFIG $ARQ_TMP_ODBC $data-$ARQ_RESULTADOS $data-$ARQ_LOGOBJ $data-$ARQ_BEST
	let CONTADOR=CONTADOR+1
done

