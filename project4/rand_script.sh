#!/bin/bash

gcc-7 rand_test.cpp -o pj.exe -lm -fopenmp -O0 -lstdc++
./pj.exe
rm pj.exe
