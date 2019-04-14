#!/bin/bash

# number of threads
for t in 1 2 4 8
do
    echo NUMT = $t
    for a in 1000000 3000000 6000000 10000000
    do
        g++ -DNUMT=$t -DNUMTRIALS=$a  project1.cpp -o proj.exe -lm -fopenmp
        ./proj.exe
    done
done

rm proj.exe