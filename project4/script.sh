#!/bin/bash

g++ project.cpp -o pj.exe -lm -fopenmp -O0 -lstdc++
./pj.exe
rm pj.exe
