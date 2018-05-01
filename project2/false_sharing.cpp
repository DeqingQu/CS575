#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

#define NUMPAD        6

struct s
{
    float value;
    //  false sharing fix #1
//    int pad[NUMPAD];
} Array[4];

const int SomeBigNumber = 10000000;

int main( int argc, char *argv[ ] )
{
#ifndef _OPENMP
    fprintf( stderr, "OpenMP is not available\n" );
    return 1;
#endif
    
    int number_threads = atoi(argv[1]);
    omp_set_num_threads(number_threads);
    int numProcessors = omp_get_num_procs( );
    fprintf( stderr, "Have %d processors, %d threads.\n", numProcessors, number_threads );

    double time0 = omp_get_wtime( );

    //  false sharing
//#pragma omp parallel for
//    for( int i = 0; i < 4; i++ )
//    {
//        for( int j = 0; j < SomeBigNumber; j++ )
//        {
//            Array[ i ].value = Array[ i ].value + (float)rand( );
//        }
//    }
    
    //  false sharing fix #2
#pragma omp parallel for
    for( int i = 0; i < 4; i++ )
    {
        float tmp = Array[ i ].value;
        for( int j = 0; j < SomeBigNumber; j++ )
        {
            tmp = tmp + (float)rand( );
        }
        Array[ i ].value = tmp;
    }
    
    double time1 = omp_get_wtime( );
    // print performance here:::
    double megaTimes = (double)(SomeBigNumber*4)/(time1-time0)/1000000.;
    printf("megaTimes = %f MegaTimes\n", megaTimes);
    
    return 0;
}
