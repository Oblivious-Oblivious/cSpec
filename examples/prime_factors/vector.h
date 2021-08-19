#ifndef __VECTOR_H_
#define __VECTOR_H_

#include <stdlib.h> /* malloc, calloc, realloc, free */

/* Initial capacity of a vector */
static const size_t vector_init_capacity = 32;

/**
 * @struct: vector
 * @desc: Defines a vector data structure
 * @param items -> A void pointer array that contains the heterogenous elements of the vector
 * @param alloced -> The total capacity of the vector
 * @param length -> The total number of values
 **/
typedef struct vector {
    void **items;
    size_t alloced;
    size_t len;
} vector;

/**
 * @func: vector_ensure_space
 * @desc: Ensure there is enough space for our values in the vector
 * @param v -> The vector to use
 * @param capacity -> The new capacity to set
 **/
static void vector_ensure_space(vector *v, size_t capacity);

/**
 * @func: new_vector
 * @desc: Initializes a vector data structure
 * @return: The newly created vector
 **/
vector *new_vector(void);

/**
 * @func: vector_add
 * @desc: Adds a new element in the vector
 * @param v -> The vector to use
 * @param item -> The item to add
 **/
void vector_add(vector *v, void *item);

/**
 * @func: vector_set
 * @desc: Set the value of a specific vector index to a new one
 * @param v-> The vector
 * @param index -> The index to set the value of
 * @param item -> The item to set the value as
 **/
void vector_set(vector *v, size_t index, void *item);

/**
 * @func: vector_get
 * @desc: Get the value of a specific vector index
 * @param v -> The vector to use
 * @param index -> The index to get the value of
 * @return The value
 **/
void *vector_get(vector *v, size_t index);

/**
 * @func: vector_delete
 * @desc: Delete a specific vector value by index
 * @param v -> The vector to use
 * @param index -> The index to delete
 **/
void vector_delete(vector *v, size_t index);

/**
 * @func: vector_length
 * @desc: Get the total number of values inserted in the vector
 * @param v -> The vector to use
 * @return: The number of items in the vector
 **/
size_t vector_length(vector *v);

#endif
