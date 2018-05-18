#!/bin/bash

# number of threads
for t in 1 2 4
do
    echo NUMT = $t
    # number of subdivisions:
    for s in 1000 10000 100000 16000000 32000000 48000000 64000000
    do
        echo NUMS = $s
#        gcc-7 -DNUMS=$s -DNUMT=$t project.cpp -o pj.exe -lm -fopenmp -O0 -lstdc++
	icpc -DNUMS=$s -DNUMT=$t -o pj.exe project.cpp -lm -openmp -align -qopt-report=3 -qopt-report-file="report-"$t"-"$s".optrpt" -qopt-report-phase=vec
        ./pj.exe
    done
done
rm pj.exe
