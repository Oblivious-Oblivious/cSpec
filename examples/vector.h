#ifndef __VECTOR_H_
#define __VECTOR_H_

#include <stddef.h> /* size_t */

#ifndef bool
  #define bool  unsigned char
  #define true  1
  #define false 0
#endif

typedef struct vector {
  int *items;
  size_t alloced;
  size_t len;
} vector;

vector *vector_create(void);
#define new_vector() vector_create()

void vector_add(vector *v, int item);
void vector_set(vector *v, size_t index, int item);
int vector_get(vector *v, size_t index);
void vector_delete(vector *v, size_t index);
size_t vector_length(vector *v);
void vector_free(vector *v);

#endif
