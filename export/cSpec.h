#ifndef __CSPEC_H_
#define __CSPEC_H_

#include <stdlib.h> /* malloc, calloc, realloc, free */
#include <signal.h> /* singal, kill */
#include <setjmp.h> /* jmp_buf, setjmp, longjmp */
#include <time.h>   /* time, localtime */
#include <stdio.h>  /* FILE, fopen, fprintf, printf, snprintf */

/* Find C version for declaring cross version implementations */
#if defined(__STDC__)
    #if defined(__STDC_VERSION__)
        /* For GNU compilers */
        #if(__STDC_VERSION__ >= 201112L)
            #ifndef _C11
                #define _C11
            #endif
        #elif(__STDC_VERSION__ >= 199901L)
            #ifndef _C99
                #define _C99
            #endif
        #else
            #ifndef _C90
                #define _C90
            #endif
        #endif
    #elif defined(_MSC_VER)
        /* Assume we are running MSVC, that defaults to c90 */
        #ifndef _C90
            #define _C90
        #endif
    #else
        #ifndef _C89
            #define _C89
        #endif
    #endif
#endif

/* Includes for the timer function */
#if defined(_WIN32)
    #include <Windows.h>
#elif defined(__unix__)
    #if defined(__clang__)
        #define CLOCK_REALTIME 0
        #define CLOCKID CLOCK_REALTIME
    #endif

    #include <time.h>

    #define HAVE_POSIX_TIMER
    #ifdef CLOCK_MONOTONIC
        #define CLOCKID CLOCK_MONOTONIC
    #endif

#elif defined(__MACH__) && defined(__APPLE__)
    #define HAVE_MACH_TIMER
    #include <mach/mach.h>
    #include <mach/mach_time.h>
#else
    #include <time.h>
#endif

/**
 * @func: cspec_get_timer
 * @desc: A cross platform timer function for profiling
 * @return The time in nanoseconds
 **/
static size_t cspec_get_timer(void) {
    static size_t is_init = 0;

    /* Cross platform definition */
    #if defined(_WIN32)
        static LARGE_INTEGER win_frequency;
        if(0 == is_init) {
            QueryPerformanceFrequency(&win_frequency);
            is_init = 1;
        }
        LARGE_INTEGER now;
        QueryPerformanceCounter(&now);
        return (1e9 * now.QuadPart) / win_frequency.QuadPart;
    #elif defined(__APPLE__)
        static mach_timebase_info_data_t info;
        if(0 == is_init) {
            mach_timebase_info(&info);
            is_init = 1;
        }
        size_t now;
        now = mach_absolute_time();
        now *= info.numer;
        now /= info.denom;
        return now;
    #elif defined(__linux)
        static struct timespec linux_rate;
        size_t now;
        struct timespec spec;
        if(0 == is_init) {
            clock_getres(CLOCKID, &linux_rate);
            is_init = 1;
        }
        clock_gettime(CLOCKID, &spec);
        now = spec.tv_sec * 1.0e9 + spec.tv_nsec;
        return now;
    #endif
}


/**
 * @param CSPEC_PASSING -> Set for passing tests
 * @param CSPEC_FAILING -> Set for failing tests
 * @param CSPEC_SKIPPED -> Set for skipeed tests
 **/
#define CSPEC_PASSING 1
#define CSPEC_FAILING 0
#define CSPEC_SKIPPED -1

/**
 * @macro: CSPEC_BLOCK
 * @desc: Expands to a do while loop that runs once
 *      Usefull for executing naked blocks
 * @param ... -> The block of code to run
 **/
#define CSPEC_BLOCK(...) do { \
    __VA_ARGS__ \
} while(0)

#define is ==
#define isnot !=
#define not !
#define and &&
#define or ||
#define equals ,
#define to
#define with
#define length ,

/**
 * @macro: cspec_compare_values
 * @desc: Runs the actual assertion between 2 values
 * @param actual -> The value passed by the user
 * @param expected -> The value `actual` is tested against
 * @param func -> The assert function according to data type
 **/
#define cspec_compare_values(actual, expected, func) CSPEC_BLOCK( \
    if(func(actual, expected)) { \
        cspec->status_of_test = CSPEC_FAILING; \
        cspec_write_position_in_file(); \
        cspec_write_assert_actual_expected(); \
    } \
)

/**
 * @macro: cspec_compare_array_values
 * @desc: Runs the actual assertion between 2 values
 * @param actual -> The value passed by the user
 * @param expected -> The value `actual` is tested against
 * @param func -> The assert function according to data type
 * @param len -> The length of the array to test
 **/
#define cspec_compare_array_values(actual, expected, func, len) CSPEC_BLOCK( \
    if(func(actual, expected, len)) { \
        cspec->status_of_test = CSPEC_FAILING; \
        cspec_write_position_in_file(); \
        cspec_write_assert_actual_expected(); \
    } \
)

/**
 * @macro: define_assert
 * @desc: Defines a compile time assertion for extended data types
 * @param name_of_assert -> The name of the new assertion
 * @param data_type_token -> The data type of the input variables
 * @param to_string_method -> Custom way to write data type as a string
 * @param comparison_method -> Custom way of comparing new data types for asserts
 **/
#define define_assert(name_of_assert, data_type_token, to_string_method, comparison_method) \
    static inline void name_of_assert(data_type_token actual, data_type_token expected) { \
        to_string_method(actual, expected); \
        cspec_compare_values(actual, expected, comparison_method); \
    }

#define define_assert_array(name_of_assert, data_type_token, to_string_method, comparison_method, len) \
    static inline void name_of_assert(data_type_token actual, data_type_token expected, size_t len) { \
        to_string_method(actual, expected, len); \
        cspec_compare_array_values(actual, expected, comparison_method, len); \
    }


/** @param bool -> A 'big' enough size to hold both 1 and 0 **/
typedef unsigned char bool;
#define true 1
#define false 0

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

#include <time.h>

/**
 * @func: cspec_error_assertion_not_supported
 * @desc: Throw an error message
 **/
static void cspec_error_assertion_not_supported(void) {
    printf("Error. Generic assertions not supported for this compiler.\n");
}

/**
 * @func: cspec_write_position_in_file
 * @desc: Setup file position and line number for the current assert
 **/
static void cspec_write_position_in_file(void) {
    cspec->position_in_file = cspec_string_new("");
    cspec_string_add_str(cspec->position_in_file, cspec->current_file);
    cspec_string_add_str(cspec->position_in_file, ":");
    cspec_string_add_int(cspec->position_in_file, cspec->current_line);
    cspec_string_add_str(cspec->position_in_file, ":");
}

/**
 * @func: cspec_write_assert_actual_expected
 * @desc: Setup the assert description for printing and exporting
 **/
static void cspec_write_assert_actual_expected(void) {
    cspec_vector *list_of_strings;
    cspec->assert_result = cspec_string_new("");
    cspec_string_add_str(cspec->test_result_message, cspec_string_get(cspec->display_tab));
    cspec_string_add_str(cspec->test_result_message, cspec_string_get(cspec->WHITE));
    cspec_string_add_str(cspec->test_result_message, "    ");
    cspec_string_add_str(cspec->test_result_message, cspec_string_get(cspec->position_in_file));
    cspec_string_add_str(cspec->test_result_message, "\n");
    cspec_string_add_str(cspec->test_result_message, cspec_string_get(cspec->display_tab));
    cspec_string_add_str(cspec->test_result_message, "        |> ");
    cspec_string_add_str(cspec->test_result_message, "`");
    cspec_string_add_str(cspec->test_result_message, cspec_string_get(cspec->current_expected));
    cspec_string_add_str(cspec->test_result_message, "` expected but got ");
    cspec_string_add_str(cspec->test_result_message, cspec_string_get(cspec->RED));
    cspec_string_add_str(cspec->test_result_message, "`");
    cspec_string_add_str(cspec->test_result_message, cspec_string_get(cspec->current_actual));
    cspec_string_add_str(cspec->test_result_message, "`\n");
    cspec_string_add_str(cspec->test_result_message, cspec_string_get(cspec->RESET));
/****************************************************************************/
    cspec_string_add_str(cspec->assert_result, "|> `");
    cspec_string_add_str(cspec->assert_result, cspec_string_get(cspec->current_expected));
    cspec_string_add_str(cspec->assert_result, "` expected but got ");
    cspec_string_add_str(cspec->assert_result, "`");
    cspec_string_add_str(cspec->assert_result, cspec_string_get(cspec->current_actual));
/****************************************************************************/
    list_of_strings = cspec_vector_new();
    cspec_vector_add(list_of_strings, cspec_string_get(cspec->position_in_file));
    cspec_vector_add(list_of_strings, cspec_string_get(cspec->assert_result));
/****************************************************************************/
    cspec_vector_add(cspec->list_of_asserts, list_of_strings);
}

