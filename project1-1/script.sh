#!/bin/bash

# number of threads
for t in 1 2 4 8
do
    echo NUMT = $t
    for a in 100000 300000 600000 1000000
    do
        g++ -DNUMT=$t -DNUMTRIALS=$a  project1.cpp -o proj.exe -lm -fopenmp
        ./proj.exe
    done
done

rm proj.exe