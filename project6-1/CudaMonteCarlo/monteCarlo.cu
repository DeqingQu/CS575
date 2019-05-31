// Array multiplication: C = A * B:

// System includes
#include <stdio.h>
#include <assert.h>
#include <malloc.h>
#include <math.h>
#include <stdlib.h>

// CUDA runtime
#include <cuda_runtime.h>

// Helper functions and utilities to work with CUDA
#include "helper_functions.h"
#include "helper_cuda.h"


#ifndef BLOCKSIZE
#define BLOCKSIZE		32		// number of threads per block
#endif

#ifndef NUMTRIALS
#define NUMTRIALS			16*1024	// array size
#endif

#ifndef TOLERANCE
#define TOLERANCE		0.00001f	// tolerance to relative error
#endif

// ranges for the random numbers:
const float XCMIN =	 0.0;
const float XCMAX =	 2.0;
const float YCMIN =	 0.0;
const float YCMAX =	 2.0;
const float RMIN  =	 0.5;
const float RMAX  =	 2.0;

// array multiplication (CUDA Kernel) on the device: C = A * B

__global__  void ArrayMul( float *A, float *B, float *C )
{
	int gid = blockIdx.x*blockDim.x + threadIdx.x;
	C[gid] = A[gid] * B[gid];
}

__global__  void MonteCarlo( float *xcs, float *ycs, float *rs, int *numHits )
{
	//int gid = blockIdx.x*blockDim.x + threadIdx.x;
	//C[gid] = A[gid] * B[gid];
    //numHits[0] = 10000;
        
    __shared__ float prods[BLOCKSIZE];

	unsigned int numItems = blockDim.x;
	unsigned int tnum = threadIdx.x;
	unsigned int wgNum = blockIdx.x;
	unsigned int gid = blockIdx.x*blockDim.x + threadIdx.x;

	prods[tnum] = 1;

	for (int offset = 1; offset < numItems; offset *= 2)
	{
		int mask = 2 * offset - 1;
		__syncthreads();
		if ((tnum & mask) == 0)
		{
			prods[tnum] += prods[tnum + offset];
		}
	}

	__syncthreads();
	if (tnum == 0)
		numHits[wgNum] = prods[0];
}

// helper functions
float Ranf( float low, float high )
{
        float r = (float) rand();               // 0 - RAND_MAX
        float t = r  /  (float) RAND_MAX;       // 0. - 1.

        return   low  +  t * ( high - low );
}

int Ranf( int ilow, int ihigh )
{
        float low = (float)ilow;
        float high = ceil( (float)ihigh );

        return (int) Ranf(low,high);
}

void TimeOfDaySeed( )
{
	struct tm y2k = { 0 };
	y2k.tm_hour = 0;   y2k.tm_min = 0; y2k.tm_sec = 0;
	y2k.tm_year = 100; y2k.tm_mon = 0; y2k.tm_mday = 1;

	time_t  timer;
	time( &timer );
	double seconds = difftime( timer, mktime(&y2k) );
	unsigned int seed = (unsigned int)( 1000.*seconds );    // milliseconds
	srand( seed );
}

// main program:

