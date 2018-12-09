/*
 * This program takes a number as an input, the number is made up by two factors which are both prime numbers.
 * The task of the program is to print out the two prime numbers that make up the product.
 * The method we used to accomplish this was by implementing the Sieve of Eratosthenes, which is an algorithm
 * that searches for prime numbers. The way the algorithm works is better seen through the visual found on
 * Wikipedia but can be briefly described as an algorithm that marks all non-primes up until the square root
 * of the product. Once the square root of the product has been reached, the numbers that were not marked
 * can now be considered as prime and will be placed into an array.
 * OpenMP can be used here in several ways, all of the implementations of OpenMP here split up the work
 * that can be found in for loops by the number of cores. In most cases this will help speed up iterating
 * through an array except for when the for loop's amount of work is less than amount of cores.
 */
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <omp.h>


unsigned long long product = 71;
//double primes[8] = {2,3,5,7,11,13,17,19};

unsigned long long * generatePrimes(double max) {

    //int *array = (int*) malloc (max*sizeof(int));
    unsigned long long cap = (unsigned long long) max + 1;
    unsigned long long count = 0;
    unsigned long long array[cap];
    //initialize entire array with 1's
    //parallelized this initialization by splitting the amount of
    //indices that a core must initialize to one.
    #pragma omp parallel shared(array)
    {
        unsigned long long tid = (unsigned long long) omp_get_thread_num();
        unsigned long long nthreads = (unsigned long long) omp_get_num_threads();
        for (unsigned long long i = tid; i < cap; i+=nthreads)
        {
            array[i] = 1;
        }
    }
    //mark 0 and 1 as not prime
    array[0] = 0;
    array[1] = 0;
    //for every array index that is still marked 1 for prime
    for (unsigned long long i = 2; (i < cap) && (array[i] == 1); i++)
    {
        //Parallelized the non-primes being marked in the array of numbers.
        #pragma omp parallel
        {
            unsigned long long tid = (unsigned long long) omp_get_thread_num();
            unsigned long long nthreads = (unsigned long long) omp_get_num_threads();
            //multiply the index number by consecutive integers and mark each product as not prime
            for (unsigned long long j = tid+2; (i * j) < cap; j+=nthreads)
            {
                array[i * j] = 0;
            }
        }
    }


    count = 0;
    //count primes for output array size
    //Parallelized the counting of each prime number, so that it returns the size of the output array.
    #pragma omp parallel
    {
        unsigned long long tid = (unsigned long long) omp_get_thread_num();
        unsigned long long nthreads = (unsigned long long) omp_get_num_threads();
        unsigned long long subcount = 0;

        for (unsigned long long i = tid+2; i < cap; i+=nthreads)
        {
            if (array[i] == 1)
            {
                subcount++;
            }
        }
        //each thread has its own copy of subcount,
        //this may help with reducing false sharing
        #pragma omp critical
            count+=subcount;

        //printf("tid %lld, subcount:%lld, count total:%lld\n",tid,subcount, count);
    }
    //printf("count: %lld\n",count);
    unsigned long long *output = (unsigned long long*)malloc(count*sizeof(unsigned long long));

    //add primes to output array
    //Parallelized like the previous for loop,
    //Divides the work up by number of cores (each thread starts at i = its own unique thread id)
    //Then iterates through for loop with multple threads.
    count = 0;
    #pragma omp parallel shared(output,array,cap)
    {
        unsigned long long tid = (unsigned long long) omp_get_thread_num();
        unsigned long long nthreads = (unsigned long long) omp_get_num_threads();
        unsigned long long subcount = 0;

        for (unsigned long long i = tid+2; i < cap; i+=nthreads)
        {
            if (array[i] == 1) {
                output[count] = i;
                subcount+=nthreads;
            }
        }
    }
    return output;
}

int main(int argc, char** argv) {
    double start = omp_get_wtime();
    //get product from terminal arguments
    if (argc > 1) {
        char * pEnd;
        printf("%s\n",*argv);
        product = strtoull(argv[1],&pEnd,10);
    } else {
        printf("No args found; using default value.\n");
    }

    printf("product = %lld \n", product);

    unsigned long long * primeList = generatePrimes(sqrt(product));
    // creating variables to pass into our parallel pragma so that it can be used in the parallelization
    unsigned long long i;
	bool found = false;
    unsigned long long  nthreads,tid;

    //The final parallelization that occurs
    //Work is split up between all cores, each core iterates through the array
    //Checks if the number it iterates to is modulus to the product, if so it is our prime factor
    //Finding one prime factor gives us the second prime factor as well.
	#pragma omp parallel shared(primeList,nthreads,product,found) private(tid,i)
	{
	    //gets number of threads, this will be useful for iterating later
        nthreads = (unsigned long long )omp_get_num_threads();
        //get thread id, this will initialize each for loop that is run
        tid = (unsigned long long )omp_get_thread_num();

		for (i = tid; primeList[i] != 0 && !found; i+=nthreads)
		{
            //printf("tid:%d, i:%ld, primeList[i]: %ld, nthreads: %d, product: %ld\n",tid,i,primeList[i],nthreads,product);
            if (fmod(product, primeList[i]) == 0)
			{
                found = true;
				printf("Primes found: %lld, %lld\n", primeList[i], (product / primeList[i]));
			}
		}
	}
	double end = omp_get_wtime();
	printf("time: %lf\n", end - start);
	return 1;
}