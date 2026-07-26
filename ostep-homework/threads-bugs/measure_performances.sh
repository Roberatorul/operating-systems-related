#!/bin/bash

LOOPS=500000
THREADS=(1 2 4 8)
PROGRAMS=("vector-deadlock" "vector-global-order" "vector-try-wait" "vector-avoid-hold-and-wait" "vector-nolock")
OUTPUT_FILE="results.csv"

# Creăm header-ul fișierului CSV
echo "Program,Threads,Time" > $OUTPUT_FILE

echo "=== Incepere Benchmark ==="

for prog in "${PROGRAMS[@]}"; do
    if [ ! -x "./$prog" ]; then
        echo "[Avertisment] Executabilul ./$prog nu a fost gasit. Il sarim."
        continue
    fi
    
    for t in "${THREADS[@]}"; do
        echo "Rulez: $prog cu $t thread-uri..."
        
        # Rulăm comanda și capturăm output-ul
        # Folosim -t (timp) și numărul de thread-uri și iterații
        OUTPUT=$(./$prog -t -n $t -l $LOOPS)
        
        # Extragem doar numărul secundelor (ex: din "Time: 0.15 seconds" luăm "0.15")
        TIME=$(echo "$OUTPUT" | grep "Time:" | awk '{print $2}')
        
        # Salvăm în CSV
        echo "$prog,$t,$TIME" >> $OUTPUT_FILE
    done
done

echo "=== Benchmark Finalizat! Rezultatele sunt in $OUTPUT_FILE ==="