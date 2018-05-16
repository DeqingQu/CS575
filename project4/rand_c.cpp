//
//  rand_c.cpp
//  
//
//  Created by Alex on 5/16/18.
//

#include <stdio.h>

int
rand_r (unsigned int *seed)
{
    unsigned int next = *seed;
    int result;
    next *= 1103515245;
    next += 12345;
    result = (unsigned int) (next / 65536) % 2048;
    next *= 1103515245;
    next += 12345;
    result <<= 10;
    result ^= (unsigned int) (next / 65536) % 1024;
    next *= 1103515245;
    next += 12345;
    result <<= 10;
    result ^= (unsigned int) (next / 65536) % 1024;
    *seed = next;
    return result;
}
int main()
{
    unsigned int seed = 0;
    int r = rand_r(&seed);
    printf("r = %d, seed = %d\n", r, seed);
    return 0;
}
