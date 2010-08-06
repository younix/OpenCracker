#!/bin/bash

CRACK_HASH=$1

./keygen -c abc -l 3 | while read PLANE
do
	HASH=$(echo $PLANE | md5sum | cut -d ' ' -f 1)
	if [ $HASH = $CRACK_HASH ]; then
		echo $PLANE
	fi
done
