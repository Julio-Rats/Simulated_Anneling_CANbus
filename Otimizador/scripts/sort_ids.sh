cat $1 | sort -nk1 > /tmp/temp
mv /tmp/temp $1
