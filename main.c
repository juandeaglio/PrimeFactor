#include <stdio.h>
#include <math.h>
#include <stdbool.h>

double product = 187;
double primes[8] = {2,3,5,7,11,13,17,19};

int main() {
    printf("Searching for prime factors of %f \n", product);
    for (int i = 0; i < 8; i++) {
        if ( fmod(product, primes[i]) == 0 ) {
            printf("Primes found: %f, %f\n", primes[i], (product/primes[i]));
            return 0;
        }
    }
    printf("Product not found.\n");
    return 1;
}