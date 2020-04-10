#ifndef __STRING_H_
#define __STRING_H_

#include <stdio.h> /* printf, snprintf */
#include <stdlib.h> /* malloc, calloc, realloc, free */
#include <string.h> /* memmove, strlen */

typedef unsigned char bool;
#define true 1
#define false 0

/** The initial minimum size of a string **/
static const size_t string_init_capacity = 32;


/**
 * @struct: string
 * @desc: A mutable string of characters used to dynamically build a string.
 * @param str -> The str char* we construct our string into
 * @param alloced -> The total sized allocated for the string
 * @param length -> The total length of the string
 * @param persistance -> A flag signaling the persistence state of the string
 **/
typedef struct string {
    char *str;
    size_t alloced;
    size_t length;
} string;

/**
 * @func: string_ensure_space
 * @desc: Ensure there is enough space for data being added plus a NULL terminator
 * @param sb -> The string builder to use
 * @param add_len -> he length that needs to be added *not* including a NULL terminator
 **/
static void string_ensure_space(string *sb, size_t add_len);

/**
 * @func: string_create
 * @desc: Create an str builder
 * @param initial_string -> The initial string to set
 * @param __persistence -> Dummy flag
 * @return The str builder
 **/
string *string_create(char *initial_string);
#define new_string(initial_string) string_create(initial_string)

/**
 * @func: string_add_str
 * @desc: Add a string to the builder
 * @param sb -> The string builder to use
 * @param str -> The string to add
 * @param len -> The length of the string to add.
 *              If 0, strlen will be called internally to determine length
 **/
void string_add_str(string *sb, const char *str);

/**
 * @func: string_add_char
 * @desc: Add a character to the builder
 * @param sb -> The string builder to use
 * @param c -> The character to add 
 **/
void string_add_char(string *sb, char c);

/**
 * @func: string_add_int
 * @desc: Add an integer to the builder
 * @param sb -> The string builder to use
 * @param val -> The integer to add
 **/
void string_add_int(string *sb, int val);

/**
 * @func: string_add_double_precision
 * @desc: Add a double to the builder
 * @param sb -> The string builder to use
 * @param val -> The double to add
 **/
void string_add_double_precision(string *sb, double val);

/**
 * @func: string_get
 * @desc: A pointer to the internal buffer with the builder's stirng data
 * @param sb -> The string builder to use
 * @return A pointer to the internal string data
 **/
char *string_get(string *sb);

/**
 * @func: string_get_char_at_index
 * @desc: Accessor to the string characters by index
 * @param sb -> The string builder to use
 * @param index -> The index of the character we want to receive
 * @return The character we searched for
 **/
char string_get_char_at_index(string *sb, size_t index);

/**
 * @func: string_shorten
 * @desc: Remove data from the end of the builder
 * @param sb -> The string builder to use
 * @param len -> The new length of the string, anything after this length is removed
 **/
void string_shorten(string *sb, size_t len);

/**
 * @func: string_delete
 * @desc: Clear the builder
 * @param sb -> The string builder to use
 **/
void string_delete(string *sb);

/**
 * @func: string_skip
 * @desc: Remove data from the beginning of the builder
 * @param sb -> The string builder to use
 * @param len -> The length to remove
 **/
void string_skip(string *sb, size_t len);

/**
 * @func: string_length
 * @desc: The length of the string contained in the builder
 * @param sb -> The string builder to use
 * @return The current length of the string
 **/
size_t string_length(string *sb);

#endif