/**
 * @func: cspec_write_nassert_actual_expected
 * @desc: Setup the `not` assert description for printing and exporting
 **/
static void cspec_write_nassert_actual_expected(void) {
    cspec_vector *list_of_strings;
    cspec->assert_result = cspec_string_new("");
    cspec_string_add_str(cspec->test_result_message, cspec_string_get(cspec->display_tab));
    cspec_string_add_str(cspec->test_result_message, cspec_string_get(cspec->WHITE));
    cspec_string_add_str(cspec->test_result_message, "    ");
    cspec_string_add_str(cspec->test_result_message, cspec_string_get(cspec->position_in_file));
    cspec_string_add_str(cspec->test_result_message, "\n");
    cspec_string_add_str(cspec->test_result_message, cspec_string_get(cspec->display_tab));
    cspec_string_add_str(cspec->test_result_message, "        |> ");
    cspec_string_add_str(cspec->test_result_message, "expected that `");
    cspec_string_add_str(cspec->test_result_message, cspec_string_get(cspec->current_expected));
    cspec_string_add_str(cspec->test_result_message, "` would differ from ");
    cspec_string_add_str(cspec->test_result_message, cspec_string_get(cspec->RED));
    cspec_string_add_str(cspec->test_result_message, "`");
    cspec_string_add_str(cspec->test_result_message, cspec_string_get(cspec->current_actual));
    cspec_string_add_str(cspec->test_result_message, "`");
    cspec_string_add_str(cspec->test_result_message, cspec_string_get(cspec->WHITE));
    cspec_string_add_str(cspec->test_result_message, " but they are the same\n");
    cspec_string_add_str(cspec->test_result_message, cspec_string_get(cspec->RESET));
/****************************************************************************/
    cspec_string_add_str(cspec->assert_result, "|> expected that `");
    cspec_string_add_str(cspec->assert_result, cspec_string_get(cspec->current_expected));
    cspec_string_add_str(cspec->assert_result, "` would differ from ");
    cspec_string_add_str(cspec->assert_result, "`");
    cspec_string_add_str(cspec->assert_result, cspec_string_get(cspec->current_actual));
    cspec_string_add_str(cspec->assert_result, "`");
    cspec_string_add_str(cspec->assert_result, " but they are the same");
/****************************************************************************/
    list_of_strings = cspec_vector_new();
    cspec_vector_add(list_of_strings, cspec_string_get(cspec->position_in_file));
    cspec_vector_add(list_of_strings, cspec_string_get(cspec->assert_result));
/****************************************************************************/
    cspec_vector_add(cspec->list_of_asserts, list_of_strings);
}

/**
 * @func: cspec_insert_it_block_in_list_of_its
 * @desc: Inserts the it block in its designated vector index
 **/
static void cspec_insert_it_block_in_list_of_its(void) {
    /* Setup the it block vector */
    cspec_vector *it_block = cspec_vector_new();
    cspec_vector_add(it_block, cspec->list_of_asserts);
    cspec_vector_add(it_block, cspec->name_of_tested_proc);
    cspec_vector_add(it_block, cspec->status_of_test);

    if(cspec->in_context_block) {
        /* Insert in a context block */
        if(cspec_vector_length(cspec->list_of_describes) > 1) {
            /* Stupid indexing for finding the correct nested blocks */
            cspec_vector *desc_block = (cspec_vector*)cspec_vector_get(cspec->list_of_describes,
                -(cspec_vector_length(cspec->list_of_describes) - cspec->inner_nest - 1));
            cspec_vector *list_of_cons = (cspec_vector*)cspec_vector_get(desc_block, 1);
            cspec_vector *con_block = (cspec_vector*)cspec_vector_get(list_of_cons,
                cspec_vector_length(list_of_cons) - 1);
            cspec_vector *list_of_con_its = (cspec_vector*)cspec_vector_get(con_block, 0);
            cspec_vector_add(list_of_con_its, it_block);
        }
        else {
            cspec_vector *desc_block = (cspec_vector*)cspec_vector_get(cspec->list_of_describes,
                cspec_vector_length(cspec->list_of_describes) - 1);
            cspec_vector *list_of_cons = (cspec_vector*)cspec_vector_get(desc_block, 1);
            cspec_vector *con_block = (cspec_vector*)cspec_vector_get(list_of_cons,
                cspec_vector_length(list_of_cons) - 1);
            cspec_vector *list_of_con_its = (cspec_vector*)cspec_vector_get(con_block, 0);
            cspec_vector_add(list_of_con_its, it_block);
        }
    }
    else {
        /* Insert in a describe block */
        cspec_vector *desc_block = (cspec_vector*)cspec_vector_get(cspec->list_of_describes,
            cspec_vector_length(cspec->list_of_describes) - 1 - cspec->outer_nest);
        cspec_vector *list_of_desc_its = (cspec_vector*)cspec_vector_get(desc_block, 0);
        cspec_vector_add(list_of_desc_its, it_block);
    }
}

/**
 * @func: cspec_report_time_taken_for_tests
 * @desc: Report the number of tests and time taken while testing
 **/
static void cspec_report_time_taken_for_tests(void) {
    printf("\n%s● %d tests\n%s✓ %d passing\n%s✗ %d failing\n%s- %d skipped%s\n",
    cspec_string_get(cspec->YELLOW),
    cspec->number_of_tests,
    cspec_string_get(cspec->GREEN),
    cspec->number_of_passing_tests,
    cspec_string_get(cspec->RED),
    cspec->number_of_failing_tests,
    cspec_string_get(cspec->GRAY),
    cspec->number_of_skipped_tests,
    cspec_string_get(cspec->RESET));

    /* Print in seconds if the time is more than 100ms */
    if(cspec->total_time_taken_for_tests > 100000000) {
        printf("%s★ Finished in %.5f seconds%s\n",
        cspec_string_get(cspec->CYAN),
        cspec->total_time_taken_for_tests / 1000000000.0,
        cspec_string_get(cspec->RESET));
    }
    /* Else print in miliseconds */
    else {
        printf("%s★ Finished in %.5f ms%s\n",
        cspec_string_get(cspec->CYAN),
        cspec->total_time_taken_for_tests / 1000000.0,
        cspec_string_get(cspec->RESET));
    }
}

/**
 * @func: cspec_fprintf_asserts
 * @desc: Writes asserts to an output file
 * @param assert -> The assert string to export
 **/
static void cspec_fprintf_asserts(cspec_vector *assert) {
    fprintf(cspec->fd, "%s            %s\n                %s\n\n",
    cspec_string_get(cspec->display_tab),
    (char*)cspec_vector_get(assert, 0),
    (char*)cspec_vector_get(assert, 1));
}

/**
 * @func: cspec_fprintf_its
 * @desc: Writes it blocks to an output file
 * @param it_block -> The it block to iterate against
 **/
static void cspec_fprintf_its(cspec_vector *it_block) {
    /* Check for type of test */
    if(((long)cspec_vector_get(it_block, 2) == CSPEC_FAILING)) {
        if(cspec_streql(cspec_string_get(cspec->type_of_export_tests), "failing")
        || cspec_streql(cspec_string_get(cspec->type_of_export_tests), "all")) {
            /* Write failing tests */
            cspec_vector *asserts;
            fprintf(cspec->fd, "%s        ✗ it %s\n",
            cspec_string_get(cspec->display_tab),
            cspec_string_get((cspec_string*)cspec_vector_get(it_block, 1)));
            asserts = (cspec_vector*)cspec_vector_get(it_block, 0);

            /* Call to print the asserts iteratevely for the current it block */
            cspec_vector_map(asserts, (cspec_lambda)cspec_fprintf_asserts);
        }
    }
    else if(((long)cspec_vector_get(it_block, 2) == CSPEC_SKIPPED)) {
        if(cspec_streql(cspec_string_get(cspec->type_of_export_tests), "skipped")
        || cspec_streql(cspec_string_get(cspec->type_of_export_tests), "all")) {
            /* Write skipped tests */
            fprintf(cspec->fd, "%s        - it %s\n",
            cspec_string_get(cspec->display_tab),
            cspec_string_get((cspec_string*)cspec_vector_get(it_block, 1)));
        }
    }
    else if(((long)cspec_vector_get(it_block, 2) == CSPEC_PASSING)) {
        /* Write passing tests */
        if(cspec_streql(cspec_string_get(cspec->type_of_export_tests), "passing")
        || cspec_streql(cspec_string_get(cspec->type_of_export_tests), "all")) {
            fprintf(cspec->fd, "%s        ✓ it %s\n",
            cspec_string_get(cspec->display_tab),
            cspec_string_get((cspec_string*)cspec_vector_get(it_block, 1)));
        }
    }
}

