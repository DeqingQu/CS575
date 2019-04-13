#!/bin/bash

# number of threads
for t in 1 2 4 8
do
    echo NUMT = $t
    gcc-7 -DNUMT=$t -DNUMTRIALS=1000000  project.cpp -o proj.exe -lm -fopenmp
    ./proj.exe
done

for t in 1 2 4 8
do
    echo NUMT = $t
    gcc-7 -DNUMT=$t -DNUMTRIALS=10000000  project.cpp -o proj.exe -lm -fopenmp
    ./proj.exe
done

for t in 1 2 4 8
do
    echo NUMT = $t
    gcc-7 -DNUMT=$t -DNUMTRIALS=100000000  project.cpp -o proj.exe -lm -fopenmp
    ./proj.exe
done

rm proj.exe