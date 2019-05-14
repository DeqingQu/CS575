#include "simd.p4.h"

#ifndef NUMTRIES
#define NUMTRIES	10
#endif

#ifndef ARRAYSIZE
#define ARRAYSIZE	100000
#endif

float A[ARRAYSIZE];
float B[ARRAYSIZE];
float C[ARRAYSIZE];

int main( int argc, char *argv[ ] )
{
#ifndef _OPENMP
        fprintf( stderr, "OpenMP is not supported here -- sorry.\n" );
        return 1;
#endif
    
        double SimdMulPerf = 0.;
        double NoneSimdMulPerf = 0.;
        double SimdMulSumPerf = 0.;
        double NoneSimdMulSumPerf = 0.;

        for( int t = 0; t < NUMTRIES; t++ )
        {
                //  SimdMul
                double time0 = omp_get_wtime( );
                SimdMul(A, B, C, ARRAYSIZE);
                double time1 = omp_get_wtime( );
                double perf = (double)ARRAYSIZE/(time1-time0)/1000000.;
                if( perf > SimdMulPerf )
                    SimdMulPerf = perf;
            
                //  NonSimdMul
                time0 = omp_get_wtime( );
                NonSimdMul(A, B, C, ARRAYSIZE);
                time1 = omp_get_wtime( );
                perf = (double)ARRAYSIZE/(time1-time0)/1000000.;
                if( perf > NoneSimdMulPerf )
                    NoneSimdMulPerf = perf;
            
                //  SimdMulSum
                time0 = omp_get_wtime( );
                float s = SimdMulSum(A, B, ARRAYSIZE);
                time1 = omp_get_wtime( );
                perf = (double)ARRAYSIZE/(time1-time0)/1000000.;
                if( perf > SimdMulSumPerf )
                {   
                    SimdMulSumPerf = perf;
                    printf("sum - simd = %.4f\n", s);
                }
            
                //  NonSimdMulSum
                time0 = omp_get_wtime( );
                s = NonSimdMulSum(A, B, ARRAYSIZE);
                time1 = omp_get_wtime( );
                perf = (double)ARRAYSIZE/(time1-time0)/1000000.;
                if( perf > NoneSimdMulSumPerf )
                {
                    NoneSimdMulSumPerf = perf;
                    printf("sum - non simd = %.4f\n", s);
                }
        }

        printf( "  SimdMul Peak Performance = %8.2lf MegaMults/Sec\n", SimdMulPerf );
        printf( "  NoneSimdMul Peak Performance = %8.2lf MegaMults/Sec\n", NoneSimdMulPerf );
        printf( "  SimdMulSum Peak Performance = %8.2lf MegaMults/Sec\n", SimdMulSumPerf );
        printf( "  NoneSimdMulSum Peak Performance = %8.2lf MegaMults/Sec\n", NoneSimdMulSumPerf );
        printf( "  Speedup for SimdMul = %8.2f \n", SimdMulPerf / NoneSimdMulPerf );
        printf( "  Speedup for SimdMulSum = %8.2f\n", SimdMulSumPerf / NoneSimdMulSumPerf );
        
    
    return 0;
}