/**
 * @func: cspec_fprintf_cons
 * @desc: Writes context blocks to an output file
 * @param con -> The context block to iterate against
 **/
static void cspec_fprintf_cons(cspec_vector *con) {
    cspec_vector *its;

    cspec_string_add_str(cspec->display_tab, "    ");

    fprintf(cspec->fd, "        `%s`\n",
    cspec_string_get((cspec_string*)cspec_vector_get(con, 1)));
    its = (cspec_vector*)cspec_vector_get(con, 0);

    /* Iterate over the it blocks under the context block */
    cspec_vector_map(its, (cspec_lambda)cspec_fprintf_its);
    
    cspec_string_skip(cspec->display_tab, 4);
}

/**
 * @func: cspec_fprintf_describes
 * @desc: Writes describe blocks to an output file
 * @param desc -> The describe block to iterate against
 **/
static void cspec_fprintf_describes(cspec_vector *desc) {
    cspec_vector *its;

    fprintf(cspec->fd, "    `%s`\n",
    cspec_string_get((cspec_string*)cspec_vector_get(desc, 2)));

    /* Iterate over the it blocks under the describe block */
    its = (cspec_vector*)cspec_vector_get(desc, 0);
    cspec_vector_map(its, (cspec_lambda)cspec_fprintf_its);

    /* Iterate over the context blocks under the describe block */
    cspec_vector *cons = (cspec_vector*)cspec_vector_get(desc, 1);
    cspec_vector_map(cons, (cspec_lambda)cspec_fprintf_cons);
}

/**
 * @func: cspec_fprintf_modules
 * @desc: Writes module blocks to an output file
 * @param mod -> The module block to iterate against
 **/
static void cspec_fprintf_modules(cspec_vector *mod) {
    cspec_vector *descs;
    
    cspec_string_delete(cspec->display_tab);
    fprintf(cspec->fd, "\nModule `%s`\n",
    cspec_string_get((cspec_string*)cspec_vector_get(mod, 1)));
    descs = (cspec_vector*)cspec_vector_get(mod, 0);

    /* Iterate over the describe blocks under the module block */
    cspec_vector_map(descs, (cspec_lambda)cspec_fprintf_describes);
}

/**
 * @func: cspec_export_to_txt
 * @desc: Export test results into a txt file
 **/
static void cspec_export_to_txt(void) {
    cspec_vector_map(cspec->list_of_modules, (cspec_lambda)cspec_fprintf_modules);
    fclose(cspec->fd);
}

/**
 * @func: cspec_xml_write_asserts
 * @desc: Writes asserts to an xml file
 * @param assert -> The assert block containing strings to print
 **/
static void cspec_xml_write_asserts(cspec_vector *assert) {
    fprintf(cspec->fd, "%s                <failure>\n",
    cspec_string_get(cspec->display_tab));
    
    fprintf(cspec->fd, "%s                    <position>%s</position>\n",
    cspec_string_get(cspec->display_tab),
    (char*)cspec_vector_get(assert, 0));

    fprintf(cspec->fd, "%s                    <error>%s</error>\n",
    cspec_string_get(cspec->display_tab),
    (char*)cspec_vector_get(assert, 1));

    fprintf(cspec->fd, "%s                </failure>\n",
    cspec_string_get(cspec->display_tab));
}

/**
 * @func: cspec_xml_write_its
 * @desc: Writes it blocks to an xml file
 * @param it_block -> The it block to iterate against
 **/
static void cspec_xml_write_its(cspec_vector *it_block) {
    /* Check for type of test */
    if(((long)cspec_vector_get(it_block, 2) == CSPEC_FAILING)) {
        if(cspec_streql(cspec_string_get(cspec->type_of_export_tests), "failing")
        || cspec_streql(cspec_string_get(cspec->type_of_export_tests), "all")) {
            cspec_vector *asserts;
            /* Write failing tests */
            fprintf(cspec->fd, "            %s<it>\n                %s<name>%s</name>\n                %s<status>failing</status>\n",
            cspec_string_get(cspec->display_tab),
            cspec_string_get(cspec->display_tab),
            cspec_string_get((cspec_string*)cspec_vector_get(it_block, 1)),
            cspec_string_get(cspec->display_tab));
            asserts = (cspec_vector*)cspec_vector_get(it_block, 0);

            /* Call to print the asserts iteratevely for the current it block */
            cspec_vector_map(asserts, (cspec_lambda)cspec_xml_write_asserts);
            fprintf(cspec->fd, "            %s</it>\n",
            cspec_string_get(cspec->display_tab));
        }
    }
    else if(((long)cspec_vector_get(it_block, 2) == CSPEC_SKIPPED)) {
        if(cspec_streql(cspec_string_get(cspec->type_of_export_tests), "skipped")
        || cspec_streql(cspec_string_get(cspec->type_of_export_tests), "all")) {
            /* Write skipped tests */
            fprintf(cspec->fd, "            %s<xit>\n                %s<name>%s</name>\n                %s<status>skipped</status>\n",
            cspec_string_get(cspec->display_tab),
            cspec_string_get(cspec->display_tab),
            cspec_string_get((cspec_string*)cspec_vector_get(it_block, 1)),
            cspec_string_get(cspec->display_tab));
            fprintf(cspec->fd, "            %s</xit>\n",
            cspec_string_get(cspec->display_tab));
        }
    }
    else if(((long)cspec_vector_get(it_block, 2) == CSPEC_PASSING)) {
        /* Write passing tests */
        if(cspec_streql(cspec_string_get(cspec->type_of_export_tests), "passing")
        || cspec_streql(cspec_string_get(cspec->type_of_export_tests), "all")) {
            fprintf(cspec->fd, "            %s<it>\n                %s<name>%s</name>\n                %s<status>passing</status>\n",
            cspec_string_get(cspec->display_tab),
            cspec_string_get(cspec->display_tab),
            cspec_string_get((cspec_string*)cspec_vector_get(it_block, 1)),
            cspec_string_get(cspec->display_tab));
            fprintf(cspec->fd, "            %s</it>\n",
            cspec_string_get(cspec->display_tab));
        }
    }
}

/**
 * @func: cspec_xml_write_cons
 * @desc: Writes context blocks to an xml file
 * @param con -> The context block to iterate against
 **/
static void cspec_xml_write_cons(cspec_vector *con) {
    cspec_vector *its;
    cspec_string_add_str(cspec->display_tab, "    ");
    fprintf(cspec->fd, "            <context>\n                <name>%s</name>\n",
    cspec_string_get((cspec_string*)cspec_vector_get(con, 1)));
    its = (cspec_vector*)cspec_vector_get(con, 0);

    /* Iterate over the it blocks under the context block */
    cspec_vector_map(its, (cspec_lambda)cspec_xml_write_its);
    fprintf(cspec->fd, "            </context>\n");
    cspec_string_skip(cspec->display_tab, 4);
}

/**
 * @func: cspec_xml_write_describes
 * @desc: Writes describe blocks to an xml file
 * @param desc -> The current describe block to iterate against
 **/
static void cspec_xml_write_describes(cspec_vector *desc) {
    cspec_vector *its;
    cspec_vector *cons;

    fprintf(cspec->fd, "        <describe>\n            <name>%s</name>\n",
    cspec_string_get((cspec_string*)cspec_vector_get(desc, 2)));

    /* Iterate over the it blocks under the describe block */
    its = (cspec_vector*)cspec_vector_get(desc, 0);
    cspec_vector_map(its, (cspec_lambda)cspec_xml_write_its);

    /* Iterate over the context blocks under the describe block */
    cons = (cspec_vector*)cspec_vector_get(desc, 1);
    cspec_vector_map(cons, (cspec_lambda)cspec_xml_write_cons);
    fprintf(cspec->fd, "        </describe>\n");
}

/**
 * @func: cspec_xml_write_modules
 * @desc: Writes module blocks to an xml file
 * @param mod -> The module block to iterate against
 **/
