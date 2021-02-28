#ifndef __DATA_STRUCTURES_H_
#define __DATA_STRUCTURES_H_

/* #include "includes.h" */

/** @param bool -> A 'big' enough size to hold both 1 and 0 **/
#ifndef bool
    #define bool unsigned char
    #define true 1
    #define false 0
#endif

/**
 * @func: cspec_lambda
 * @desc: A generic function type used upon iterable data structures
 * @param -> An element belonging to an iterable
 * @return -> A value that satisfies the callee's purpose (map, filter, reduce)
 **/
typedef void* (*cspec_lambda)(void*);

/**
 * @struct: cspec_string
 * @desc: A mutable string of characters used to dynamically build a string.
 * @param str -> The str char* we construct our string into
 * @param alloced -> The total sized allocated for the string
 * @param len -> The total len of the string
 * @param persistance -> A flag signaling the persistence state of the string
 **/
typedef struct cspec_string {
    char *str;
    size_t alloced;
    size_t len;
} cspec_string;

/**
 * @struct: cspec_vector
 * @desc: Defines a vector data structure
 * @param items -> A void pointer array that contains the heterogenous elements of the vector
 * @param alloced -> The total capacity of the vector
 * @param len -> The total number of values
 **/
typedef struct cspec_vector {
    void **items;
    size_t alloced;
    size_t len;
} cspec_vector;

/**
 * @struct: cspec_data_struct
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
typedef struct cspec_data_struct {
    int number_of_tests;
    int number_of_passing_tests;
    int number_of_failing_tests;
    int number_of_skipped_tests;
    int status_of_test;
    FILE *fd;
    /* jmp_buf escape; */

    size_t total_time_taken_for_tests;
    cspec_vector *signals_vector;

    cspec_string *test_result_message;
    cspec_string *name_of_tested_proc;
    cspec_string *name_of_describe;
    cspec_string *name_of_context;
    cspec_string *name_of_module;
    cspec_string *display_tab;
    int inner_nest;
    int outer_nest;

    cspec_string *type_of_tests;
    cspec_string *type_of_export_tests;
    cspec_string *current_assert;
    char *current_file;
    cspec_string *current_actual;
    cspec_string *current_expected;
    cspec_string *position_in_file;
    cspec_string *assert_result;
    int current_line;
    bool in_context_block;

    void (*before_func)(void);
    void (*after_func)(void);

    /** Colors **/
    cspec_string *GREEN;
    cspec_string *RED;
    cspec_string *YELLOW;
    cspec_string *PURPLE;
    cspec_string *CYAN;
    cspec_string *GRAY;
    cspec_string *WHITE;
    cspec_string *RESET;
    cspec_string *BACK_PURPLE;

    cspec_vector *list_of_modules;
    cspec_vector *list_of_describes;
    cspec_vector *list_of_contexts;
    cspec_vector *list_of_its;
    cspec_vector *list_of_asserts;
} cspec_data_struct;

cspec_data_struct *cspec;

/**
 * @func: cspec_fabs
 * @desc: Abs for floats
 * @param value -> The value to get `abs` for
 * @return Absolute value
 **/
static double cspec_fabs(double value) {
    return value < 0 ? (-value) : (value);
}

/**
 * @func: cspec_streql
 * @desc: Accurate equivalent of string.h 'strcmp' function
 * @param a -> The first string to compare
 * @param b -> The second string to compare
 * @return A bool signaling weather the strings are equal or not
 **/
static bool cspec_streql(const char *a, const char *b) {
    while(*a && *a == *b) {
        ++a;
        ++b;
    }
    if((int)(unsigned char)(*a) - (int)(unsigned char)(*b) == 0)
        return true;
    return false;
}

/**
 * @func: cspec_memmove
 * @desc: Accurate equivalent of string.h 'memmove' function
 * @param dest -> The dest pointer to move memory to
 * @param str -> The source string to copy from
 * @param n -> The size of the source string
 **/
