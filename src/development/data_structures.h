#ifndef __DATA_STRUCTURES_H_
#define __DATA_STRUCTURES_H_

/***** DATA STRUCTURES *****/

/** @param bool -> A 'big' enough size to hold both 1 and 0 **/
#ifndef bool
    typedef unsigned char bool;
    #define true 1
    #define false 0
#endif

/**
 * @func: _lambda
 * @desc: A generic function type used upon iterable data structures
 * @param -> An element belonging to an iterable
 * @return -> A value that satisfies the callee's purpose (map, filter, reduce)
 **/
typedef void* (*_lambda)(void*);

/**
 * @struct: _string
 * @desc: A mutable string of characters used to dynamically build a string.
 * @param str -> The str char* we construct our string into
 * @param alloced -> The total sized allocated for the string
 * @param length -> The total length of the string
 * @param persistance -> A flag signaling the persistence state of the string
 **/
typedef struct _string {
    char *str;
    size_t alloced;
    size_t length;
} _string;

/**
 * @struct: _vector
 * @desc: Defines a vector data structure
 * @param items -> A void pointer array that contains the heterogenous elements of the vector
 * @param alloced -> The total capacity of the vector
 * @param length -> The total number of values
 **/
typedef struct _vector {
    void **items;
    size_t alloced;
    size_t length;
} _vector;

/**
 * @struct: _cspec-_data_struct
 * @desc: Global variables grouped in container
 * @param number_of_tests -> The total number of tests performed
 * @param number_of_passing_tests -> Counts the passing tests
 * @param number_of_failing_tests -> Counts the failing tests
 * @param number_of_skipped_tests -> Counts the skipped tests
 * @param status_of_test -> Either _PASSING|_FAILING|_SKIPPED
 * @param fd -> A file descriptor used for saving test results
 * @param total_time_taken_for_tests
 * @param signals_vector -> A vector saving descriptions about signals
 * @param test_result_message -> The string builder we construct for assertions
 * @param name_of_tested_proc -> The current name of the it block being tested
 * @param name_of_describe -> The current name of the describe block
 * @param name_of_context -> The current name of the context block
 * @param name_of_module -> The current name of the module block
 * @param display_tab -> A 4 space overhead used for a nicer display of test results
 * @param inner_nest -> Counts the times we enter a nested block
 * @param outer_nest -> Counts the times we exit a nested block
 * @param type_of_tests -> The type of tests we want to display
 * @param type_of_export_tests -> The type of tests we want to export
 * @param current_assert -> The current assert token (only used in `assert_that`)
 * @param current_file -> Current value of the __FILE__ macro used for tracking assert positions in the file
 * @param current_actual -> Current actual value token
 * @param current_expected -> Current expected value token
 * @param position_in_file -> A string builder containing __FILE__ and __LINE__ results
 * @param assert_result -> A string builder containing the result description of the current assert
 * @param current_line -> Current value of the __LINE__ macro
 * @param in_context_block -> A boolean signaling if we are testing a context block
 * @param before_func -> A function pointer to be executed before it blocks
 * @param after_func -> A function pointer to be executed after it blocks
 * @param COLORS -> Terminal string color codes
 * @param list_of_modules -> A vector containing data about modules
 * @param list_of_describes -> A vector containing data about describes, part of list_of_modules
 * @param list_of_contexts -> A vector containing data about contexts, part of list_of_describes
 * @param list_of_its -> A vector containing data about it block, part of list_of_contexts
 * @param list_of_asserts -> A vector containing data about asserts, part of list_of_its
 **/
typedef struct _cspec_data_struct {
    int number_of_tests;
    int number_of_passing_tests;
    int number_of_failing_tests;
    int number_of_skipped_tests;
    int status_of_test;
    FILE *fd;
    // jmp_buf escape;

    size_t total_time_taken_for_tests;
    _vector *signals_vector;

    _string *test_result_message;
    _string *name_of_tested_proc;
    _string *name_of_describe;
    _string *name_of_context;
    _string *name_of_module;
    _string *display_tab;
    int inner_nest;
    int outer_nest;

    _string *type_of_tests;
    _string *type_of_export_tests;
    _string *current_assert;
    _string *current_file;
    _string *current_actual;
    _string *current_expected;
    _string *position_in_file;
    _string *assert_result;
    int current_line;
    bool in_context_block;

    void (*before_func)(void);
    void (*after_func)(void);

    /** Colors **/
    _string *GREEN;
    _string *RED;
    _string *YELLOW;
    _string *PURPLE;
    _string *CYAN;
    _string *GRAY;
    _string *WHITE;
    _string *RESET;
    _string *BACK_PURPLE;

    _vector *list_of_modules;
    _vector *list_of_describes;
    _vector *list_of_contexts;
    _vector *list_of_its;
    _vector *list_of_asserts;
} _cspec_data_struct;

