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

const float GRAIN_GROWS_PER_MONTH =             8.0;
const float ONE_DEER_EATS_PER_MONTH =           0.5;

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

void GrainDeer()
{
    
}

void Grain()
{
    
}

void Watcher()
{
    unsigned int seed = time(NULL);
    float ang = (  30.*(float)NowMonth + 15.  ) * ( M_PI / 180. );
    float temp = AVG_TEMP - AMP_TEMP * cos( ang );
    
    NowTemp = temp + Ranf( -RANDOM_TEMP, RANDOM_TEMP, &seed );
    
    float precip = AVG_PRECIP_PER_MONTH + AMP_PRECIP_PER_MONTH * sin( ang );
    
    NowPrecip = precip + Ranf( -RANDOM_PRECIP, RANDOM_PRECIP, &seed );
    
    if( NowPrecip < 0. )
        NowPrecip = 0.;
    
    //  update month and year
    NowMonth++;
    NowYear = 2014 + NowMonth%12;
    
    printf("Month %d, Year %d\n", NowMonth, NowYear);
}

int
main( )
{
    
    NowNumDeer = 1;
    NowHeight =  1.;
    NowMonth =    0;
    NowYear  = 2014;
    
#ifndef _OPENMP
    fprintf( stderr, "OpenMP is not supported here -- sorry.\n" );
    return 1;
#endif
    
    omp_set_num_threads(3);
#pragma omp parallel sections
    {
#pragma omp section
        {
            GrainDeer();
        }
#pragma omp section
        {
            Grain();
        }
#pragma omp section
        {
            Watcher();
        }
        // implied barrier: all sections must complete before we get here
    }

        return 0;
}
