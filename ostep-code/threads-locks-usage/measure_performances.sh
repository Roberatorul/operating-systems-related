#!/bin/bash
make clean
make

TARGET="./approximate-counter"

echo "Threshold,Expected,Returned" > results.csv

for (( i = 10; i < 1000000; i *= 10 ))
do
    ${TARGET} $i
done