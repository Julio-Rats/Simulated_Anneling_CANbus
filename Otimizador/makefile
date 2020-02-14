CC     = gcc
CFLAGS = -O3
EXE    = otimizador
LIB    = -lm

BINDIR = ../Bin
SIMDIR = ../src/Simulador\ deterministico
SMADIR = ../src/Simulated\ Annealing
MAIDIR = ../src/Interface

all: fifo.c get_candb.c simulador.c  simulated.c  timer.c main.c
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

main.c:
	gcc -c $(MAIDIR)/$@ -o $(BINDIR)/main.o $(CFLAGS)

clean:
	rm $(BINDIR)/*.o
