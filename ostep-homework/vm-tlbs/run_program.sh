#!/bin/bash

make clean
make

MAX_NUM_PAGES=16384
MAX_NUM_TRIALS=1000000
TARGET_FILE="output.txt"

> "${TARGET_FILE}"

for (( num_pages=2; num_pages<=MAX_NUM_PAGES; num_pages*=2 ))
do
    for (( num_trials=2; num_trials<=MAX_NUM_TRIALS; num_trials*=2 ))
    do
        echo "Running with $num_pages pages and $num_trials trials" >> "${TARGET_FILE}"
        ./tlb $num_pages $num_trials >> "${TARGET_FILE}"
		echo "" >> "${TARGET_FILE}"
    done
done

echo "View output in ${TARGET_FILE} file"
