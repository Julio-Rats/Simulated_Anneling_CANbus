CC     = gcc
CFLAGS = -O3
EXE    = otimizador
LIB    = -lm

BINDIR = ../Bin
SIMDIR = ../src/Simulador\ deterministico
SMADIR = ../src/Simulated\ Anneling
DRVDIR = ../src/Driver

all: fifo.c get_candb.c simulador.c  simulated.c  timer.c driver.c
	$(CC) -o $(BINDIR)/$(EXE) $(BINDIR)/*.o  $(CFLAGS) $(LIB)

fifo.c:
	gcc -c $(SIMDIR)/$@ -o $(BINDIR)/fifo.o $(CFLAGS)

get_candb.c:
	gcc -c $(SIMDIR)/$@ -o $(BINDIR)/get_candb.o $(CFLAGS)

simulador.c:
	gcc -c $(SIMDIR)/$@ -o $(BINDIR)/simulador.o $(CFLAGS)

simulated.c:
	gcc -c $(SMADIR)/$@ -o $(BINDIR)/simulated.o $(CFLAGS)

timer.c:
	gcc -c $(SMADIR)/$@ -o $(BINDIR)/timer.o $(CFLAGS)

driver.c:
	gcc -c $(DRVDIR)/$@ -o $(BINDIR)/driver.o $(CFLAGS)

clean:
	rm $(BINDIR)/*.o $(EXE)
