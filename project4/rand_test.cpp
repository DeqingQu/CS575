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

    omp_set_num_threads(5);

    for (int i=0; i<5; i++)
#pragma omp parallel sections
    {
#pragma omp section
        {
            unsigned int seed = time(NULL);
            float r = Ranf(0., 1., &seed);
            printf("random1 = %f\n", r);
        }
#pragma omp section
        {
            unsigned int seed = time(NULL);
            float r = Ranf(0., 1., &seed);
            printf("random2 = %f\n", r);
        }
#pragma omp section
        {
            unsigned int seed = time(NULL);
            float r = Ranf(0., 1., &seed);
            printf("random3 = %f\n", r);
        }
#pragma omp section
        {
            unsigned int seed = time(NULL);
            float r = Ranf(0., 1., &seed);
            printf("random4 = %f\n", r);
        }
    }
    
    return 0;
}