static void cspec_xml_write_modules(cspec_vector *mod) {
    cspec_vector *descs;

    time_t t = time(NULL);
    struct tm calc_time;
    localtime_r(&calc_time, &t);

    cspec_string_delete(cspec->display_tab);
    fprintf(cspec->fd, "    <module>\n        <name>%s</name>\n        <failures>%d</failures>\n        <skipped>%d</skipped>\n        <tests>%d</tests>\n        <timestamp>%d-%02d-%02d %02d:%02d:%02d</timestamp>\n",
    cspec_string_get((cspec_string*)cspec_vector_get(mod, 1)),
    cspec->number_of_failing_tests,
    cspec->number_of_skipped_tests,
    cspec->number_of_tests,
    calc_time.tm_year + 1900, calc_time.tm_mon + 1, calc_time.tm_mday, calc_time.tm_hour, calc_time.tm_min, calc_time.tm_sec);
    descs = (cspec_vector*)cspec_vector_get(mod, 0);

    /* Iterate over the describe blocks under the module block */
    cspec_vector_map(descs, (cspec_lambda)cspec_xml_write_describes);
    fprintf(cspec->fd, "    </module>\n");
}

/**
 * @func: cspec_export_to_xml
 * @desc: Export test results into an xml file
 **/
static void cspec_export_to_xml(void) {
    fprintf(cspec->fd, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");
    
    if(cspec->total_time_taken_for_tests > 100000000) {
        fprintf(cspec->fd, "<modules>\n    <duration>%.5f seconds</duration>\n",
        cspec->total_time_taken_for_tests / 1000000000.0);
    }
    else {
        fprintf(cspec->fd, "<modules>\n    <duration>%.5f ms</duration>\n",
        cspec->total_time_taken_for_tests / 1000000.0);
    }
    
    cspec_vector_map(cspec->list_of_modules, (cspec_lambda)cspec_xml_write_modules);
    fprintf(cspec->fd, "</modules>\n");
    fclose(cspec->fd);
}

/**
 * @func: cspec_export_to_md
 * @desc: Export test results into a markdown file
 **/
static void cspec_export_to_md(void) {
    /* TODO COMPLETE EXPORTING TO MD */
}

/**
 * @func: cspec_export_to_html
 * @desc: Export test results into an html file
 **/
static void cspec_export_to_html(void) {
    /* TODO COMPLETE EXPORTING TO HTML */
}

/**
 * @func: cspec_signal_handler
 * @desc: Handles errors where undefined behaviour
 *         calls for segfauls or other singals
 * @param signal_id -> The number assign to the signal
 **/
static void cspec_signal_handler(const int signal_id) {
    cspec_string *id = cspec_string_new("");
    cspec_string_add_int(id, signal_id);
    char *sig_description;

    /* Find the correct signal description */
    switch(signal_id) {
        /* SIGINT */ case 2: sig_description = (char*)cspec_vector_get(cspec->signals_vector, 0); break;
        /* SIGILL */ case 4: sig_description = (char*)cspec_vector_get(cspec->signals_vector, 1); break;
        /* SIGABRT */ case 6: sig_description = (char*)cspec_vector_get(cspec->signals_vector, 2); break;
        /* SIGFPE */ case 8: sig_description = (char*)cspec_vector_get(cspec->signals_vector, 3); break;
        /* SIGSEGV */ case 11: sig_description = (char*)cspec_vector_get(cspec->signals_vector, 4); break;
        /* SIGTERM */ case 15: sig_description = (char*)cspec_vector_get(cspec->signals_vector, 5); break;
    }

    /* Craft a response for signal errors */
    cspec_string *sig = cspec_string_new("");
    cspec_string_add_str(sig, cspec_string_get(cspec->display_tab));
    cspec_string_add_str(sig, cspec_string_get(cspec->RED));
    cspec_string_add_str(sig, "✗");
    cspec_string_add_str(sig, cspec_string_get(cspec->WHITE));
    cspec_string_add_str(sig, " it ");
    cspec_string_add_str(sig, cspec_string_get(cspec->name_of_tested_proc));
    cspec_string_add_str(sig, ":\n");
    cspec_string_add_str(sig, cspec_string_get(cspec->display_tab));
    cspec_string_add_str(sig, "    ");
    cspec_string_add_str(sig, cspec->current_file);
    cspec_string_add_str(sig, ":");
    cspec_string_add_int(sig, cspec->current_line);
    cspec_string_add_str(sig, "\n");
    cspec_string_add_str(sig, cspec_string_get(cspec->display_tab));
    cspec_string_add_str(sig, "        |> ");
    cspec_string_add_str(sig, "signal: ");
    cspec_string_add_str(sig, cspec_string_get(cspec->RED));
    cspec_string_add_str(sig, sig_description);
    cspec_string_add_str(sig, cspec_string_get(cspec->WHITE));
    cspec_string_add_str(sig, " got caught on \n");
    cspec_string_add_str(sig, cspec_string_get(cspec->display_tab));
    cspec_string_add_str(sig, "          -> ");
    cspec_string_add_str(sig, cspec_string_get(cspec->RED));
    cspec_string_add_str(sig, "`");

    /* This only works for `assert_that` blocks */
    cspec_string_add_str(sig, cspec_string_get(cspec->current_assert));
    /*------------------------------------------*/

    cspec_string_add_str(sig, "`");
    cspec_string_add_str(sig, cspec_string_get(cspec->RESET));
    printf("%s\n\n", cspec_string_get(sig));
    /* signal(signal_id, cspec_signal_handler); */
    signal(signal_id, SIG_DFL);
    /* longjmp(&cspec->escape, 1); */
}

/**
 * @func: cspec_setup_test_data
 * @desc: Allocates memory for vectors to save test results in
 **/
static void cspec_setup_test_data(void) {
    /* Printf a neat intro */
    printf("\033[38;5;95m/######## ########/\n");
    printf("\033[38;5;95m/##### \033[38;5;89mc\033[38;5;90mS\033[38;5;91mp\033[38;5;92me\033[38;5;93mc\033[0m \033[38;5;95m#####/\n");
    printf("/######## ########/\033[0m\n");
    
    /* Craft the global variable struct */
    cspec = (cspec_data_struct*)malloc(sizeof(cspec_data_struct) + 1);

    cspec->number_of_tests = 0;
    cspec->number_of_passing_tests = 0;
    cspec->number_of_failing_tests = 0;
    cspec->number_of_skipped_tests = 0;
    cspec->total_time_taken_for_tests = 0;
    cspec->status_of_test = CSPEC_PASSING;
    /* jmp_buf cspec->escape; */

    cspec->test_result_message = NULL;
    cspec->name_of_tested_proc = NULL;
    cspec->name_of_describe = NULL;
    cspec->name_of_context = NULL;

    cspec->current_assert = NULL;
    cspec->current_file = NULL;
    cspec->current_line = 0;
    cspec->current_actual = NULL;
    cspec->current_expected = NULL;
    cspec->position_in_file = NULL;
    cspec->assert_result = NULL;
    cspec->type_of_tests = NULL;
    cspec->type_of_export_tests = NULL;

    /** Before and after procs **/
    cspec->before_func = NULL;
    cspec->after_func = NULL;

    cspec->display_tab = NULL;
    cspec->signals_vector = cspec_vector_new();
    cspec->inner_nest = -1;
    cspec->outer_nest = 0;
    cspec->in_context_block = false;

    /** Colors **/
    cspec->GREEN = cspec_string_new("\033[38;5;78m");
    cspec->RED = cspec_string_new("\033[1;31m");
    cspec->YELLOW = cspec_string_new("\033[38;5;11m");
    cspec->PURPLE = cspec_string_new("\033[38;5;207m");
    cspec->CYAN = cspec_string_new("\033[1;36m");
    cspec->GRAY = cspec_string_new("\033[38;5;244m");
    cspec->WHITE = cspec_string_new("\033[38;5;254m");
    cspec->RESET = cspec_string_new("\033[0m");
    cspec->BACK_PURPLE = cspec_string_new("\033[48;5;89m");

    cspec->list_of_modules = cspec_vector_new();
    cspec->list_of_describes = cspec_vector_new();
    cspec->list_of_contexts = cspec_vector_new();
    cspec->list_of_its = cspec_vector_new();
    cspec->list_of_asserts = cspec_vector_new();

    /* Signals that can be processed */
    cspec_vector_add(cspec->signals_vector, (void*)"`SIGINT | Interrupt`");
    cspec_vector_add(cspec->signals_vector, (void*)"`SIGILL | Illegal instruction`");
    cspec_vector_add(cspec->signals_vector, (void*)"`SIGABRT | Abort`");
    cspec_vector_add(cspec->signals_vector, (void*)"`SIGFPE | Floating - Point arithmetic exception`");
    cspec_vector_add(cspec->signals_vector, (void*)"`SIGSEGV | Segmentation Violation`");
    cspec_vector_add(cspec->signals_vector, (void*)"`SIGTERM | Termination`");

    /* Setup the signals */
    signal(SIGINT, cspec_signal_handler);
    signal(SIGILL, cspec_signal_handler);
    signal(SIGABRT, cspec_signal_handler);
    signal(SIGFPE, cspec_signal_handler);
    signal(SIGSEGV, cspec_signal_handler);
    signal(SIGTERM, cspec_signal_handler);
}


/**
 * @macro: spec
 * @desc: Call the main automation function
 **/
#define spec(...) \
    int main(int argc, char **argv) { \
        CSPEC_BLOCK(__VA_ARGS__); \
    }

/**
 * @macro: spec_suite
 * @desc: A simple function definition for running test suites
 * @param ... -> The block of modules to run
 **/
#define spec_suite(...) \
    /* Define a function to run tests modules at */ \
    static void run_spec_suite(char *type_of_tests) { \
        cspec_setup_test_data(); \
        cspec->type_of_tests = cspec_string_new(type_of_tests); \
        \
        \
        /* Check for valid test type */ \
        if(!cspec_streql(type_of_tests, "passing") \
        && !cspec_streql(type_of_tests, "failing") \
        && !cspec_streql(type_of_tests, "skipped") \
        && !cspec_streql(type_of_tests, "all")) { \
            printf("\n\033[1;31mInput a type of test to log passing|failing|skipped|all\033[0m\n\n"); \
            return; /* Exit the function */ \
        } \
        \
        \
        CSPEC_BLOCK(__VA_ARGS__); \
        cspec_report_time_taken_for_tests(); \
        \
        \
    }

/**
 * @macro: module
 * @desc: Expands to a function definition of the test suite
 * @param suite_name -> The name for the new module of tests
 * @param ... -> The block to define
 **/
#define module(suite_name, ...) \
    /* Define a static method for a module */ \
    static void suite_name(void) { \
        cspec_vector *mod; \
        \
        cspec->name_of_module = cspec_string_new(#suite_name); \
        cspec->list_of_describes = cspec_vector_new(); \
        \
        \
        /* Print module information */ \
        printf("\n%s%sModule `%s`%s\n", \
        cspec_string_get(cspec->BACK_PURPLE), \
        cspec_string_get(cspec->YELLOW), \
        cspec_string_get(cspec->name_of_module), \
        cspec_string_get(cspec->RESET)); \
        cspec->display_tab = cspec_string_new(""); \
        \
        \
        CSPEC_BLOCK(__VA_ARGS__); /* Run describes */ \
        \
        \
        /* Construct a module and save for exporting */ \
        mod = cspec_vector_new(); \
        cspec_vector_add(mod, cspec->list_of_describes); \
        cspec_vector_add(mod, cspec->name_of_module); \
        cspec_vector_add(cspec->list_of_modules, mod); \
    }

/**
 * @macro: before
 * @desc: Expands to a setup proc that gets executed before the tests
 * @param ... -> The proc to run
 **/
#define before(...) CSPEC_BLOCK( \
    __VA_ARGS__ \
)

/**
 * @marco: after
 * @desc: Expands to a teardown proc that gets executed after the tests
 * @param ... -> The proc to run
 **/
#define after(...) CSPEC_BLOCK( \
    __VA_ARGS__ \
)

