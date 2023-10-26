#!/bin/bash

if test "$#" -ne 2
then
        echo "nu avem destui parametrii"
fi

count=0
count_rec=0

for arg in "$1"/*
do

    if test -f "$arg"
    then
	var=`cat "$arg" | grep -E "^[A-Z][a-zA-Z0-9\ \,\]+[^\.]\.$" | grep -E -v "si[\ ]*\," | grep -E -v "n[p|b]" | wc -l`
	if test "$var" -ne 0
	then
	   echo "$arg" >> "$2"
	fi
    fi

    if test -L "$arg"
    then
	count=`expr $count + 1`
    fi

    if test -d "$arg"
    then
	count_rec=`bash "$0" "$arg" "$2"`
	count=`expr $count + $count_rec`
    fi


done

echo "$count"

