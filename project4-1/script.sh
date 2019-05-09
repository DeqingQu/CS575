#!/bin/bash

# number of threads
for size in 1000 10000 100000 1000000
do
    echo ARRAYSIZE = $size
    g++  -c  simd.p4.cpp -o simd.p4.o
    g++  -DARRAYSIZE=$size -o arraymult.exe  arraymult.cpp simd.p4.o  -lm  -fopenmp
    ./arraymult.exe
done

rm arraymult.exe