/**
 * @macro: before_each
 * @desc: Sets the argument to a function to run before each it block
 * @param func -> The func to set
 **/
#define before_each(func) CSPEC_BLOCK( \
    cspec->before_func = func; \
)

/**
 * @marco: after_each
 * @desc: Sets the argument to a function to run after each it block
 * @param func -> The func to set
 **/
#define after_each(func) CSPEC_BLOCK( \
    cspec->after_func = func; \
)

/**
 * @macro: cspec_describe
 * @desc: Defines a `describe` level block
 * @param __VA_ARGS__
 **/
#define cspec_describe(...) CSPEC_BLOCK( \
    if(cspec->inner_nest == -1) cspec->outer_nest = 0; \
    \
    /* On every describe block we nest inwards */ \
    cspec->inner_nest++; \
    \
    __VA_ARGS__ \
    \
    /* Reset nests */ \
    cspec->outer_nest++; \
    cspec->inner_nest--; \
)

/**
 * @marco: describe
 * @desc: Expands to a block execution where tests are encompassed
 * @param object_name -> The name of the unit to describe
 * @param ... -> The proc to extend to
 **/
#define describe(object_name, ...) CSPEC_BLOCK( \
    cspec_vector *describe_block; \
    \
    cspec_string_add_str(cspec->display_tab, "    "); \
    cspec->list_of_its = cspec_vector_new(); \
    cspec->list_of_contexts = cspec_vector_new(); \
    cspec->name_of_describe = cspec_string_new(object_name); \
    \
    \
    /* Display describe data */ \
    printf("%s%s`%s`%s\n", \
    cspec_string_get(cspec->display_tab), \
    cspec_string_get(cspec->PURPLE), \
    object_name, \
    cspec_string_get(cspec->RESET)); \
    \
    \
    /* Construct the describe block */ \
    describe_block = cspec_vector_new(); \
    cspec_vector_add(describe_block, cspec_vector_dup(cspec->list_of_its)); \
    cspec_vector_add(describe_block, cspec_vector_dup(cspec->list_of_contexts)); \
    cspec_vector_add(describe_block, cspec->name_of_describe); \
    cspec_vector_add(cspec->list_of_describes, describe_block); \
    \
    \
    cspec_describe(__VA_ARGS__); \
    \
    \
    cspec_string_skip(cspec->display_tab, 4); \
)

/**
 * @macro: context
 * @desc: Basically aliasing for descibe
 * @param object_name -> The name of the unit to describe
 * @param ... -> The proc to extend to
 **/
#define context(object_name, ...) CSPEC_BLOCK( \
    cspec_vector *context_block; \
    cspec_vector *desc_block; \
    cspec_vector *list_of_cons; \
    \
    cspec->in_context_block = true; \
    cspec_string_add_str(cspec->display_tab, "    "); \
    cspec->list_of_its = cspec_vector_new(); \
    cspec->name_of_context = cspec_string_new(object_name); \
    \
    \
    /* Display context data */ \
    printf("%s%s`%s`%s\n", \
    cspec_string_get(cspec->display_tab), \
    cspec_string_get(cspec->YELLOW), \
    object_name, \
    cspec_string_get(cspec->WHITE)); \
    \
    \
    /* Construct data for export */ \
    context_block = cspec_vector_new(); \
    cspec_vector_add(context_block, cspec_vector_dup(cspec->list_of_its)); \
    cspec_vector_add(context_block, cspec->name_of_context); \
    \
    /* Grab the specific describe block using `inner_nest` */ \
    desc_block = cspec_vector_get(cspec->list_of_describes, cspec->inner_nest); \
    list_of_cons = cspec_vector_get(desc_block, 1); \
    cspec_vector_add(list_of_cons, context_block); \
    \
    \
    cspec_describe(__VA_ARGS__); \
    \
    \
    cspec_string_skip(cspec->display_tab, 4); \
    cspec->in_context_block = false; \
)

/**
 * @macro: xit
 * @desc: Temporarily disables a test
 * @param proc_name -> The name of test to run
 * @param ... -> The actual test code
 **/
#define xit(proc_name, ...) CSPEC_BLOCK( \
    size_t start_test_timer; \
    size_t end_test_timer; \
    \
    /* Execute a `before` function */ \
    if(cspec->before_func) (*cspec->before_func)(); \
    cspec_string_add_str(cspec->display_tab, "    "); \
    \
    /* Setup for a skipped it block */ \
    cspec->number_of_tests++; \
    cspec->number_of_skipped_tests++; \
    cspec->test_result_message = cspec_string_new(""); \
    cspec->name_of_tested_proc = cspec_string_new(proc_name); \
    cspec->list_of_asserts = cspec_vector_new(); \
    cspec->status_of_test = CSPEC_SKIPPED; \
    \
    /* Dummy timers */ \
    start_test_timer = cspec_get_timer(); \
    end_test_timer = cspec_get_timer(); \
    \
    /* Check for valid test type */ \
    if((cspec_streql(cspec_string_get(cspec->type_of_tests), "all")) \
    || (cspec_streql(cspec_string_get(cspec->type_of_tests), "skipped"))) { \
        /* Display data about the skipped it */ \
        printf("%s%s- %s%s\n", \
        cspec_string_get(cspec->display_tab), \
        cspec_string_get(cspec->GRAY), \
        cspec_string_get(cspec->name_of_tested_proc), \
        cspec_string_get(cspec->RESET)); \
        \
        \
    } \
    cspec->total_time_taken_for_tests += end_test_timer - start_test_timer; \
    \
    \
    cspec_insert_it_block_in_list_of_its(); \
    \
    \
    /* Reset the display tab and execute the `after_func` */ \
    cspec_string_skip(cspec->display_tab, 4); \
    if(cspec->after_func) (*cspec->after_func)(); \
)

