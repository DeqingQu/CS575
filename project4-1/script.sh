#!/bin/bash

# number of threads
for size in 1000 5000 10000 50000 100000 200000 500000 1000000 2000000 5000000
do
    echo ARRAYSIZE = $size
    g++  -c simd.p4.cpp -o simd.p4.o
    g++  -DARRAYSIZE=$size -o main  main.cpp simd.p4.o  -lm  -fopenmp
    ./main
done

rm simd.p4.o
rm main