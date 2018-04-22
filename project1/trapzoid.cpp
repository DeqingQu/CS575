#include <omp.h>
#include <stdio.h>
#include <math.h>

#define NUMT             4
#define numSubdivisions 1280000

double Function(double x) {
    return sin(x);
}

int
main( )
{

#ifndef _OPENMP
    fprintf( stderr, "OpenMP is not supported here -- sorry.\n" );
    return 1;
#endif
    
    const double A = 0.;
    const double B = M_PI;
    double dx = ( B - A ) / (float) ( numSubdivisions - 1 );
    
    omp_set_num_threads( NUMT );
    double sum = ( Function( A ) + Function( B ) ) / 2.;

    double time0 = omp_get_wtime( );

    #pragma omp parallel for default(none),shared(dx),reduction(+:sum)
    for( int i = 1; i < numSubdivisions - 1; i++ )
    {
        double x = A + dx * (float) i;
        double f = Function( x );
        sum += f;
    }
    sum *= dx;
    
    
    double time1 = omp_get_wtime( );
    double time = time1 - time0;
    double megaMults = (double)(numSubdivisions - 1)/(time)/1000000.;

    printf("Result = %8.2lf \n", sum);
    printf("Performance = %8.2lf megaMults/Sec\n", megaMults);
}
