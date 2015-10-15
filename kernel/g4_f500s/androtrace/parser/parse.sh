#!/bin/bash

UIO=`ls ./data/*.uio`

gcc -static -o androtrace_parser trace_parser.c

if [ $1 -a $1 = "all" ]
then
	for i in $UIO;
	do
		./androtrace_parser $i
	done
else
	for i in $UIO;
	do
		#if [ ! -e "$i"_parse.txt ]
		#then
			./androtrace_parser $i
		#fi
	done
fi
