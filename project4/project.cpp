#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

int  NowYear;           // 2014 - 2019
int  NowMonth;          // 0 - 11

float NowPrecip;        // inches of rain per month
float NowTemp;          // temperature this month
float NowHeight;        // grain height in inches
int   NowNumDeer;       // current deer population
int   NowNumWolf;     // current wolf population

const float GRAIN_GROWS_PER_MONTH =             8.0;
const int   DEER_GROWS_PER_MONTH =              20;
const int   WOLF_GROWS_PER_MONTH =              1;

const float ONE_DEER_EATS_PER_MONTH =           0.01;
const float ONE_WOLF_EATS_PER_MONTH =           1;

const float AVG_PRECIP_PER_MONTH =              6.0;
const float AMP_PRECIP_PER_MONTH =              6.0;
const float RANDOM_PRECIP =                     2.0;

const float AVG_TEMP =                          50.0;
const float AMP_TEMP =                          20.0;
const float RANDOM_TEMP =                       10.0;

const float MIDTEMP =                           40.0;
const float MIDPRECIP =                         10.0;

float Ranf( float low, float high, unsigned int* seed )
{
    float r = (float) rand_r(seed);      // 0 - RAND_MAX
    return( low + r * ( high - low ) / (float)RAND_MAX );
}

void GrainGrowth()
{
    //  compute into tmp variables
    float tempFactor = exp(-pow((NowTemp - MIDTEMP) / 10., 2));
    float precipFactor = exp(-pow((NowPrecip - MIDPRECIP) / 10., 2));
    float tempNowHeight  = NowHeight;
    tempNowHeight += tempFactor * precipFactor * GRAIN_GROWS_PER_MONTH;
    tempNowHeight -= (float)NowNumDeer * ONE_DEER_EATS_PER_MONTH;
    if(tempNowHeight <= 0.)
        tempNowHeight = 0.;
#pragma omp barrier
    //  copy into global variables
    NowHeight = tempNowHeight;
#pragma omp barrier
#pragma omp barrier

}

void Deer()
{
    int tempNowNumDeer = NowNumDeer;
    float grainFactor = exp(-pow((NowHeight - GRAIN_GROWS_PER_MONTH) / 10., 2));
    unsigned int seed = time(NULL);
    tempNowNumDeer += grainFactor * DEER_GROWS_PER_MONTH;
    tempNowNumDeer -= ONE_WOLF_EATS_PER_MONTH * NowNumWolf;
    if (tempNowNumDeer <= 0)
    {
        tempNowNumDeer = grainFactor * DEER_GROWS_PER_MONTH;
    }
#pragma omp barrier
    NowNumDeer = tempNowNumDeer;
#pragma omp barrier
#pragma omp barrier
    
}

void Wolf()
{
    int tempNumWolf = NowNumWolf;
    unsigned int seed = time(NULL);
    float rand = Ranf(0., 1.0, &seed);
    int rand_i = rand_r(&seed);
    printf("ran = %f, rand_i = %d\n", rand, rand_i);
    if (rand < 0.1) {
        tempNumWolf += WOLF_GROWS_PER_MONTH;
    }
    if (rand > 0.95) {
        tempNumWolf -= WOLF_GROWS_PER_MONTH;
    }
    if (tempNumWolf <= 0)
    {
        tempNumWolf = WOLF_GROWS_PER_MONTH;
    }
#pragma omp barrier
    NowNumWolf = tempNumWolf;
#pragma omp barrier
#pragma omp barrier
}

void Watcher()
{
    #pragma omp barrier
    
    #pragma omp barrier
//    printf("Month %d, Year %d\n", NowMonth % 12 + 1, NowYear);
    printf("%f\t%f\t%f\t%d\t%d\n", NowTemp, NowPrecip, NowHeight, NowNumDeer, NowNumWolf);
    
    //  update month and year
    NowMonth++;
    NowYear = 2014 + NowMonth/12;
    
    //  update temp and precipitation
    unsigned int seed = time(NULL);
    float ang = (  30.*(float)NowMonth + 15.  ) * ( M_PI / 180. );
    float temp = AVG_TEMP - AMP_TEMP * cos( ang );
    NowTemp = temp + Ranf( -RANDOM_TEMP, RANDOM_TEMP, &seed );
    float precip = AVG_PRECIP_PER_MONTH + AMP_PRECIP_PER_MONTH * sin( ang );
    NowPrecip = precip + Ranf( -RANDOM_PRECIP, RANDOM_PRECIP, &seed );
    
    if( NowPrecip < 0. )
        NowPrecip = 0.;
    #pragma omp barrier
}

int main( )
{
    //  setup global variables
    NowNumDeer = 30;
    NowNumWolf = 4;
    NowHeight =  1.;
    NowMonth =    0;
    NowYear  = 2014;
    
    //  calculate temp and precipitation
    unsigned int seed = time(NULL);
    float ang = (  30.*(float)NowMonth + 15.  ) * ( M_PI / 180. );
    float temp = AVG_TEMP - AMP_TEMP * cos( ang );
    NowTemp = temp + Ranf( -RANDOM_TEMP, RANDOM_TEMP, &seed );
    float precip = AVG_PRECIP_PER_MONTH + AMP_PRECIP_PER_MONTH * sin( ang );
    NowPrecip = precip + Ranf( -RANDOM_PRECIP, RANDOM_PRECIP, &seed );
    
#ifndef _OPENMP
    fprintf( stderr, "OpenMP is not supported here -- sorry.\n" );
    return 1;
#endif
    
    omp_set_num_threads(4);
    
    while (NowMonth < 72) {
#pragma omp parallel sections
        {
#pragma omp section
            {
                Deer();
            }
#pragma omp section
            {
                Wolf();
            }
#pragma omp section
            {
                GrainGrowth();
            }
#pragma omp section
            {
                Watcher();
            }
            // implied barrier: all sections must complete before we get here
        }
    }

        return 0;
}
