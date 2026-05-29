#ifndef _PRIMES_MODULE_H_
#define _PRIMES_MODULE_H_

#include "../src/cSpec.h"

typedef struct prime_factors_vector {
  int *items;
  size_t alloced;
  size_t len;
} prime_factors_vector;

static const size_t prime_factors_vector_init_capacity = 32;

static void
prime_factors_vector_ensure_space(prime_factors_vector *v, size_t capacity) {
  int *items;

  if(v == NULL || capacity == 0) {
    return;
  }

  items = (int *)realloc(v->items, sizeof(int) * capacity);
  if(items != NULL) {
    v->items   = items;
    v->alloced = capacity;
  }
}

static prime_factors_vector *prime_factors_vector_create(void) {
  prime_factors_vector *v =
    (prime_factors_vector *)malloc(sizeof(prime_factors_vector));
  v->alloced = prime_factors_vector_init_capacity;
  v->len     = 0;
  v->items   = (int *)malloc(sizeof(int) * v->alloced);
  return v;
}

static void prime_factors_vector_add(prime_factors_vector *v, int item) {
  if(v == NULL) {
    return;
  }
  if(v->alloced == v->len) {
    prime_factors_vector_ensure_space(v, v->alloced * 2);
  }
  v->items[v->len++] = item;
}

static int prime_factors_vector_get(prime_factors_vector *v, size_t index) {
  if(v == NULL) {
    return 0;
  }
  if(index < v->len) {
    return v->items[index];
  }
  return 0;
}

static size_t prime_factors_vector_length(prime_factors_vector *v) {
  if(v == NULL) {
    return 0;
  }
  return v->len;
}

static void prime_factors_vector_free(prime_factors_vector *v) {
  free(v->items);
  free(v);
}

static prime_factors_vector *factors_of(int remainder) {
  prime_factors_vector *factors = prime_factors_vector_create();
  long divisor                  = 2;

  while(remainder > 1) {
    while(remainder % divisor == 0) {
      prime_factors_vector_add(factors, (int)divisor);
      remainder /= divisor;
    }
    divisor++;
  }
  return factors;
}

module(T_primes, {
  it("finds the factors of 1", {
    prime_factors_vector *f1 = factors_of(1);
    assert_that(prime_factors_vector_length(f1) is 0);
    prime_factors_vector_free(f1);
  });

  it("finds the factors of 2", {
    prime_factors_vector *f2 = factors_of(2);
    assert_that(prime_factors_vector_get(f2, 0) is 2);
    prime_factors_vector_free(f2);
  });

  it("finds the factors of 3", {
    prime_factors_vector *f3 = factors_of(3);
    assert_that(prime_factors_vector_get(f3, 0) is 3);
    prime_factors_vector_free(f3);
  });

  it("finds the factors of 4", {
    prime_factors_vector *f4 = factors_of(4);
    assert_that_int(prime_factors_vector_get(f4, 0) equals to 2);
    assert_that_int(prime_factors_vector_get(f4, 1) equals to 2);
    prime_factors_vector_free(f4);
  });

  it("finds the factors of 5", {
    prime_factors_vector *f5 = factors_of(5);
    assert_that_int(prime_factors_vector_get(f5, 0) equals to 5);
    prime_factors_vector_free(f5);
  });

  it("finds the factors of 6", {
    prime_factors_vector *f6 = factors_of(6);
    assert_that_int(prime_factors_vector_get(f6, 0) equals to 2);
    assert_that_int(prime_factors_vector_get(f6, 1) equals to 3);
    prime_factors_vector_free(f6);
  });

  it("finds the factors of 7", {
    prime_factors_vector *f7 = factors_of(7);
    assert_that_int(prime_factors_vector_get(f7, 0) equals to 7);
    prime_factors_vector_free(f7);
  });

  it("finds the factors of 8", {
    prime_factors_vector *f8 = factors_of(8);
    assert_that_int(prime_factors_vector_get(f8, 0) equals to 2);
    assert_that_int(prime_factors_vector_get(f8, 1) equals to 2);
    assert_that_int(prime_factors_vector_get(f8, 2) equals to 2);
    prime_factors_vector_free(f8);
  });

  it("finds the factors of 9", {
    prime_factors_vector *f9 = factors_of(9);
    assert_that_int(prime_factors_vector_get(f9, 0) equals to 3);
    assert_that_int(prime_factors_vector_get(f9, 1) equals to 3);
    prime_factors_vector_free(f9);
  });

  it("finds the factors of 2*2*3*3*5*7*11*11*13", {
    prime_factors_vector *f = factors_of(2 * 2 * 3 * 3 * 5 * 7 * 11 * 11 * 13);
    assert_that_int(prime_factors_vector_get(f, 0) equals to 2);
    assert_that_int(prime_factors_vector_get(f, 1) equals to 2);
    assert_that_int(prime_factors_vector_get(f, 2) equals to 3);
    assert_that_int(prime_factors_vector_get(f, 3) equals to 3);
    assert_that_int(prime_factors_vector_get(f, 4) equals to 5);
    assert_that_int(prime_factors_vector_get(f, 5) equals to 7);
    assert_that_int(prime_factors_vector_get(f, 6) equals to 11);
    assert_that_int(prime_factors_vector_get(f, 7) equals to 11);
    assert_that_int(prime_factors_vector_get(f, 8) equals to 13);
    prime_factors_vector_free(f);
  });
})

#endif
