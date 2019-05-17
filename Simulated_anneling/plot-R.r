args <- commandArgs(trailingOnly = TRUE)

dados <- read.table(file=args[1])

png(filename=args[2], width = 1200, height = 500)
plot(dados$V1, dados$V2, type='l', xlab='Iterações', ylab='Funcao Objetiva')
dev.off()