static void *cspec_memmove(void *dest, const void *src, size_t n) {
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
 * @func: cspec_string_ensure_space
 * @desc: Ensure there is enough space for data being added plus a NULL terminator
 * @param sb -> The string builder to use
 * @param add_len -> he len that needs to be added *not* including a NULL terminator
 **/
static void cspec_string_ensure_space(cspec_string *sb, size_t add_len) {
    if(sb == NULL || add_len == 0) return;

    /* If out allocated space is big enough */
    if(sb->alloced >= sb->len + add_len + 1) return;

    while(sb->alloced < sb->len + add_len + 1) {
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
    sb->str = (char*)realloc(sb->str, sb->alloced);
}

/**
 * @func: cspec_string_add_str
 * @desc: Add a string to the builder
 * @param sb -> The string builder to use
 * @param str -> The string to add
 **/
static void cspec_string_add_str(cspec_string *sb, const char *str) {
    const char *ptr;
    size_t len;
    
    if(sb == NULL || str == NULL || *str == '\0') return;
    
    ptr = str;
    while(*ptr) ++ptr;
    len = ptr - str;
    
    cspec_string_ensure_space(sb, len);
    cspec_memmove(sb->str+sb->len, str, len);

    /* Reset len and NULL terminate */
    sb->len += len;
    sb->str[sb->len] = '\0';
}

/**
 * @func: cspec_string_new
 * @desc: Create an str builder
 * @param initial_string -> The initial string to set
 * @return The str builder
 **/
static cspec_string *cspec_string_new(const char *initial_string) {
    cspec_string *sb;
    sb = (cspec_string*)calloc(1, sizeof(*sb));
    sb->str = (char*)malloc(32);
    *sb->str = '\0';
    sb->alloced = 32;
    sb->len = 0;
    cspec_string_add_str(sb, initial_string);
    return sb;
}

/**
 * @func: cspec_string_add_int
 * @desc: Add an integer to the builder
 * @param sb -> The string builder to use
 * @param val -> The integer to add
 **/
static void cspec_string_add_int(cspec_string *sb, int val) {
    char str[1024];

    if(sb == NULL) return;

    snprintf(str, sizeof(str), "%d", val);
    cspec_string_add_str(sb, str);
}

/**
 * @func: cspec_string_add_double_precision
 * @desc: Add a double to the builder
 * @param sb -> The string builder to use
 * @param val -> The double to add
 **/
static void cspec_string_add_double_precision(cspec_string *sb, double val) {
    char str[1024];

    if(sb == NULL) return;

    /* Use %g for minimum precision on printing floats */
    snprintf(str, sizeof(str), "%g", val);
    cspec_string_add_str(sb, str);
}

/**
 * @func: cspec_string_get
 * @desc: A pointer to the internal buffer with the builder's stirng data
 * @param sb -> The string builder to use
 * @return A pointer to the internal cspec_string data
 **/
static char *cspec_string_get(cspec_string *sb) {
    if(sb == NULL) return NULL;
    return sb->str;
}

/**
 * @func: cspec_string_shorten
 * @desc: Remove data from the end of the builder
 * @param sb -> The string builder to use
 * @param len -> The new len of the string, anything after this len is removed
 **/
static void cspec_string_shorten(cspec_string *sb, size_t len) {
    if(sb == NULL || len >= sb->len) return;

    /* Reset the len and NULL terminate, ingoring
        all values right to the NULL from memory */
    sb->len = len;
    sb->str[sb->len] = '\0';
}

/**
 * @func: cspec_string_delete
 * @desc: Clear the builder
 * @param sb -> The string builder to use
 **/
static void cspec_string_delete(cspec_string *sb) {
    if(sb == NULL) return;
    cspec_string_shorten(sb, 0);
}

/**
 * @func: cspec_string_skip
 * @desc: Remove data from the beginning of the builder
 * @param sb -> The cspec_string builder to use
 * @param len -> The len to remove
 **/
static void cspec_string_skip(cspec_string *sb, size_t len) {
    if(sb == NULL || len == 0) return;

    if(len >= sb->len) {
        /* If we choose to drop more bytes than the
            string has simply clear the string */
        cspec_string_delete(sb);
        return;
    }

    sb->len -= len;

    /* +1 to move the NULL. */
    cspec_memmove(sb->str, sb->str + len, sb->len + 1);
}

/**
 * @func: cspec_string_length
 * @desc: The len of the string contained in the builder
 * @param sb -> The string builder to use
 * @return The current len of the string
 **/
static size_t cspec_string_length(cspec_string *sb) {
    if(sb == NULL) return 0;
    return sb->len;
}

/**
 * @func: cspec_string_dup
 * @desc: Return a memory duplicate string
 * @param sb -> The string to duplicate
 * @return The dup string
 **/
static cspec_string *cspec_string_dup(cspec_string *sb) {
    cspec_string *dup;
    if(sb == NULL) return NULL;

    dup = cspec_string_new("");
    cspec_string_add_str(dup, cspec_string_get(sb));
    return dup;
}

/**
 * @func: cspec_string_substring
 * @desc: Return a substring of our current string without modifying the original
 * @param str -> The string builder we are using
 * @param from -> The point where we start our substring
 * @param to -> The point where we end our substring
 * @return A substring of a copy of the original string
 **/
static cspec_string *cspec_string_substring(cspec_string *str, size_t str_from, size_t str_to) {
    cspec_string *strdup = cspec_string_dup(str);
    cspec_string_skip(strdup, str_from);
    cspec_string_shorten(strdup, str_to - str_from + 1);
    return strdup;
}

/**
 * @func: cspec_vector_ensure_space
 * @desc: Ensure there is enough space for our values in the vector
 * @param v -> The vector to use
 * @param capacity -> The new capacity to set
 **/
static void cspec_vector_ensure_space(cspec_vector *v, size_t capacity) {
    void **items;
    if(v == NULL || capacity == 0) return;

    /* Attempt to reallocate new memory in the items list */
    items = (void**)realloc(v->items, sizeof(void*) * capacity);

    if(items) {
        /* Reset the items in the new memory space */
        v->items = items;
        v->alloced = capacity;
    }
}

/**
 * @func: cspec_vector_new
 * @desc: Initializes a vector data structure
 * @return: The newly created vector
 **/
static cspec_vector *cspec_vector_new(void) {
    cspec_vector *v = (cspec_vector *)malloc(sizeof(cspec_vector));
    v->alloced = 32;
    v->len = 0;
    v->items = (void**)malloc(sizeof(void*) * v->alloced);
    return v;
}

/**
 * @func: cspec_vector_add
 * @desc: Adds a new element in the vector
 * @param v -> The vector to use
 * @param item -> The item to add
 **/
static void cspec_vector_add(cspec_vector *v, void *item) {
    if(v == NULL) return;
    if(v->alloced == v->len)
        cspec_vector_ensure_space(v, v->alloced * 2);
    v->items[v->len++] = item;
}

/**
 * @func: cspec_vector_get
 * @desc: Get the value of a specific vector index
 * @param v -> The vector to use
 * @param index -> The index to get the value of
 * @return The value
 **/
static void *cspec_vector_get(cspec_vector *v, size_t index) {
    if(v == NULL) return NULL;
    if(index >= 0 && index < v->len)
        return v->items[index];
    return NULL;
}

/**
 * @func: cspec_vector_length
 * @desc: Get the total number of values inserted in the vector
 * @param v -> The vector to use
 * @return: The number of items in the vector
 **/
static size_t cspec_vector_length(cspec_vector *v) {
    if(v == NULL) return 0;
    return v->len;
}

/**
 * @func: vector_dup
 * @desc: Get a memory duplicate of the passed vector
 * @param v -> The vector to use
 * @return The duplicate vector
 **/
static cspec_vector *cspec_vector_dup(cspec_vector *v) {
    size_t i;
    cspec_vector *dup;

    if(v == NULL) return NULL;

    dup = cspec_vector_new();

    /* Iteratively copy the vector items from one memory location to another */
    for(i = 0; i < cspec_vector_length(v); i++)
        cspec_vector_add(dup, cspec_vector_get(v, i));
    return dup;
}

/**
 * @func: cspec_vector_map
 * @desc: Maps all vector elements in iteration using a modifier function pointer
 * @param v -> The vector to map
 * @param modifier -> The modifier function
 * @return The mapped vector duplicate
 **/
static cspec_vector *cspec_vector_map(cspec_vector *v, cspec_lambda modifier) {
    size_t i;
    cspec_vector *dup;
    if(v == NULL || modifier == NULL) return NULL;

    dup = cspec_vector_new();

    for(i = 0; i < cspec_vector_length(v); i++)
        /* Pass each element through the modifier and add it to the new vector */
        cspec_vector_add(dup, modifier(cspec_vector_get(v, i)));
    return dup;
}

#endif
