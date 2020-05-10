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
    /* TODO We allow NULL elements (NOT TESTED) */
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
    for(int i = index; i < v->len - 1; i++) {
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

vector *vector_dup(vector *v) {
    if(v == NULL) return NULL;

    vector *dup = new_vector();
    
    /* Iteratively copy the vector items from one memory location to another */
    for(size_t i = 0; i < vector_length(v); i++)
        vector_add(dup, vector_get(v, i));

    return dup;
}

vector *vector_map(vector *v, lambda modifier) {
    if(v == NULL || modifier == NULL) return NULL;

    vector *dup = new_vector();

    for(size_t i = 0; i < vector_length(v); i++) {
        /* Pass each element through the modifier and add it to the new vector */
        vector_add(dup, modifier(vector_get(v, i)));
    }

    return dup;
}

vector *vector_filter(vector *v, lambda filter) {
    if(v == NULL || filter == NULL) return NULL;

    vector *dup = new_vector();
    
    for(size_t i = 0; i < vector_length(v); i++) {
        void *item = vector_get(v, i);
        /* If the item passes the filter it gets added to the dup vector */
        if(filter(item))
            vector_add(dup, item);
    }

    return dup;
}

void *vector_reduce(vector *v, lambda2 fold) {
    if(v == NULL || fold == NULL) return NULL;

    /* Get the initial value that gets returned
        with the accumulation of the vector elements */
    void *accumulator = vector_get(v, 0);

    for(size_t i = 1; i < vector_length(v); i++) {
        /* Accumulate the current item */
        void *current = vector_get(v, i);
        accumulator = fold(accumulator, current);
    }
    
    return accumulator;
}