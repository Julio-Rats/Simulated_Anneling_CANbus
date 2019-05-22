# Parametros

# Horizonte de tempo, em ms
HORIZONTE <- 100
TAMJANELA <- 25

# Le os argumentos em linha de comando
Comando <- commandArgs(TRUE)

if (length(Comando) != 1)
{
	cat('\nErro: Sintaxe Incorreta. Usar:\n\nRscript plot-msg.r <arquivo-entrada>\n\n')
	quit(save='no')
} else {
	Entrada <- Comando[1]
}

# Le o arquivo de entrada contendo o ID, Periodo, Offset e Duracao da Mensagem
DataMsg <- read.table(file=Entrada, header=TRUE)

# Seta a semente do gerador de numeros aleatorios para sempre gerar mesma sequencia de 
# cores como resultado do 'sample'
set.seed(1024)

# Cria a paleta de cores de acordo com o numero de linhas
IdsLst  <- unique(DataMsg$ID)
NumMsg  <- length(IdsLst)
Cores   <- sample(rainbow(NumMsg), NumMsg)

# Hash de cores
Hash    <- data.frame(unique(DataMsg$ID), Cores)

# Funcao ineficiente para buscar a cor a usar para cada mensagem
GetColor <- function(Id)
{
	for (i in 1:length(Hash[,1]))
	{
		if(Hash[i,1] == Id)
		{
			return(as.character(Hash[i,2]))
		}
	}
}

# Para cada mensagem, desenha a mensagem no tempo até o limite maximo de horizonte 
# que iremos logar
IdxMsg <- 1

# Para cada intervalo de TAMJANELA ms
for(JanelaTempo in seq(0, HORIZONTE, TAMJANELA))
{
	# Grava o plot da mensagem
	Titulo <- paste('Mensagem no Tempo [', JanelaTempo, ' .. ', JanelaTempo+TAMJANELA, ']', sep='')
	png(file=paste(Entrada,'-mensagens-', JanelaTempo, '.png', sep=''), width=2000)
	plot(c(JanelaTempo, JanelaTempo+TAMJANELA), c(0, 2), type='n', main=Titulo, xlab='Tempo', ylab='')

	repeat
	{
		# Replica um retangulo para a mensagem, do inicio da janela até o final
		Id      <- DataMsg[IdxMsg,]$ID
		Duracao <- DataMsg[IdxMsg,]$DURATION
		Tempo   <- DataMsg[IdxMsg,]$TIMESTAMP

		# Determina a cor de preenchimento da mensagem
		CorMsg  <- GetColor(Id)

		# Se a janela de tempo for muito grande, melhor usar mesma cor para linha
		if(TAMJANELA > 100)
		{	
			# Define a cor com base na paleta
			CorLinha <- CorMsg
		} else {
			CorLinha <- 'black'
		}

		# Desenha a barra que representa a ocupação da mensagem no barramento ao longo
		# do tempo de simulação
		polygon(c(Tempo, Tempo, Tempo+Duracao, Tempo+Duracao, Tempo), 
                c(0, 2, 2, 0, 0), col=CorMsg, border=CorLinha)

		# Se o tempo da mensagem atual extrapola o tempo
		if(Tempo > JanelaTempo+TAMJANELA) { break }

		# Passa para a proxima mensagem
		IdxMsg <- IdxMsg + 1
	}

	# Desliga o plot
	dev.off()
}
