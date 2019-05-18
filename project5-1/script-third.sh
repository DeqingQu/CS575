#!/bin/bash

# number of threads
for gs in 1024*1024 2*1024*1024 4*1024*1024 6*1024*1024 8*1024*1024 16*1024*1024 32*1024*1024 64*1024*1024
do
    g++ -DNUM_ELEMENTS=$gs -o third third.cpp /scratch/cuda-7.0/lib64/libOpenCL.so -lm -fopenmp
    ./third
done
rm third
