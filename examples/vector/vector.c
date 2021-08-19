#include "vector.h"

static void vector_ensure_space(vector *v, size_t capacity) {
    if(v == NULL || capacity == 0) return;

    /* Attempt to reallocate new memory in the items list */
    void **items = realloc(v->items, sizeof(void*) * capacity);

    if(items) {
        /* Reset the items in the new memory space */
        v->items = items;
        v->alloced = capacity;
    }
}

vector *vector_create(void) {
    vector *v = malloc(sizeof(vector));
    v->alloced = vector_init_capacity;
    v->len = 0;
    v->items = malloc(sizeof(void*) * v->alloced);
    return v;
}

void vector_add(vector *v, void *item) {
    if(v == NULL) return;
    if(v->alloced == v->len)
        vector_ensure_space(v, v->alloced * 2);
    v->items[v->len++] = item;
}

void vector_set(vector *v, size_t index, void *item) {
    if(v == NULL) return;
    if(index >= 0 && index < v->len)
        v->items[index] = item;
}

void *vector_get(vector *v, size_t index) {
    if(v == NULL) return NULL;
    if(index >= 0 && index < v->len)
        return v->items[index];
    return NULL;
}

void vector_delete(vector *v, size_t index) {
    if(v == NULL) return;
    if(index < 0 || index >= v->len) return;
    
    v->items[index] = NULL;

    /* Reset the rest of the elements forwards */
    for(size_t i = index; i < v->len - 1; i++) {
        v->items[i] = v->items[i + 1];
        v->items[i + 1] = NULL;
    }

    v->len--;

    if(v->len > 0 && v->len == v->alloced / 4)
        vector_ensure_space(v, v->alloced / 2);
}

size_t vector_length(vector *v) {
    if(v == NULL) return 0;
    return v->len;
};
