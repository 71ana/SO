#!/bin/bash

if test "$#" -ne 1
then
        echo "nu avem destui parametrii"
fi

count=0

while read line
do
    var=$(echo $line | grep -E "^[A-Z][a-zA-Z0-9\ \,\]+[^\.]*[.!?]$" | grep -E -v "si[\ ]*\," | grep -E -v "n[p|b]" | grep -E "$1")
    if test "$var" != ""
    then
	count=`expr $count + 1`
    fi
done

echo "$count"
