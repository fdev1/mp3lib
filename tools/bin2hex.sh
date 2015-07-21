#!/bin/bash
rm $2
cat $1 | \
od -An -t x1 | \
while read line; \
do  \
	for char in $line; \
	do \
		echo $char >> $2; \
	done ; \
done
