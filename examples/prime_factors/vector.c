#include "vector.h"

static void vector_ensure_space(vector *v, size_t capacity) {
  int *items;
  if(v == NULL || capacity == 0) {
    return;
  }

  /* Attempt to reallocate new memory in the items list */
  items = realloc(v->items, sizeof(int) * capacity);

  if(items) {
    /* Reset the items in the new memory space */
    v->items   = items;
    v->alloced = capacity;
  }
}

vector *new_vector(void) {
  vector *v  = (vector *)malloc(sizeof(vector));
  v->alloced = vector_init_capacity;
  v->len     = 0;
  v->items   = (int *)malloc(sizeof(int) * v->alloced);
  return v;
}

void vector_add(vector *v, int item) {
  if(v == NULL) {
    return;
  }
  if(v->alloced == v->len) {
    vector_ensure_space(v, v->alloced * 2);
  }
  v->items[v->len++] = item;
}

void vector_set(vector *v, size_t index, int item) {
  if(v == NULL) {
    return;
  }
  if(index < v->len) {
    v->items[index] = item;
  }
}

int vector_get(vector *v, size_t index) {
  if(v == NULL) {
    return 0;
  }
  if(index < v->len) {
    return v->items[index];
  }
  return 0;
}

void vector_delete(vector *v, size_t index) {
  size_t i;
  if(v == NULL) {
    return;
  }
  if(index >= v->len) {
    return;
  }

  /* Reset the rest of the elements forwards */
  for(i = index; i < v->len - 1; i++) {
    v->items[i] = v->items[i + 1];
  }

  v->len--;

  if(v->len > 0 && v->len == v->alloced / 4) {
    vector_ensure_space(v, v->alloced / 2);
  }
}

size_t vector_length(vector *v) {
  if(v == NULL) {
    return 0;
  }
  return v->len;
}
