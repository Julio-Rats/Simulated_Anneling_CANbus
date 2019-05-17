all: simulated
	@echo ""
	@echo ""
	@echo "***************************************************************"
	@echo "*                                                             *"
	@echo "* simulated:                                                  *"
	@echo "* Compilacao efetuada com sucesso. Verifique as mensagens     *"
	@echo "* acima para warnings e outras informacoes do GCC.            *"
	@echo "*                                                             *"
	@echo "***************************************************************"
	@echo ""
	@echo ""

simulated: simulated.o timer.o driver.o
	gcc -O3 ../deterministic_sim_CAN/*.o simulated.o timer.o driver.o -o simulated -lm

simulated.o:
	gcc -c -O3 simulated.c

timer.o: timer.h timer.c
	gcc -c -O3 timer.c

driver.o:
	gcc -c -O3 ../Driver/driver.c

clean:
	rm *.o
	rm simulated
	@echo ""
	@echo ""
	@echo "***************************************************************"
	@echo "*                                                             *"
	@echo "* Removendo os arquivos .o gerados na compilacao do codigo    *"
	@echo "*                                                             *"
	@echo "***************************************************************"
	@echo ""
	@echo "Restaram os seguintes arquivos no diretorio:"
	@ls -la
	@echo ""
