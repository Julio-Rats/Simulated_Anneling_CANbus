for file in $(ls *.dat)
do
	echo $file
	Rscript plot-msg-log.r $file && convert -append ${file}-mensagens-*.png $file.png
	rm -f ${file}-mensagens-*.png
done

QTDE=$(ls *.dat.png | wc -l)
BASENAME=$(ls -1 *.dat.png | head -n1 | sed 's/-[0-9]*.dat.png//g')
IMAGES=$(seq 1 $QTDE | xargs -I {} echo ${BASENAME}-{}.dat.png | tr '\n' ' ')
nice convert -delay 50 -loop 0 $IMAGES ${BASENAME}.gif
