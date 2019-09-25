cat $1 | sed -e /^[^0-9]/d | sed -e /^$/d > $2
