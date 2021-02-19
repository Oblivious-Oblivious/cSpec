#include "prime_factors.h"

vector *factors_of(int remainder) {
    vector *factors = new_vector();
    
    long divisor = 2;

    while(remainder > 1) {
        while(remainder % divisor == 0) {
            vector_add(factors, (void*)divisor);
            remainder /= divisor;
        }
        divisor++;
    }
    return factors;
}
