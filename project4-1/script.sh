#!/bin/bash

# number of threads
for size in 1000 5000 10000 20000 40000 60000 80000 100000 120000 140000 160000 180000 200000
do
    echo ARRAYSIZE = $size
    g++  -c  simd.p4.cpp -o simd.p4.o
    g++  -DARRAYSIZE=$size -o arraymult.exe  arraymult.cpp simd.p4.o  -lm  -fopenmp
    ./arraymult.exe
done

rm simd.p4.o
rm arraymult.exe