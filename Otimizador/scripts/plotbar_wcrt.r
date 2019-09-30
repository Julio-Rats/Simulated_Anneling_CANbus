argv <- commandArgs(trailingOnly = TRUE)

png(paste(argv[1],"png",sep="."))
	
dados <- read.table(file=argv[1], header=T)
a <- barplot(dados$WCRT, dados$ID, xlab="ID Prioridades", ylab="WCRT (ms)")
axis(1, at = a, label = dados$ID)

dev.off()
