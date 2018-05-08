#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include <time.h>

//#define NUMPAD  15
//#define NUMT    1

struct s
{
    float value;
} Array[4];

int main( int argc, char *argv[ ] )
{
#ifndef _OPENMP
    fprintf( stderr, "OpenMP is not available\n" );
    return 1;
#endif
    
    omp_set_num_threads(NUMT);
    int numProcessors = omp_get_num_procs();
    fprintf( stderr, "Have %d processors, %d threads.\n", numProcessors, NUMT );
    
    const int SomeBigNumber = 10000000;
    double time0 = omp_get_wtime();

#pragma omp parallel for
    for( int i = 0; i < 4; i++ )
    {
        float tmp = Array[i].value;
        for( int j = 0; j < SomeBigNumber; j++ )
        {
            unsigned int seed = 0;
            tmp = tmp + (float)rand_r(&seed);
        }
        Array[i].value = tmp;
    }
    
    double time1 = omp_get_wtime();
    
    // print performance here:::
    double megaTimes = (double)(SomeBigNumber*4)/(time1-time0)/1000000.;
    printf("megaTimes = %f MegaTimes\n", megaTimes);
    
    return 0;
}

