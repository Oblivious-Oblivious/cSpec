#ifndef _PRIMES_MODULE_H_
#define _PRIMES_MODULE_H_

#include "../../src/cSpec.h"
#include "prime_factors.h"

module(primes, {
    before({
        vector *empty_list = new_vector();
    });

    it("finds the factors of 1", {
        int f1 = vector_length(factors_of(1));
        assert_that(f1 is 0);
    });

    it("finds the factors of 2", {
        int f2 = vector_get(factors_of(2), 0);
        assert_that(f2 is 2);
    });

    it("finds the factors of 3", {
        int f3 = vector_get(factors_of(3), 0);
        assert_that(f3 is 3);
    });

    it("finds the factors of 4", {
        vector *f4 = factors_of(4);
        assert_that_int(vector_get(f4, 0) equals to 2);
        assert_that_int(vector_get(f4, 1) equals to 2);
    });

    it("finds the factors of 5", {
        vector *f5 = factors_of(5);
        assert_that_int(vector_get(f5, 0) equals to 5);
    });

    it("finds the factors of 6", {
        vector *f6 = factors_of(6);
        assert_that_int(vector_get(f6, 0) equals to 2);
        assert_that_int(vector_get(f6, 1) equals to 3);
    });

    it("finds the factors of 7", {
        vector *f7 = factors_of(7);
        assert_that_int(vector_get(f7, 0) equals to 7);
    });
    
    it("finds the factors of 8", {
        vector *f8 = factors_of(8);
        assert_that_int(vector_get(f8, 0) equals to 2);
        assert_that_int(vector_get(f8, 1) equals to 2);
        assert_that_int(vector_get(f8, 2) equals to 2);
    });

    it("finds the factors of 9", {
        vector *f9 = factors_of(9);
        assert_that_int(vector_get(f9, 0) equals to 3);
        assert_that_int(vector_get(f9, 1) equals to 3);
    });

    it("finds the factors of 2*2*3*3*5*7*11*11*13", {
        vector *f = factors_of(2*2*3*3*5*7*11*11*13);
        assert_that_int(vector_get(f, 0) equals to 2);
        assert_that_int(vector_get(f, 1) equals to 2);
        assert_that_int(vector_get(f, 2) equals to 3);
        assert_that_int(vector_get(f, 3) equals to 3);
        assert_that_int(vector_get(f, 4) equals to 5);
        assert_that_int(vector_get(f, 5) equals to 7);
        assert_that_int(vector_get(f, 6) equals to 11);
        assert_that_int(vector_get(f, 7) equals to 11);
        assert_that_int(vector_get(f, 8) equals to 13);
    });
});

#endif
