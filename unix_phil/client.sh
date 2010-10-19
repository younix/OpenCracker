#!/bin/sh

SERVER=127.0.0.1
TMP="tmp"

function get_var {
	cat $TMP | grep "$2" | cut -d ":" -f 2
}

curl http://${SERVER}/cracker.txt > tmp

CHARSET=$(get_var charset)
START=$(get_var start)
END=$(get_var end)
CRYPT=$(get_var crypt)

echo "./cracker.sh -c" "$CHARSET" "-s $START -e $END"
