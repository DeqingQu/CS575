#!/bin/bash

gcc-7 project.cpp -o pj.exe -lm -fopenmp -O0
./pj.exe
rm pj.exe
