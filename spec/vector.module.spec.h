#ifndef __VECTOR_MODULE_SPEC_H_
#define __VECTOR_MODULE_SPEC_H_

#include "../src/cSpec.h"

typedef struct vector {
  int *items;
  size_t alloced;
  size_t len;
} vector;

static const size_t vector_init_capacity = 32;

static void vector_ensure_space(vector *v, size_t capacity) {
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

static vector *vector_create(void) {
  vector *v  = (vector *)malloc(sizeof(vector));
  v->alloced = vector_init_capacity;
  v->len     = 0;
  v->items   = (int *)malloc(sizeof(int) * v->alloced);
  return v;
}

#define new_vector() vector_create()

static void vector_add(vector *v, int item) {
  if(v == NULL) {
    return;
  }
  if(v->alloced == v->len) {
    vector_ensure_space(v, v->alloced * 2);
  }
  v->items[v->len++] = item;
}

static int vector_get(vector *v, size_t index) {
  if(v == NULL) {
    return 0;
  }
  if(index < v->len) {
    return v->items[index];
  }
  return 0;
}

static void vector_delete(vector *v, size_t index) {
  size_t i;

  if(v == NULL || index >= v->len) {
    return;
  }

  for(i = index; i < v->len - 1; i++) {
    v->items[i] = v->items[i + 1];
  }

  v->len--;

  if(v->len > 0 && v->len == v->alloced / 4) {
    vector_ensure_space(v, v->alloced / 2);
  }
}

static size_t vector_length(vector *v) {
  if(v == NULL) {
    return 0;
  }
  return v->len;
}

static void vector_free(vector *v) {
  free(v->items);
  free(v);
}

module(T_vector, {
  describe("vector", {
    vector *v;
    int a, b, c;

    before({
      v = NULL;
      a = 1;
      b = 2;
      c = 4;
    });

    it("creates a new vector", {
      v = new_vector();
      assert_that(v isnot NULL);
    });

    it("adds exactly three elements to the vector", {
      vector_add(v, a);
      vector_add(v, b);
      vector_add(v, c);
      assert_that_int(vector_length(v) equals to 3);
    });

    it("peeks the second element of the vector", {
      assert_that_int(vector_get(v, 1) equals to b);
    });

    it("removes the second element of the vector", {
      vector_delete(v, 1);
      assert_that_int(vector_get(v, 1) equals to c);
    });

    after({ vector_free(v); });
  });
})

#endif
