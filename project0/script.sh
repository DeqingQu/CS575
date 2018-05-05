#!/bin/bash

# number of threads
for t in 1 2 4
do
    echo NUMT = $t
    gcc-7 -DNUMT=$t project.cpp -o proj.exe -lm -fopenmp
    ./proj.exe
done
rm proj.exe