/**
 * @macro: it
 * @desc: Expands to a test run and saves all data gathered
 * @param proc_name -> The name of test to run
 * @param proc -> The actual test code
 **/
#define it(proc_name, ...) CSPEC_BLOCK( \
    /* Execute a `before` function */ \
    if(cspec->before_func) (*cspec->before_func)(); \
    \
    /* Setup for an it block */ \
    cspec_string_add_str(cspec->display_tab, "    "); \
	cspec->number_of_tests++; \
    cspec->test_result_message = cspec_string_new(""); \
    cspec->name_of_tested_proc = cspec_string_new(proc_name); \
    cspec->list_of_asserts = cspec_vector_new(); \
    \
    /* Assume its a passing test */ \
    cspec->status_of_test = CSPEC_PASSING; \
    cspec->current_line = __LINE__; \
    cspec->current_file = __FILE__; \
    \
    size_t start_test_timer = cspec_get_timer(); \
    \
    /* Execute asserts */ \
    __VA_ARGS__ \
    /* jmp_buf cspec->escape; */ \
    /* setjmp(&cspec->escape); */ \
    size_t end_test_timer = cspec_get_timer(); \
    \
    if(cspec->status_of_test) { \
        /* In case all asserts pass we print passing test information */ \
        cspec->number_of_passing_tests++; \
        if((cspec_streql(cspec_string_get(cspec->type_of_tests), "all")) \
        || (cspec_streql(cspec_string_get(cspec->type_of_tests), "passing"))) { \
            printf("%s%s✓%s it %s%s\n", \
            cspec_string_get(cspec->display_tab), \
            cspec_string_get(cspec->GREEN), \
            cspec_string_get(cspec->WHITE), \
            cspec_string_get(cspec->name_of_tested_proc), \
            cspec_string_get(cspec->RESET)); \
        } \
	} \
    else { \
        /* Even if 1 of the asserts in the current it block
            fails, assume we have a failing test */ \
		cspec->number_of_failing_tests++; \
        if((cspec_streql(cspec_string_get(cspec->type_of_tests), "all")) \
        || (cspec_streql(cspec_string_get(cspec->type_of_tests), "failing"))) { \
            printf("%s%s✗%s it %s:\n%s%s\n", \
            cspec_string_get(cspec->display_tab), \
            cspec_string_get(cspec->RED), \
            cspec_string_get(cspec->WHITE), \
            cspec_string_get(cspec->name_of_tested_proc), \
            cspec_string_get(cspec->test_result_message), \
            cspec_string_get(cspec->RESET)); \
        } \
    } \
    /* Calculate the total time */ \
    cspec->total_time_taken_for_tests += end_test_timer - start_test_timer; \
    \
    \
    cspec_insert_it_block_in_list_of_its(); \
    \
    \
    /* Reset the display tab and execute the `after_func` */ \
    cspec_string_skip(cspec->display_tab, 4); \
    if(cspec->after_func) (*cspec->after_func)(); \
)

/**
 * @macro: export_test_results
 * @desc: Exports the test results to some file type
 * @param vec -> Either passing|failing|skipped|all
 * @param type -> Export type either txt|xml|markdown
 **/
#define export_test_results(name, vec, type) CSPEC_BLOCK( \
    /* Check for valid type of test export */ \
    if(!cspec_streql(vec, "passing") \
    && !cspec_streql(vec, "failing") \
    && !cspec_streql(vec, "skipped") \
    && !cspec_streql(vec, "all")) { \
        printf("\n%sInput a type of test to export passing|failing|skipped|all%s\n\n", \
        cspec_string_get(cspec->RED), \
        cspec_string_get(cspec->RESET)); \
        return 0; \
    } \
    cspec->type_of_export_tests = cspec_string_new(vec); \
    \
    /* Reset the display tab */ \
    cspec_string_delete(cspec->display_tab); \
    cspec_string_add_str(cspec->display_tab, "    "); \
    if(type == "txt") { \
        cspec->fd = fopen(name, "w+"); \
        cspec_export_to_txt(); \
    } \
    else if(type == "xml") { \
        cspec->fd = fopen(name, "w+"); \
        cspec_export_to_xml(); \
    } \
    else if(type == "markdown") { \
        cspec->fd = fopen(name, "w+"); \
        cspec_export_to_md(); \
    } \
    else { \
        printf("\n%sSpecify the export type: `txt|xml|markdown`%s\n\n", \
        cspec_string_get(cspec->RED), \
        cspec_string_get(cspec->RESET)); \
        return 0; /* Exit the main function */ \
    } \
)


/**
 * @func: cspec_to_string_charptr_array_write
 * @desc: Writes actual and expected values
 * @param actual -> The value passed by the user
 * @param expected -> The value `actual` is tested against
 * @param len -> The length of the arrays
 **/
static void cspec_to_string_charptr_array_write(char **actual, char **expected, size_t len) {
    size_t i;
    cspec->current_actual = cspec_string_new("[");
    cspec->current_expected = cspec_string_new("[");

    for(i = 0; i < len - 1; i++) {
        cspec_string_add_str(cspec->current_actual, actual[i]);
        cspec_string_add_str(cspec->current_actual, ", ");

        cspec_string_add_str(cspec->current_expected, expected[i]);
        cspec_string_add_str(cspec->current_expected, ", ");
    }
    cspec_string_add_str(cspec->current_actual, actual[len - 1]);
    cspec_string_add_str(cspec->current_actual, "]");

    cspec_string_add_str(cspec->current_expected, expected[len - 1]);
    cspec_string_add_str(cspec->current_expected, "]");
}

/**
 * @func: cspec_charptr_array_comparison
 * @desc: Compares two string arrays for equality in elements
 * @param actual -> The value passed by the user
 * @param expected -> The value `actual` is tested against
 * @param len -> The length of the arrays
 * @return a boolean
 **/
static bool cspec_charptr_array_comparison(char **actual, char **expected, size_t len) {
    /* Check for members */
    size_t i;
    for(i = 0; i < len; i++)
        if(!cspec_streql(actual[i], expected[i])) return false;

    /* All elements are equal */
    return true;
}

/**
 * @func: cspec_call_assert_that_charptr_array
 * @desc: Assertion of two int arrays
 **/
define_assert_array(
    cspec_call_assert_that_charptr_array,
    double*,
    cspec_to_string_charptr_array_write,
    !cspec_charptr_array_comparison,
    len
)

/**
 * @func: cspec_call_nassert_that_charptr_array
 * @desc: Negative assertion of two double arrays
 **/
define_assert_array(
    cspec_call_nassert_that_charptr_array,
    double*,
    cspec_to_string_charptr_array_write,
    cspec_charptr_array_comparison,
    len
)

/**
 * @macro: assert_that_charptr_array
 * @desc: Assert that the expected charptr array equals to the result
 * @param actual -> The actual value
 * @param expected -> The expected value
 **/
#define assert_that_charptr_array(inner) CSPEC_BLOCK( \
    cspec->current_file = __FILE__; \
    cspec->current_line = __LINE__; \
    cspec_call_assert_that_charptr_array(inner); \
)

/**
 * @macro: nassert_that_charptr_array
 * @desc: Assert that the expected charptr array differs from the result
 * @param actual -> The actual value
 * @param expected -> The expected value
 **/
#define nassert_that_charptr_array(inner) CSPEC_BLOCK( \
    cspec->current_file = __FILE__; \
    cspec->current_line = __LINE__; \
    cspec_call_nassert_that_charptr_array(inner); \
)


/**
 * @macro: cspec_to_string_charptr_write
 * @desc: Writes actual and expected values
 * @param actual -> The value passed by the user
 * @param expected -> The value `actual` is tested against
 **/
/* TODO -> CONVERT FROM A BLOCK TO A FUNCTION */
#define cspec_to_string_charptr_write(actual, expected) CSPEC_BLOCK( \
    cspec->current_actual = cspec_string_new(actual); \
    cspec->current_expected = cspec_string_new(expected); \
)

/**
 * @func: cspec_charptr_comparison
 * @desc: A function that compares char pointers for assertions
 * @param actual -> The value passed by the user
 * @param expected -> The value `actual` is tested against
 * @return a boolean
 **/
static bool cspec_charptr_comparison(char *actual, char *expected) {
    return cspec_streql(expected, actual);
}

