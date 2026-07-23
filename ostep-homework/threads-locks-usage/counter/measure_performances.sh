#!/bin/bash

make clean
make

MAX_THREADS=16

# > results.csv resets the previous run
echo "NumThreads,Expected,SerialValue,SerialTime,ConcurentValue,ConcurentTime,SloppyValue,SloppyTime,TLSValue,TLSTime" > results.csv
for (( i=1; i <=MAX_THREADS; i++ ))
do
	./test-counter $i
done
