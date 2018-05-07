#!/bin/bash

# number of threads
for t in 1 2 4 8
do
    echo NUMT = $t
    gcc-7 -DNUMT=$t fs_fix2.cpp -o fs.exe -lm -fopenmp
    ./fs.exe
done
rm fs.exe
