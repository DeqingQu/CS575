#include "simd.p4.h"

#ifndef ARRAYSIZE
#define ARRAYSIZE	100000
#endif

#ifndef NUMTRIES
#define NUMTRIES	100
#endif

float A[ARRAYSIZE];
float B[ARRAYSIZE];
float C[ARRAYSIZE];

float Ranf( float low, float high )
{
    float r = (float) rand();         // 0 - RAND_MAX
    return(   low  +  r * ( high - low ) / (float)RAND_MAX   );
}

int main( int argc, char *argv[ ] )
{
#ifndef _OPENMP
        fprintf( stderr, "OpenMP is not supported here -- sorry.\n" );
        return 1;
#endif
    
        double maxPerformanceSimdMul = 0.;
        double maxPerformanceNoneSimdMul = 0.;
        double maxPerformanceSimdMulSum = 0.;
        double maxPerformanceNoneSimdMulSum = 0.;
        
        for( int i =0; i < ARRAYSIZE; i++ )
        {
            A[i] = Ranf(-100.0f, 100.0f);
            B[i] = Ranf(-100.0f, 100.0f);
            C[i] = Ranf(-100.0f, 100.0f);
        }

        for( int t = 0; t < NUMTRIES; t++ )
        {
                //  SimdMul
                double time0 = omp_get_wtime( );
                SimdMul(A, B, C, ARRAYSIZE);
                double time1 = omp_get_wtime( );
                double p = (double)ARRAYSIZE/(time1-time0)/1000000.;
                if( p > maxPerformanceSimdMul )
                    maxPerformanceSimdMul = p;
            
                //  NonSimdMul
                time0 = omp_get_wtime( );
                NonSimdMul(A, B, C, ARRAYSIZE);
                time1 = omp_get_wtime( );
                p = (double)ARRAYSIZE/(time1-time0)/1000000.;
                if( p > maxPerformanceNoneSimdMul )
                    maxPerformanceNoneSimdMul = p;
            
                //  SimdMulSum
                time0 = omp_get_wtime( );
                float s = SimdMulSum(A, B, ARRAYSIZE);
                time1 = omp_get_wtime( );
                p = (double)ARRAYSIZE/(time1-time0)/1000000.;
                if( p > maxPerformanceSimdMulSum )
                {   
                    maxPerformanceSimdMulSum = p;
                    printf("sum - simd = %.4f\n", s);
                }
            
                //  NonSimdMulSum
                time0 = omp_get_wtime( );
                s = NonSimdMulSum(A, B, ARRAYSIZE);
                time1 = omp_get_wtime( );
                p = (double)ARRAYSIZE/(time1-time0)/1000000.;
                if( p > maxPerformanceNoneSimdMulSum )
                {
                    maxPerformanceNoneSimdMulSum = p;
                    printf("sum - simd = %.4f\n", s);
                }
        }

        printf( "  maxPerformanceSimdMul Peak Performance = %8.2lf MegaMults/Sec\n", maxPerformanceSimdMul );
        printf( "  maxPerformanceNoneSimdMul Peak Performance = %8.2lf MegaMults/Sec\n", maxPerformanceNoneSimdMul );
        printf( "  maxPerformanceSimdMulSum Peak Performance = %8.2lf MegaMults/Sec\n", maxPerformanceSimdMulSum );
        printf( "  maxPerformanceNoneSimdMulSum Peak Performance = %8.2lf MegaMults/Sec\n", maxPerformanceNoneSimdMulSum );
        printf( "  Speedup for SimdMul = %8.2f \n", maxPerformanceSimdMul / maxPerformanceNoneSimdMul );
        printf( "  Speedup for SimdMulSum = %8.2f\n", maxPerformanceSimdMulSum / maxPerformanceNoneSimdMulSum );
        
    
    return 0;
}