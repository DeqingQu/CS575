#!/bin/bash

# number of threads
for gs in 1024 16*1024 256*1024 1024*1024 4*1024*1024 8*1024*1024
do
    # number of subdivisions:
    for ls in 1 4 16 32 128 512 1024
    do
	let ng=$gs/$ls
        g++ -DNUM_ELEMENTS=$gs -DLOCAL_SIZE=$ls -DNUM_WORK_GROUPS=$ng -o first first.cpp /scratch/cuda-7.0/lib64/libOpenCL.so -lm -fopenmp
        ./first
    done
done
rm first
