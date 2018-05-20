#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define NUMTRIES        5

float A[NUMS];
float C[NUMS];

float Ranf( float low, float high, unsigned int* seed )
{
    float r = (float) rand_r(seed);      // 0 - RAND_MAX
    return( low + r * ( high - low ) / (float)RAND_MAX );
}

int main( )
{
#ifndef _OPENMP
        fprintf( stderr, "OpenMP is not supported here -- sorry.\n" );
        return 1;
#endif

        omp_set_num_threads( NUMT );
    
        double sumMegaSqrts = 0.;
        unsigned int seed = time(NULL);
    
        for( int i = 0; i < NUMS; i++ )
        {
            A[i] = Ranf(0., 1., &seed);
        }
    
        for( int t = 0; t < NUMTRIES; t++ )
        {
            double time0 = omp_get_wtime( );
#pragma omp parallel for 
            for( int i = 0; i < NUMS; i++ )
            {
                C[i] = sqrt( A[i] );
            }

            double time1 = omp_get_wtime( );
            double megaSqrts = (double)NUMS/(time1-time0)/1000000.;
            sumMegaSqrts += megaSqrts;
        }
    
        double avgMegaSqrts = sumMegaSqrts/(double)NUMTRIES;
        printf( "Average Performance = %.2lf MegaSqrts/Sec\n", avgMegaSqrts );

	// note: %lf stands for "long float", which is how printf prints a "double"
	//        %d stands for "decimal integer", not "double"

        return 0;
}
