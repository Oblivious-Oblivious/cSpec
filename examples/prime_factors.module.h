#ifndef _PRIMES_MODULE_H_
#define _PRIMES_MODULE_H_

#include "../src/cSpec.h"
#include "prime_factors.h"

module(primes, {
  it("finds the factors of 1", {
    vector *f1 = factors_of(1);
    assert_that(vector_length(f1) is 0);
    vector_free(f1);
  });

  it("finds the factors of 2", {
    vector *f2 = factors_of(2);
    assert_that(vector_get(f2, 0) is 2);
    vector_free(f2);
  });

  it("finds the factors of 3", {
    vector *f3 = factors_of(3);
    assert_that(vector_get(f3, 0) is 3);
    vector_free(f3);
  });

  it("finds the factors of 4", {
    vector *f4 = factors_of(4);
    assert_that_int(vector_get(f4, 0) equals to 2);
    assert_that_int(vector_get(f4, 1) equals to 2);
    vector_free(f4);
  });

  it("finds the factors of 5", {
    vector *f5 = factors_of(5);
    assert_that_int(vector_get(f5, 0) equals to 5);
    vector_free(f5);
  });

  it("finds the factors of 6", {
    vector *f6 = factors_of(6);
    assert_that_int(vector_get(f6, 0) equals to 2);
    assert_that_int(vector_get(f6, 1) equals to 3);
    vector_free(f6);
  });

  it("finds the factors of 7", {
    vector *f7 = factors_of(7);
    assert_that_int(vector_get(f7, 0) equals to 7);
    vector_free(f7);
  });

  it("finds the factors of 8", {
    vector *f8 = factors_of(8);
    assert_that_int(vector_get(f8, 0) equals to 2);
    assert_that_int(vector_get(f8, 1) equals to 2);
    assert_that_int(vector_get(f8, 2) equals to 2);
    vector_free(f8);
  });

  it("finds the factors of 9", {
    vector *f9 = factors_of(9);
    assert_that_int(vector_get(f9, 0) equals to 3);
    assert_that_int(vector_get(f9, 1) equals to 3);
    vector_free(f9);
  });

  it("finds the factors of 2*2*3*3*5*7*11*11*13", {
    vector *f = factors_of(2 * 2 * 3 * 3 * 5 * 7 * 11 * 11 * 13);
    assert_that_int(vector_get(f, 0) equals to 2);
    assert_that_int(vector_get(f, 1) equals to 2);
    assert_that_int(vector_get(f, 2) equals to 3);
    assert_that_int(vector_get(f, 3) equals to 3);
    assert_that_int(vector_get(f, 4) equals to 5);
    assert_that_int(vector_get(f, 5) equals to 7);
    assert_that_int(vector_get(f, 6) equals to 11);
    assert_that_int(vector_get(f, 7) equals to 11);
    assert_that_int(vector_get(f, 8) equals to 13);
    vector_free(f);
  });
})

#endif