_cspec_data_struct *_cspec;

/**
 * @func: __streql
 * @desc: Accurate equivalent of string.h 'strcmp' function
 * @param a -> The first string to compare
 * @param b -> The second string to compare
 * @return A bool signaling weather the strings are equal or not
 **/
static bool __streql(const char *a, const char *b) {
    while(*a && *a == *b) {
        ++a;
        ++b;
    }
    if((int)(unsigned char)(*a) - (int)(unsigned char)(*b) == 0)
        return true;
    return false;
}

/**
 * @func: __memmove
 * @desc: Accurate equivalent of string.h 'memmove' function
 * @param dest -> The dest pointer to move memory to
 * @param str -> The source string to copy from
 * @param n -> The size of the source string
 **/
static void *__memmove(void *dest, const void *src, size_t n) {
    unsigned char *pd = (unsigned char*) dest;
    const unsigned char *ps = (unsigned char*)src;

    if(ps < pd)
        for(pd += n, ps += n; n--;)
            *--pd = *--ps;
    else
        while(n--)
            *pd++ = *ps++;

    return dest;
}

/**
 * @func: _string_ensure_space
 * @desc: Ensure there is enough space for data being added plus a NULL terminator
 * @param sb -> The string builder to use
 * @param add_len -> he length that needs to be added *not* including a NULL terminator
 **/
static void _string_ensure_space(_string *sb, size_t add_len) {
    if(sb == NULL || add_len == 0) return;

    /* If out allocated space is big enough */
    if(sb->alloced >= sb->length + add_len + 1) return;

    while(sb->alloced < sb->length + add_len + 1) {
        /* Doubling growth strategy */
        sb->alloced <<= 1;
        if(sb->alloced == 0) {
            /* Left shift of max bits will go to 0. An unsigned type set to
             * -1 will return the maximum possible size. However, we should
             *  have run out of memory well before we need to do this. Since
             *  this is the theoretical maximum total system memory we don't
             *  have a flag saying we can't grow any more because it should
             *  be impossible to get to this point */
            sb->alloced--;
        }
    }
    sb->str = realloc(sb->str, sb->alloced);
}

/**
 * @func: _string_add_str
 * @desc: Add a string to the builder
 * @param sb -> The string builder to use
 * @param str -> The string to add
 **/
static void _string_add_str(_string *sb, const char *str) {
    if(sb == NULL || str == NULL || *str == '\0') return;
    
    const char *ptr = str;
    while(*ptr) ++ptr;
    size_t len = ptr - str;
    
    _string_ensure_space(sb, len);
    __memmove(sb->str+sb->length, str, len);

    /* Reset length and NULL terminate */
    sb->length += len;
    sb->str[sb->length] = '\0';
}

/**
 * @func: _new_string
 * @desc: Create an str builder
 * @param initial_string -> The initial string to set
 * @return The str builder
 **/
static _string *_new_string(char *initial_string) {
    _string *sb;
    sb = calloc(1, sizeof(*sb));
    sb->str = malloc(32);
    *sb->str = '\0';
    sb->alloced = 32;
    sb->length = 0;
    _string_add_str(sb, initial_string);
    return sb;
}

/**
 * @func: _string_add_int
 * @desc: Add an integer to the builder
 * @param sb -> The string builder to use
 * @param val -> The integer to add
 **/
static void _string_add_int(_string *sb, int val) {
    char str[1024];

    if(sb == NULL) return;

    snprintf(str, sizeof(str), "%d", val);
    _string_add_str(sb, str);
}

/**
 * @func: _string_add_double_precision
 * @desc: Add a double to the builder
 * @param sb -> The string builder to use
 * @param val -> The double to add
 **/
static void _string_add_double_precision(_string *sb, double val) {
    char str[1024];

    if(sb == NULL) return;

    /* Use %g for minimum precision on printing floats */
    snprintf(str, sizeof(str), "%g", val);
    _string_add_str(sb, str);
}

/**
 * @func: _string_get
 * @desc: A pointer to the internal buffer with the builder's stirng data
 * @param sb -> The string builder to use
 * @return A pointer to the internal __string data
 **/
static char *_string_get(_string *sb) {
    if(sb == NULL) return NULL;
    return sb->str;
}

/**
 * @func: _string_shorten
 * @desc: Remove data from the end of the builder
 * @param sb -> The string builder to use
 * @param len -> The new length of the string, anything after this length is removed
 **/
static void _string_shorten(_string *sb, size_t len) {
    if(sb == NULL || len >= sb->length) return;

    /* Reset the length and NULL terminate, ingoring
        all values right to the NULL from memory */
    sb->length = len;
    sb->str[sb->length] = '\0';
}

