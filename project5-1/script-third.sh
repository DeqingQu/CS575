#!/bin/bash

# number of threads
for gs in 1024 512*1024 1024*1024 2*1024*1024 4*1024*1024 8*1024*1024
do
    # number of subdivisions:
    for ls in 32 64 128 256
    do
	let ng=$gs/$ls
        g++ -DNUM_ELEMENTS=$gs -DLOCAL_SIZE=$ls -DNUM_WORK_GROUPS=$ng -o third third.cpp /scratch/cuda-7.0/lib64/libOpenCL.so -lm -fopenmp
        ./third
    done
done

rm third