int
main( int argc, char* argv[ ] )
{
	int dev = findCudaDevice(argc, (const char **)argv);

	// allocate host memory:

//	float * hA = new float [ SIZE ];
//	float * hB = new float [ SIZE ];
//	float * hC = new float [ SIZE ];
    
    
	float * hxcs = new float [ NUMTRIALS ];
	float * hycs = new float [ NUMTRIALS ];
	float * hrs = new float [ NUMTRIALS ];
    int * hnumHits = new int [ NUMTRIALS/BLOCKSIZE ];
    

    // fill the random-value arrays:
    for( int n = 0; n < NUMTRIALS; n++ )
    {
        hxcs[n] = Ranf( XCMIN, XCMAX );
        hycs[n] = Ranf( YCMIN, YCMAX );
        hrs[n] = Ranf(  RMIN,  RMAX );
    }
    
//	for( int i = 0; i < SIZE; i++ )
//	{
//		hA[i] = hB[i] = (float) sqrt(  (float)i  );
//	}

	// allocate device memory:

//	float *dA, *dB, *dC;
    float *dxcs, *dycs, *drs;
    int *dnumHits;

//	dim3 dimsA( SIZE, 1, 1 );
//	dim3 dimsB( SIZE, 1, 1 );
//	dim3 dimsC( SIZE, 1, 1 );
    
	dim3 dimsxcs( NUMTRIALS, 1, 1 );
	dim3 dimsycs( NUMTRIALS, 1, 1 );
	dim3 dimsrs( NUMTRIALS, 1, 1 );
    dim3 dimsnumHits( NUMTRIALS, 1, 1);
    

	cudaError_t status;
//	status = cudaMalloc( reinterpret_cast<void **>(&dA), SIZE*sizeof(float) );
//		checkCudaErrors( status );
//	status = cudaMalloc( reinterpret_cast<void **>(&dB), SIZE*sizeof(float) );
//		checkCudaErrors( status );
//	status = cudaMalloc( reinterpret_cast<void **>(&dC), SIZE*sizeof(float) );
//		checkCudaErrors( status );

	status = cudaMalloc( reinterpret_cast<void **>(&dxcs), NUMTRIALS*sizeof(float) );
		checkCudaErrors( status );
	status = cudaMalloc( reinterpret_cast<void **>(&dycs), NUMTRIALS*sizeof(float) );
		checkCudaErrors( status );
	status = cudaMalloc( reinterpret_cast<void **>(&drs), NUMTRIALS*sizeof(float) );
		checkCudaErrors( status );
    status = cudaMalloc( reinterpret_cast<void **>(&dnumHits), (NUMTRIALS/BLOCKSIZE)*sizeof(int) );
		checkCudaErrors( status );


	// copy host memory to the device:

//	status = cudaMemcpy( dA, hA, SIZE*sizeof(float), cudaMemcpyHostToDevice );
//		checkCudaErrors( status );
//	status = cudaMemcpy( dB, hB, SIZE*sizeof(float), cudaMemcpyHostToDevice );
//		checkCudaErrors( status );

	status = cudaMemcpy( dxcs, hxcs, NUMTRIALS*sizeof(float), cudaMemcpyHostToDevice );
		checkCudaErrors( status );
	status = cudaMemcpy( dycs, hycs, NUMTRIALS*sizeof(float), cudaMemcpyHostToDevice );
		checkCudaErrors( status );
    status = cudaMemcpy( drs, hrs, NUMTRIALS*sizeof(float), cudaMemcpyHostToDevice );
		checkCudaErrors( status );

	// setup the execution parameters:

	dim3 threads(BLOCKSIZE, 1, 1 );
	dim3 grid( NUMTRIALS / threads.x, 1, 1 );

	// Create and start timer

	cudaDeviceSynchronize( );

	// allocate CUDA events that we'll use for timing:

	cudaEvent_t start, stop;
	status = cudaEventCreate( &start );
		checkCudaErrors( status );
	status = cudaEventCreate( &stop );
		checkCudaErrors( status );

	// record the start event:

	status = cudaEventRecord( start, NULL );
		checkCudaErrors( status );

	// execute the kernel:

//	ArrayMul<<< grid, threads >>>( dA, dB, dC );
    MonteCarlo<<< grid, threads >>>( dxcs, dycs, drs, dnumHits );

	// record the stop event:

	status = cudaEventRecord( stop, NULL );
		checkCudaErrors( status );

	// wait for the stop event to complete:

	status = cudaEventSynchronize( stop );
		checkCudaErrors( status );

	float msecTotal = 0.0f;
	status = cudaEventElapsedTime( &msecTotal, start, stop );
		checkCudaErrors( status );

	// compute and print the performance

	double secondsTotal = 0.001 * (double)msecTotal;
	double trialsPerSecond = (float)NUMTRIALS / secondsTotal;
	double megaTrialsPerSecond = trialsPerSecond / 1000000.;
	fprintf( stderr, "Size = %10d, MegaTrials/Second = %10.2lf\n", NUMTRIALS, megaTrialsPerSecond );

	// copy result from the device to the host:

	status = cudaMemcpy( hnumHits, dnumHits, (NUMTRIALS/BLOCKSIZE)*sizeof(int), cudaMemcpyDeviceToHost );
		checkCudaErrors( status );
    
    int sum = 0.;
	for(int i = 0; i < NUMTRIALS/BLOCKSIZE; i++ )
	{
		sum += hnumHits[i];
	}
    float prob = (float)(sum) / (float)(NUMTRIALS);
	fprintf( stderr, "\nProbability=%8.4lf\n", prob );
    fprintf( stderr, "\NumHit=%10d\n", sum );
    
	// clean up memory:
//	delete [ ] hA;
//	delete [ ] hB;
//	delete [ ] hC;
    delete [ ] hxcs;
    delete [ ] hycs;
    delete [ ] hrs;
    delete [ ] hnumHits;

//	status = cudaFree( dA );
//		checkCudaErrors( status );
//	status = cudaFree( dB );
//		checkCudaErrors( status );
//	status = cudaFree( dC );
//		checkCudaErrors( status );

    status = cudaFree( dxcs );
		checkCudaErrors( status );
	status = cudaFree( dycs );
		checkCudaErrors( status );
	status = cudaFree( drs );
		checkCudaErrors( status );
    status = cudaFree( dnumHits );
		checkCudaErrors( status );



	return 0;
}
