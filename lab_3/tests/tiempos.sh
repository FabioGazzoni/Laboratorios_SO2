#!/bin/bash

n=100

for i in $(seq 1 $n)
do
    echo "Ejecución n° $i:" >> times
    (time ../bin/laboratorio3) 2>> times
    echo "" >> times
done