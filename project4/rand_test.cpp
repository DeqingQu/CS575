#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

float Ranf( float low, float high, unsigned int* seed )
{
    float r = (float) rand_r(seed);      // 0 - RAND_MAX
    return( low + r * ( high - low ) / (float)RAND_MAX );
}

int main()
{

    omp_set_num_threads(4);

    for (int i=0; i<2; i++)
#pragma omp parallel sections
    {
#pragma omp section
        {
            unsigned int seed = omp_get_thread_num() * 234682;
            float r = Ranf(0., 1., &seed);
            printf("random1 = %f\n", r);
        }
#pragma omp section
        {
            unsigned int seed = omp_get_thread_num() * 49712646;
            float r = Ranf(0., 1., &seed);
            printf("random2 = %f\n", r);
        }
#pragma omp section
        {
            unsigned int seed = omp_get_thread_num() * 4124816;

            float r = Ranf(0., 1., &seed);
            printf("random3 = %f\n", r);
        }
#pragma omp section
        {
            unsigned int seed = omp_get_thread_num() * 112312428347284;
            float r = Ranf(0., 1., &seed);
            printf("random4 = %f\n", r);
        }
    }
    
    return 0;
}
