#!/bin/bash

CRACK_HASH=$1
CHARSET="abcdefghijklmnopqrstuvwxyz"
KEYLEN=3

./keygen -c "$CHARSET" -l $KEYLEN | while read PLANE
do
	HASH=$(echo $PLANE | md5sum | cut -d ' ' -f 1)
	if [ "$HASH" = "$CRACK_HASH" ]; then
		echo $PLANE
		exit 2
	fi
done

echo "key not found"
exit 1
