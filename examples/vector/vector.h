#ifndef __VECTOR_H_
#define __VECTOR_H_

#include <stdio.h> /* printf, snprintf */
#include <stdlib.h> /* malloc, calloc, realloc, free */
#include <string.h> /* memmove, strlen */

#ifndef bool
  #define bool unsigned char
  #define true 1
  #define false 0
#endif

static const size_t vector_init_capacity = 32;

typedef struct vector {
  int *items;
  size_t alloced;
  size_t len;
} vector;

static void vector_ensure_space(vector *v, size_t capacity);

vector *vector_create(void);
#define new_vector() vector_create()

void vector_add(vector *v, int item);
void vector_set(vector *v, size_t index, int item);
int vector_get(vector *v, size_t index);
void vector_delete(vector *v, size_t index);
size_t vector_length(vector *v);

#endif
