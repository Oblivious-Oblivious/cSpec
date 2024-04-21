#ifndef __VECTOR_H_
#define __VECTOR_H_

#include <stdlib.h>

static const size_t vector_init_capacity = 32;

typedef struct vector {
  int *items;
  size_t alloced;
  size_t len;
} vector;

static void vector_ensure_space(vector *v, size_t capacity);

vector *new_vector(void);
void vector_add(vector *v, int item);
void vector_set(vector *v, size_t index, int item);
int vector_get(vector *v, size_t index);
void vector_delete(vector *v, size_t index);
size_t vector_length(vector *v);

#endif