/**
 * @func: cspec_call_assert_that_charptr
 * @desc: Assert that the expected string is equal to the result
 * @param actual -> The value passed by the user
 * @param expected -> The value `actual` is tested against
 **/
define_assert(
    cspec_call_assert_that_charptr,
    char*,
    cspec_to_string_charptr_write,
    !cspec_charptr_comparison
)

/**
 * @func: cspec_call_nassert_that_charptr
 * @desc: Assert that the expected string is different than the result
 * @param actual -> The value passed by the user
 * @param expected -> The value `actual` is tested against
 **/
define_assert(
    cspec_call_nassert_that_charptr,
    char*,
    cspec_to_string_charptr_write,
    cspec_charptr_comparison
)

/**
 * @macro: assert_that_charptr
 * @desc: Assert that the expected string is equal to the result
 * @param actual -> The actual value
 * @param expected -> The expected string
 **/
#define assert_that_charptr(inner) CSPEC_BLOCK( \
    cspec->current_file = __FILE__; \
    cspec->current_line = __LINE__; \
    cspec_call_assert_that_charptr(inner); \
)

/**
 * @macro: nassert_that_charptr
 * @desc: Assert that the expected string is different than the result
 * @param actual -> The actual value
 * @param expected -> The expected string
 **/
#define nassert_that_charptr(inner) CSPEC_BLOCK( \
    cspec->current_file = __FILE__; \
    cspec->current_line = __LINE__; \
    cspec_call_nassert_that_charptr(inner); \
)


/**
 * @func: cspec_to_string_double_array_write
 * @desc: Writes the actual and expected values
 * @param actual -> The value passed by the user
 * @param expected -> The value `actual` is tested against
 * @param len -> The length of the arrays
 **/
static void cspec_to_string_double_array_write(double *actual, double *expected, size_t len) {
    size_t i;

    cspec->current_actual = cspec_string_new("[");
    cspec->current_expected = cspec_string_new("[");

    for(i = 0; i < len - 1; i++) {
        cspec_string_add_double_precision(cspec->current_actual, actual[i]);
        cspec_string_add_str(cspec->current_actual, ", ");

        cspec_string_add_double_precision(cspec->current_expected, expected[i]);
        cspec_string_add_str(cspec->current_expected, ", ");
    }
    cspec_string_add_double_precision(cspec->current_actual, actual[len - 1]);
    cspec_string_add_str(cspec->current_actual, "]");

    cspec_string_add_double_precision(cspec->current_expected, expected[len - 1]);
    cspec_string_add_str(cspec->current_expected, "]");
}

/**
 * @func: cspec_double_array_comparison
 * @desc: Conpares two double arrays for equality in elements
 * @param actual -> The value passed by the user
 * @param expected -> The value `actual` is tested against
 * @param len -> The length of the arrays
 * @return a boolean
 **/
static bool cspec_double_array_comparison(double *actual, double *expected, size_t len) {
    /* Check for members */
    size_t i;
    for(i = 0; i < len; i++)
        if(cspec_fabs(actual[i] - expected[i]) > 1E-12) return false;

    /* All elements are equal */
    return true;
}

/**
 * @func: cspec_call_assert_that_double_array
 * @desc: Assertion of two int arrays
 **/
define_assert_array(
    cspec_call_assert_that_double_array,
    double*,
    cspec_to_string_double_array_write,
    !cspec_double_array_comparison,
    len
)

/**
 * @func: cspec_call_nassert_that_double_array
 * @desc: Negative assertion of two double arrays
 **/
define_assert_array(
    cspec_call_nassert_that_double_array,
    double*,
    cspec_to_string_double_array_write,
    cspec_double_array_comparison,
    len
)

/**
 * @macro: assert_that_double_array
 * @desc: Assert that the expected double array equals to the result
 * @param actual -> The actual value
 * @param expected -> The expected value
 **/
#define assert_that_double_array(inner) CSPEC_BLOCK( \
    cspec->current_file = __FILE__; \
    cspec->current_line = __LINE__; \
    cspec_call_assert_that_double_array(inner); \
)

/**
 * @macro: nassert_that_double_array
 * @desc: Assert that the expected double array differs from the result
 * @param actual -> The actual value
 * @param expected -> The expected value
 **/
#define nassert_that_double_array(inner) CSPEC_BLOCK( \
    cspec->current_file = __FILE__; \
    cspec->current_line = __LINE__; \
    cspec_call_nassert_that_double_array(inner); \
)


/**
 * @macro: cspec_to_string_double_write
 * @desc: Writes actual and expected values
 * @param actual -> The value passed by the user
 * @param expected -> The value `actual` is tested against
 **/
/* TODO -> CONVERT FROM A BLOCK TO A FUNCTION */
#define cspec_to_string_double_write(actual, expected) CSPEC_BLOCK( \
    cspec->current_actual = cspec_string_new(""); \
    cspec->current_expected = cspec_string_new(""); \
    cspec_string_add_double_precision(cspec->current_actual, actual); \
    cspec_string_add_double_precision(cspec->current_expected, expected); \
)

/**
 * @func: cspec_double_comparison
 * @desc: A function that compares doubles for assertions
 * @param actual -> The value passed by the user
 * @param expected -> The value `actual` is tested against
 * @return a boolean
 **/
static bool cspec_double_comparison(double actual, double expected) {
    /* Calculate the margin to which the difference
        is too big so the test fails */
    return cspec_fabs(actual - expected) < 1E-12;
}

/**
 * @func: cspec_call_assert_that_double
 * @desc: Assert that the expected double is different than the result
 * @param actual -> The value passed by the user
 * @param expected -> The value `actual` is tested against
 **/
define_assert(
    cspec_call_assert_that_double,
    double,
    cspec_to_string_double_write,
    !cspec_double_comparison
)

/**
 * @func: cspec_call_nassert_that_double
 * @desc: Assert that the expected double is different than the result
 * @param actual -> The value passed by the user
 * @param expected -> The value `actual` is tested against
 **/
define_assert(
    cspec_call_nassert_that_double,
    double,
    cspec_to_string_double_write,
    cspec_double_comparison
)

/**
 * @macro: assert_that_double
 * @desc: Assert that the expected double is different than the result
 * @param actual -> The actual value
 * @param expected -> The expected double
 **/
#define assert_that_double(inner) CSPEC_BLOCK( \
    cspec->current_file = __FILE__; \
    cspec->current_line = __LINE__; \
    cspec_call_assert_that_double(inner); \
)

/**
 * @macro: nassert_that_double
 * @desc: Assert that the expected double is different than the result
 * @param actual -> The actual value
 * @param expected -> The expected double
 **/
#define nassert_that_double(inner) CSPEC_BLOCK( \
    cspec->current_file = __FILE__; \
    cspec->current_line = __LINE__; \
    cspec_call_nassert_that_double(inner); \
)


/**
 * @macro: assert_that
 * @desc: Asserts that a proc returns true
 * @param test -> The test proc to run
 **/
