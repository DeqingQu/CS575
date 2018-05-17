#!/bin/bash

# number of threads
for t in 1 2 4
do
    echo NUMT = $t
    # number of subdivisions:
    for s in 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16
    do
        echo NUMPAD = $s
        gcc-7 -DNUMPAD=$s -DNUMT=$t fs_fix1.cpp -o fs.exe -lm -fopenmp -O0 -lstdc++
        ./fs.exe
    done
done
rm fs.exe