/**
 * @func: _string_delete
 * @desc: Clear the builder
 * @param sb -> The string builder to use
 **/
static void _string_delete(_string *sb) {
    if(sb == NULL) return;
    _string_shorten(sb, 0);
}

/**
 * @func: _string_skip
 * @desc: Remove data from the beginning of the builder
 * @param sb -> The _string builder to use
 * @param len -> The length to remove
 **/
static void _string_skip(_string *sb, size_t len) {
    if(sb == NULL || len == 0) return;

    if(len >= sb->length) {
        /* If we choose to drop more bytes than the
            string has simply clear the string */
        _string_delete(sb);
        return;
    }

    sb->length -= len;

    /* +1 to move the NULL. */
    __memmove(sb->str, sb->str + len, sb->length + 1);
}

/**
 * @func: _string_length
 * @desc: The length of the string contained in the builder
 * @param sb -> The string builder to use
 * @return The current length of the string
 **/
static size_t _string_length(_string *sb) {
    if(sb == NULL) return 0;
    return sb->length;
}

/**
 * @func: _string_dup
 * @desc: Return a memory duplicate string
 * @param sb -> The string to duplicate
 * @return The dup string
 **/
static _string *_string_dup(_string *sb) {
    if(sb == NULL) return NULL;

    _string *dup = _new_string("");
    _string_add_str(dup, _string_get(sb));
    return dup;
}

/**
 * @func: _string_substring
 * @desc: Return a substring of our current string without modifying the original
 * @param str -> The string builder we are using
 * @param from -> The point where we start our substring
 * @param to -> The point where we end our substring
 * @return A substring of a copy of the original string
 **/
static _string *_string_substring(_string *str, size_t from, size_t _to_) {
    _string *strdup = _string_dup(str);
    _string_skip(strdup, from);
    _string_shorten(strdup, _to_ - from + 1);
    return strdup;
}

/**
 * @func: _vector_ensure_space
 * @desc: Ensure there is enough space for our values in the vector
 * @param v -> The vector to use
 * @param capacity -> The new capacity to set
 **/
static void _vector_ensure_space(_vector *v, size_t capacity) {
    if(v == NULL || capacity == 0) return;

    /* Attempt to reallocate new memory in the items list */
    void **items = realloc(v->items, sizeof(void*) * capacity);

    if(items) {
        /* Reset the items in the new memory space */
        v->items = items;
        v->alloced = capacity;
    }
}

/**
 * @func: _new_vector
 * @desc: Initializes a vector data structure
 * @return: The newly created vector
 **/
static _vector *_new_vector(void) {
    _vector *v = malloc(sizeof(_vector));
    v->alloced = 32;
    v->length = 0;
    v->items = malloc(sizeof(void*) * v->alloced);
    return v;
}

/**
 * @func: _vector_add
 * @desc: Adds a new element in the vector
 * @param v -> The vector to use
 * @param item -> The item to add
 **/
static void _vector_add(_vector *v, void *item) {
    if(v == NULL) return;
    if(v->alloced == v->length)
        _vector_ensure_space(v, v->alloced * 2);
    v->items[v->length++] = item;
}

/**
 * @func: _vector_get
 * @desc: Get the value of a specific vector index
 * @param v -> The vector to use
 * @param index -> The index to get the value of
 * @return The value
 **/
static void *_vector_get(_vector *v, size_t index) {
    if(v == NULL) return NULL;
    if(index >= 0 && index < v->length)
        return v->items[index];
    return NULL;
}

/**
 * @func: _vector_length
 * @desc: Get the total number of values inserted in the vector
 * @param v -> The vector to use
 * @return: The number of items in the vector
 **/
static size_t _vector_length(_vector *v) {
    if(v == NULL) return 0;
    return v->length;
}

/**
 * @func: vector_dup
 * @desc: Get a memory duplicate of the passed vector
 * @param v -> The vector to use
 * @return The duplicate vector
 **/
static _vector *_vector_dup(_vector *v) {
    if(v == NULL) return NULL;

    _vector *dup = _new_vector();

    /* Iteratively copy the vector items from one memory location to another */
    for(size_t i = 0; i < _vector_length(v); i++)
        _vector_add(dup, _vector_get(v, i));
    return dup;
}

/**
 * @func: _vector_map
 * @desc: Maps all vector elements in iteration using a modifier function pointer
 * @param v -> The vector to map
 * @param modifier -> The modifier function
 * @return The mapped vector duplicate
 **/
static _vector *_vector_map(_vector *v, _lambda modifier) {
    if(v == NULL || modifier == NULL) return NULL;

    _vector *dup = _new_vector();

    for(size_t i = 0; i < _vector_length(v); i++)
        /* Pass each element through the modifier and add it to the new vector */
        _vector_add(dup, modifier(_vector_get(v, i)));
    return dup;
}

#endif
