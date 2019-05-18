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
#ARQ_LOG_CANOE="log-busload.dat"   #"log.dat"
ARQ_RESULTADOS="Saida.dat"
ARQ_LOGOBJ='LogOBJ.dat'
Path_LOGOBJ='LogOBJ/'
ARQ_BEST='LogBEST.dat'
Path_BEST='LogBest/'

# Recompila o codigo
make clean
make
make clean

Rdata=$(date +%s)

# Executa a aplicacao a quantidade de repeticoes especificada
CONTADOR=0
until [ $CONTADOR -ge $NUM_REPETICOES ];
do
	data=$(date +%s)
	echo "Execucao No. $CONTADOR"
	echo
	./simulated $ARQ_CONFIG $ARQ_TMP_ODBC $Rdata-$ARQ_RESULTADOS $Path_LOGOBJ$data-$ARQ_LOGOBJ $Path_BEST$data-$ARQ_BEST
	let CONTADOR=CONTADOR+1
done
