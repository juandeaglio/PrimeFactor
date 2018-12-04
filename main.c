#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <omp.h>



int product = 71;
//double primes[8] = {2,3,5,7,11,13,17,19};

int * generatePrimes(double max) {
	//int *array = (int*) malloc (max*sizeof(int));
	int cap = (int) max + 1;
	int count = 0;
	int array[cap];
	//initialize entire array with 1's
	for (int i = 0; i < cap; i++) {
		array[i] = 1;
	}
	//mark 0 and 1 as not prime
	array[0] = 0;
	array[1] = 0;
	//for every array index that is still marked 1 for prime
	for (int i = 2; (i < cap) && (array[i] == 1); i++) {
		//multiply the index number by consecutive integers and mark each product as not prime
		for (int j = 2; (i * j) < cap; j++) {
			array[i * j] = 0;
		}
	}
	//count primes for output array size
	for (int i = 2; i < cap; i++) {
		if (array[i] == 1) {
			//printf("%d\n", i);
			count++;
		}
	}
	int *output = (int*)malloc(count*sizeof(int));
	count = 0;
	//add primes to output array
	for (int i = 2; i < cap; i++) {
	    if (array[i] == 1) {
            output[count] = i;
            count++;
	    }
	}
	return output;
}

int main(int argc, char** argv) {
    double start = omp_get_wtime();
    //get product from terminal arguments
    if (argc > 1) {
        char * pEnd;
        product = (int)strtol(argv[1],&pEnd,10);
    } else {
        printf("No args found; using default value.\n");
    }

    printf("product = %d \n", product);
    printf("maximum lower prime = sqrt(product) = %lf \n",sqrt(product));
	//generate a list of primes up to the max lower prime
	printf("Generating table of possible prime factors\n");
    int * primeList = generatePrimes(sqrt(product));
	//check each prime against product
    printf("Attempting to factor\n");
	for (int i = 0; primeList[i] != 0; i++) {
	    if ( fmod(product, primeList[i]) == 0 ) {
            printf("Primes found: %d, %d\n", primeList[i], (product/primeList[i]));
            double end = omp_get_wtime();
            printf("time: %lf", end-start);
            return 0;
	    }
	}
    printf("Prime factors not found.\n");
	return 1;
}