#define assert_that(test) CSPEC_BLOCK( \
    cspec->current_file = __FILE__; \
    cspec->current_line = __LINE__; \
    cspec->position_in_file = cspec_string_new(""); \
    cspec->assert_result = cspec_string_new(""); \
    \
    /* Save the test token */ \
    cspec->current_assert = #test; \
    \
    /* Assert a custom block of code */ \
    if(!(test)) { \
        cspec->status_of_test = CSPEC_FAILING; \
        cspec_write_position_in_file(); \
        \
        /* Craft the string builder for terminal output */ \
        cspec_string_add_str(cspec->test_result_message, cspec_string_get(cspec->display_tab)); \
        cspec_string_add_str(cspec->test_result_message, cspec_string_get(cspec->WHITE)); \
        cspec_string_add_str(cspec->test_result_message, "    "); \
        cspec_string_add_str(cspec->test_result_message, cspec_string_get(cspec->position_in_file)); \
        cspec_string_add_str(cspec->test_result_message, "\n"); \
        cspec_string_add_str(cspec->test_result_message, cspec_string_get(cspec->display_tab)); \
        cspec_string_add_str(cspec->test_result_message, "        |> "); \
        cspec_string_add_str(cspec->test_result_message, cspec_string_get(cspec->RED)); \
        cspec_string_add_str(cspec->test_result_message, "`"); \
        cspec_string_add_str(cspec->test_result_message, #test); \
        cspec_string_add_str(cspec->test_result_message, "`"); \
        cspec_string_add_str(cspec->test_result_message, cspec_string_get(cspec->WHITE)); \
        cspec_string_add_str(cspec->test_result_message, " should be true\n"); \
        cspec_string_add_str(cspec->test_result_message, cspec_string_get(cspec->RESET)); \
/*************************************************************************/ \
        /* Craft the string builder for text output (miss the color codes) */ \
        cspec_string_add_str(cspec->assert_result, "|> `"); \
        cspec_string_add_str(cspec->assert_result, #test); \
        cspec_string_add_str(cspec->assert_result, "`"); \
        cspec_string_add_str(cspec->assert_result, " should be true"); \
/****************************************************************************/ \
        cspec_vector *list_of_strings = cspec_vector_new(); \
        cspec_vector_add(list_of_strings, cspec_string_get(cspec->position_in_file)); \
        cspec_vector_add(list_of_strings, cspec_string_get(cspec->assert_result)); \
/****************************************************************************/ \
        cspec_vector_add(cspec->list_of_asserts, list_of_strings); \
    } \
)

/**
 * @macro: nassert_that
 * @desc: Asserts that a proc returns false
 * @param test -> The test proc to run
 **/
#define nassert_that(test) CSPEC_BLOCK( \
    cspec->current_file = __FILE__; \
    cspec->current_line = __LINE__; \
    cspec->position_in_file = cspec_string_new(""); \
    cspec->assert_result = cspec_string_new(""); \
    \
    /* Save the test token */ \
    cspec->current_assert = #test; \
    \
	if((test)) { \
        cspec->status_of_test = CSPEC_FAILING; \
        cspec_write_position_in_file(); \
        \
        /* Craft the string builder for terminal output */ \
        cspec_string_add_str(cspec->test_result_message, cspec_string_get(cspec->display_tab)); \
        cspec_string_add_str(cspec->test_result_message, cspec_string_get(cspec->WHITE)); \
        cspec_string_add_str(cspec->test_result_message, "    "); \
        cspec_string_add_str(cspec->test_result_message, cspec_string_get(cspec->position_in_file)); \
        cspec_string_add_str(cspec->test_result_message, "\n"); \
        cspec_string_add_str(cspec->test_result_message, cspec_string_get(cspec->display_tab)); \
        cspec_string_add_str(cspec->test_result_message, "        |> "); \
        cspec_string_add_str(cspec->test_result_message, cspec_string_get(cspec->RED)); \
        cspec_string_add_str(cspec->test_result_message, "`"); \
        cspec_string_add_str(cspec->test_result_message, #test); \
        cspec_string_add_str(cspec->test_result_message, "`"); \
        cspec_string_add_str(cspec->test_result_message, cspec_string_get(cspec->WHITE)); \
        cspec_string_add_str(cspec->test_result_message, " should be false\n"); \
        cspec_string_add_str(cspec->test_result_message, cspec_string_get(cspec->RESET)); \
/*************************************************************************/ \
        /* Craft the string builder for text output (miss the color codes) */ \
        cspec_string_add_str(cspec->assert_result, "|> `"); \
        cspec_string_add_str(cspec->assert_result, #test); \
        cspec_string_add_str(cspec->assert_result, "`"); \
        cspec_string_add_str(cspec->assert_result, " should be false"); \
/****************************************************************************/ \
        cspec_vector *list_of_strings = cspec_vector_new(); \
        cspec_vector_add(list_of_strings, cspec_string_get(cspec->position_in_file)); \
        cspec_vector_add(list_of_strings, cspec_string_get(cspec->assert_result)); \
/****************************************************************************/ \
        cspec_vector_add(cspec->list_of_asserts, list_of_strings); \
    } \
)


/**
 * @func: cspec_to_string_int_array_write
 * @desc: Writes actual and expected values
 * @param actual -> The value passed by the user
 * @param expected -> The value `actual` is tested against
 * @param len -> The length of the arrays
 **/
static void cspec_to_string_int_array_write(int *actual, int *expected, size_t len) {
    size_t i;

    cspec->current_actual = cspec_string_new("[");
    cspec->current_expected = cspec_string_new("[");

    for(i = 0; i < len - 1; i++) {
        cspec_string_add_int(cspec->current_actual, actual[i]);
        cspec_string_add_str(cspec->current_actual, ", ");

        cspec_string_add_int(cspec->current_expected, expected[i]);
        cspec_string_add_str(cspec->current_expected, ", ");
    }
    cspec_string_add_int(cspec->current_actual, actual[len - 1]);
    cspec_string_add_str(cspec->current_actual, "]");

    cspec_string_add_int(cspec->current_expected, expected[len - 1]);
    cspec_string_add_str(cspec->current_expected, "]");
}

/**
 * @func: cspec_int_array_comparison
 * @desc: Compares two int arrays for equality in elements
 * @param actual -> The value passed by the user
 * @param expected -> The value `actual` is tested against
 * @param len -> The length of the arrays
 * @return a boolean
 **/
static bool cspec_int_array_comparison(int *actual, int *expected, size_t len) {
    /* Check for members */
    size_t i;
    for(i = 0; i < len; i++)
        if(actual[i] != expected[i]) return false;

    /* All elements are equal */
    return true;
}

/**
 * @func: cspec_call_assert_that_int_array
 * @desc: Assertion of two int arrays
 **/
define_assert_array(
    cspec_call_assert_that_int_array,
    int*,
    cspec_to_string_int_array_write,
    !cspec_int_array_comparison,
    len
)

/**
 * @func: cspec_call_nassert_that_int_array
 * @desc: Negative assertion of two int arrays
 **/
define_assert_array(
    cspec_call_nassert_that_int_array,
    int*,
    cspec_to_string_int_array_write,
    cspec_int_array_comparison,
    len
)

/**
 * @macro: assert_that_int_array
 * @desc: Assert that the expected int array equals to the the result
 * @param actual -> The actual value
 * @param expected -> The expected value
 **/
#define assert_that_int_array(inner) CSPEC_BLOCK( \
    cspec->current_file = __FILE__; \
    cspec->current_line = __LINE__; \
    cspec_call_assert_that_int_array(inner); \
)

/**
 * @macro: nassert_that_int_array
 * @desc: Assert that the expected int array differs from the result
 * @param actual -> The actual value
 * @param expected -> The expected value
 **/
#define nassert_that_int_array(inner) CSPEC_BLOCK( \
    cspec->current_file = __FILE__; \
    cspec->current_line = __LINE__; \
    cspec_call_nassert_that_int_array(inner); \
)


/**
 * @macro: cspec_to_string_int_write
 * @desc: Writes actual and expected values
 * @param actual -> The value passed by the user
 * @param expected -> The value `actual` is tested against
 **/
/* TODO -> CONVERT FROM A BLOCK TO A FUNCTION */
#define cspec_to_string_int_write(actual, expected) CSPEC_BLOCK( \
    cspec->current_actual = cspec_string_new(""); \
    cspec->current_expected = cspec_string_new(""); \
    cspec_string_add_int(cspec->current_actual, actual); \
    cspec_string_add_int(cspec->current_expected, expected); \
)

/**
 * @func: cspec_int_comparison
 * @desc: A function that compares integers for assertions
 * @param actual -> The value passed by the user
 * @param expected -> The value `actual` is tested against
 * @return a boolean
 **/
static bool cspec_int_comparison(int actual, int expected) {
    return actual == expected;
}

/**
 * @func: cspec_call_assert_that_int
 * @desc: Assert that the expected integer is equal to the result
 * @param actual -> The value passed by the user
 * @param expected -> The value `actual` is tested against
 **/
define_assert(
    cspec_call_assert_that_int,
    int,
    cspec_to_string_int_write,
    !cspec_int_comparison
)

/**
 * @func: cspec_call_nassert_that_int
 * @desc: Assert that the expected integer is different than the result
 * @param actual -> The value passed by the user
 * @param expected -> The value `actual` is tested against
 **/
define_assert(
    cspec_call_nassert_that_int,
    int,
    cspec_to_string_int_write,
    cspec_int_comparison
)

/**
 * @macro: assert_that_int
 * @desc: Assert that the expected integer is equal to the result
 * @param actual -> The actual value
 * @param expected -> The expected int
 **/
#define assert_that_int(inner) CSPEC_BLOCK( \
    cspec->current_file = __FILE__; \
    cspec->current_line = __LINE__; \
    cspec_call_assert_that_int(inner); \
)

/**
 * @macro: nassert_that_int
 * @desc: Assert that the expected integer is different than the result
 * @param actual -> The actual value
 * @param expected -> The expected int
 **/
#define nassert_that_int(inner) CSPEC_BLOCK( \
    cspec->current_file = __FILE__; \
    cspec->current_line = __LINE__; \
    cspec_call_nassert_that_int(inner); \
)


#endif
