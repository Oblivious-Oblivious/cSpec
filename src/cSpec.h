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
 * @func: _get_timer
 * @desc: A cross platform timer function for profiling
 * @return The time in nanoseconds
 **/
static size_t _get_timer(void) {
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
        if(0 == is_init) {
            clock_getres(CLOCKID, &linux_rate);
            is_init = 1;
        }
        size_t now;
        struct timespec spec;
        clock_gettime(CLOCKID, &spec);
        now = spec.tv_sec * 1.0e9 + spec.tv_nsec;
        return now;
    #endif
}


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
 * @param len -> The total len of the string
 * @param persistance -> A flag signaling the persistence state of the string
 **/
typedef struct _string {
    char *str;
    size_t alloced;
    size_t len;
} _string;

/**
 * @struct: _vector
 * @desc: Defines a vector data structure
 * @param items -> A void pointer array that contains the heterogenous elements of the vector
 * @param alloced -> The total capacity of the vector
 * @param len -> The total number of values
 **/
typedef struct _vector {
    void **items;
    size_t alloced;
    size_t len;
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
 * @func: _fabs
 * @desc: Abs for floats
 * @param value -> The value to get `abs` for
 * @return Absolute value
 **/
static double _fabs(double value) {
    return value < 0 ? (-value) : (value);
}

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
 * @param add_len -> he len that needs to be added *not* including a NULL terminator
 **/
static void _string_ensure_space(_string *sb, size_t add_len) {
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
    __memmove(sb->str+sb->len, str, len);

    /* Reset len and NULL terminate */
    sb->len += len;
    sb->str[sb->len] = '\0';
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
    sb->len = 0;
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
 * @param len -> The new len of the string, anything after this len is removed
 **/
static void _string_shorten(_string *sb, size_t len) {
    if(sb == NULL || len >= sb->len) return;

    /* Reset the len and NULL terminate, ingoring
        all values right to the NULL from memory */
    sb->len = len;
    sb->str[sb->len] = '\0';
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
 * @param len -> The len to remove
 **/
static void _string_skip(_string *sb, size_t len) {
    if(sb == NULL || len == 0) return;

    if(len >= sb->len) {
        /* If we choose to drop more bytes than the
            string has simply clear the string */
        _string_delete(sb);
        return;
    }

    sb->len -= len;

    /* +1 to move the NULL. */
    __memmove(sb->str, sb->str + len, sb->len + 1);
}

/**
 * @func: _string_length
 * @desc: The len of the string contained in the builder
 * @param sb -> The string builder to use
 * @return The current len of the string
 **/
static size_t _string_length(_string *sb) {
    if(sb == NULL) return 0;
    return sb->len;
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
    v->len = 0;
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
    if(v->alloced == v->len)
        _vector_ensure_space(v, v->alloced * 2);
    v->items[v->len++] = item;
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
    if(index >= 0 && index < v->len)
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
    return v->len;
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


/**
 * @param _PASSING -> Set for passing tests
 * @param _FAILING -> Set for failing tests
 * @param _SKIPPED -> Set for skipeed tests
 **/
#define _PASSING 1
#define _FAILING 0
#define _SKIPPED -1

/**
 * @macro: _BLOCK
 * @desc: Expands to a do while loop that runs once
 *      Usefull for executing naked blocks
 * @param ... -> The block of code to run
 **/
#define _BLOCK(...) do { \
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
 * @macro: spec
 * @desc: Call the main automation function
 **/
#define spec(...) \
    int main(int argc, char **argv) { \
        _BLOCK(__VA_ARGS__); \
    }

/**
 * @macro: spec_suite
 * @desc: A simple function definition for running test suites
 * @param ... -> The block of modules to run
 **/
#define spec_suite(...) \
    /* Define a function to run tests modules at */ \
    static void run_spec_suite(char *type_of_tests) { \
        _setup_test_data(); \
        _cspec->type_of_tests = _new_string(type_of_tests); \
        \
        \
        /* Check for valid test type */ \
        if(!__streql(type_of_tests, "passing") \
        && !__streql(type_of_tests, "failing") \
        && !__streql(type_of_tests, "skipped") \
        && !__streql(type_of_tests, "all")) { \
            printf("\n\033[1;31mInput a type of test to log passing|failing|skipped|all\033[0m\n\n"); \
            return; /* Exit the function */ \
        } \
        \
        \
        _BLOCK(__VA_ARGS__); \
        _report_time_taken_for_tests(); \
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
        _cspec->name_of_module = _new_string(#suite_name); \
        _cspec->list_of_describes = _new_vector(); \
        \
        \
        /* Print module information */ \
        printf("\n%s%sModule `%s`%s\n", \
        _string_get(_cspec->BACK_PURPLE), \
        _string_get(_cspec->YELLOW), \
        _string_get(_cspec->name_of_module), \
        _string_get(_cspec->RESET)); \
        _cspec->display_tab = _new_string(""); \
        \
        \
        _BLOCK(__VA_ARGS__); /* Run describes */ \
        \
        \
        /* Construct a module and save for exporting */ \
        _vector *mod = _new_vector(); \
        _vector_add(mod, _cspec->list_of_describes); \
        _vector_add(mod, _cspec->name_of_module); \
        _vector_add(_cspec->list_of_modules, mod); \
        \
        /* mod([describes], name) */ \
    }

/**
 * @macro: before
 * @desc: Expands to a setup proc that gets executed before the tests
 * @param ... -> The proc to run
 **/
#define before(...) _BLOCK( \
    __VA_ARGS__ \
)

/**
 * @marco: after
 * @desc: Expands to a teardown proc that gets executed after the tests
 * @param ... -> The proc to run
 **/
#define after(...) _BLOCK( \
    __VA_ARGS__ \
)

/**
 * @macro: before_each
 * @desc: Sets the argument to a function to run before each it block
 * @param func -> The func to set
 **/
#define before_each(func) _BLOCK( \
    _cspec->before_func = func; \
)

/**
 * @marco: after_each
 * @desc: Sets the argument to a function to run after each it block
 * @param func -> The func to set
 **/
#define after_each(func) _BLOCK( \
    _cspec->after_func = func; \
)

/**
 * @macro: _describe
 * @desc: Defines a `describe` level block
 * @param __VA_ARGS__
 **/
#define _describe(...) _BLOCK( \
    if(_cspec->inner_nest == -1) _cspec->outer_nest = 0; \
    \
    /* On every describe block we nest inwards */ \
    _cspec->inner_nest++; \
    \
    __VA_ARGS__ \
    \
    /* Reset nests */ \
    _cspec->outer_nest++; \
    _cspec->inner_nest--; \
)

/**
 * @marco: describe
 * @desc: Expands to a block execution where tests are encompassed
 * @param object_name -> The name of the unit to describe
 * @param ... -> The proc to extend to
 **/
#define describe(object_name, ...) _BLOCK( \
    _string_add_str(_cspec->display_tab, "    "); \
    _cspec->list_of_its = _new_vector(); \
    _cspec->list_of_contexts = _new_vector(); \
    _cspec->name_of_describe = _new_string(object_name); \
    \
    \
    /* Display describe data */ \
    printf("%s%s`%s`%s\n", \
    _string_get(_cspec->display_tab), \
    _string_get(_cspec->PURPLE), \
    object_name, \
    _string_get(_cspec->RESET)); \
    \
    \
    /* Construct the describe block */ \
    _vector *describe_block = _new_vector(); \
    _vector_add(describe_block, _vector_dup(_cspec->list_of_its)); \
    _vector_add(describe_block, _vector_dup(_cspec->list_of_contexts)); \
    _vector_add(describe_block, _cspec->name_of_describe); \
    _vector_add(_cspec->list_of_describes, describe_block); \
    \
    \
    _describe(__VA_ARGS__); \
    \
    \
    _string_skip(_cspec->display_tab, 4); \
    \
    /* desc([its], [contexts], name)*/ \
)

/**
 * @macro: context
 * @desc: Basically aliasing for descibe
 * @param object_name -> The name of the unit to describe
 * @param ... -> The proc to extend to
 **/
#define context(object_name, ...) _BLOCK( \
    _cspec->in_context_block = true; \
    _string_add_str(_cspec->display_tab, "    "); \
    _cspec->list_of_its = _new_vector(); \
    _cspec->name_of_context = _new_string(object_name); \
    \
    \
    /* Display context data */ \
    printf("%s%s`%s`%s\n", \
    _string_get(_cspec->display_tab), \
    _string_get(_cspec->YELLOW), \
    object_name, \
    _string_get(_cspec->WHITE)); \
    \
    \
    /* Construct data for export */ \
    _vector *context_block = _new_vector(); \
    _vector_add(context_block, _vector_dup(_cspec->list_of_its)); \
    _vector_add(context_block, _cspec->name_of_context); \
    \
    /* Grab the specific describe block using `inner_nest` */ \
    _vector *desc_block = _vector_get(_cspec->list_of_describes, _cspec->inner_nest); \
    _vector *list_of_cons = _vector_get(desc_block, 1); \
    _vector_add(list_of_cons, context_block); \
    \
    \
    _describe(__VA_ARGS__); \
    \
    \
    _string_skip(_cspec->display_tab, 4); \
    _cspec->in_context_block = false; \
    \
    /* con([its], name) */ \
)

/**
 * @macro: xit
 * @desc: Temporarily disables a test
 * @param proc_name -> The name of test to run
 * @param ... -> The actual test code
 **/
#define xit(proc_name, ...) _BLOCK( \
    /* Execute a `before` function */ \
    if(_cspec->before_func) (*_cspec->before_func)(); \
    _string_add_str(_cspec->display_tab, "    "); \
    \
    /* Setup for a skipped it block */ \
    _cspec->number_of_tests++; \
    _cspec->number_of_skipped_tests++; \
    _cspec->test_result_message = _new_string(""); \
    _cspec->name_of_tested_proc = _new_string(proc_name); \
    _cspec->list_of_asserts = _new_vector(); \
    _cspec->status_of_test = _SKIPPED; \
    \
    /* Dummy timers */ \
    size_t start_test_timer = _get_timer(); \
    size_t end_test_timer = _get_timer(); \
    \
    /* Check for valid test type */ \
    if((__streql(_string_get(_cspec->type_of_tests), "all")) \
    || (__streql(_string_get(_cspec->type_of_tests), "skipped"))) { \
        /* Display data about the skipped it */ \
        printf("%s%s- %s%s\n", \
        _string_get(_cspec->display_tab), \
        _string_get(_cspec->GRAY), \
        _string_get(_cspec->name_of_tested_proc), \
        _string_get(_cspec->RESET)); \
        \
        \
    } \
    _cspec->total_time_taken_for_tests += end_test_timer - start_test_timer; \
    \
    \
    _insert_it_block_in_list_of_its(); \
    \
    \
    /* Reset the display tab and execute the `after_func` */ \
    _string_skip(_cspec->display_tab, 4); \
    if(_cspec->after_func) (*_cspec->after_func)(); \
)

/**
 * @macro: it
 * @desc: Expands to a test run and saves all data gathered
 * @param proc_name -> The name of test to run
 * @param proc -> The actual test code
 **/
#define it(proc_name, ...) _BLOCK( \
    /* Execute a `before` function */ \
    if(_cspec->before_func) (*_cspec->before_func)(); \
    \
    /* Setup for an it block */ \
    _string_add_str(_cspec->display_tab, "    "); \
	_cspec->number_of_tests++; \
    _cspec->test_result_message = _new_string(""); \
    _cspec->name_of_tested_proc = _new_string(proc_name); \
    _cspec->list_of_asserts = _new_vector(); \
    \
    /* Assume its a passing test */ \
    _cspec->status_of_test = _PASSING; \
    _cspec->current_line = __LINE__; \
    _cspec->current_file = __FILE__; \
    \
    size_t start_test_timer = _get_timer(); \
    \
    /* Execute asserts */ \
    __VA_ARGS__ \
    /* jmp_buf _cspec->escape; */ \
    /* setjmp(&_cspec->escape); */ \
    size_t end_test_timer = _get_timer(); \
    \
    if(_cspec->status_of_test) { \
        /* In case all asserts pass we print passing test information */ \
        _cspec->number_of_passing_tests++; \
        if((__streql(_string_get(_cspec->type_of_tests), "all")) \
        || (__streql(_string_get(_cspec->type_of_tests), "passing"))) { \
            printf("%s%s✓%s it %s%s\n", \
            _string_get(_cspec->display_tab), \
            _string_get(_cspec->GREEN), \
            _string_get(_cspec->WHITE), \
            _string_get(_cspec->name_of_tested_proc), \
            _string_get(_cspec->RESET)); \
        } \
	} \
    else { \
        /* Even if 1 of the asserts in the current it block
            fails, assume we have a failing test */ \
		_cspec->number_of_failing_tests++; \
        if((__streql(_string_get(_cspec->type_of_tests), "all")) \
        || (__streql(_string_get(_cspec->type_of_tests), "failing"))) { \
            printf("%s%s✗%s it %s:\n%s%s\n", \
            _string_get(_cspec->display_tab), \
            _string_get(_cspec->RED), \
            _string_get(_cspec->WHITE), \
            _string_get(_cspec->name_of_tested_proc), \
            _string_get(_cspec->test_result_message), \
            _string_get(_cspec->RESET)); \
        } \
    } \
    /* Calculate the total time */ \
    _cspec->total_time_taken_for_tests += end_test_timer - start_test_timer; \
    \
    \
    _insert_it_block_in_list_of_its(); \
    \
    \
    /* Reset the display tab and execute the `after_func` */ \
    _string_skip(_cspec->display_tab, 4); \
    if(_cspec->after_func) (*_cspec->after_func)(); \
)


/**
 * @macro: assert_that
 * @desc: Asserts that a proc returns true
 * @param test -> The test proc to run
 **/
#define assert_that(test) _BLOCK( \
    _cspec->current_file = __FILE__; \
    _cspec->current_line = __LINE__; \
    _cspec->position_in_file = _new_string(""); \
    _cspec->assert_result = _new_string(""); \
    \
    /* Save the test token */ \
    _cspec->current_assert = #test; \
    \
    /* Assert a custom block of code */ \
	if(!(test)) { \
        _cspec->status_of_test = _FAILING; \
        _write_position_in_file(); \
        \
        /* Craft the string builder for terminal output */ \
        _string_add_str(_cspec->test_result_message, _string_get(_cspec->display_tab)); \
        _string_add_str(_cspec->test_result_message, _string_get(_cspec->WHITE)); \
        _string_add_str(_cspec->test_result_message, "    "); \
        _string_add_str(_cspec->test_result_message, _string_get(_cspec->position_in_file)); \
        _string_add_str(_cspec->test_result_message, "\n"); \
        _string_add_str(_cspec->test_result_message, _string_get(_cspec->display_tab)); \
        _string_add_str(_cspec->test_result_message, "        |> "); \
        _string_add_str(_cspec->test_result_message, _string_get(_cspec->RED)); \
        _string_add_str(_cspec->test_result_message, "`"); \
        _string_add_str(_cspec->test_result_message, #test); \
        _string_add_str(_cspec->test_result_message, "`"); \
        _string_add_str(_cspec->test_result_message, _string_get(_cspec->WHITE)); \
        _string_add_str(_cspec->test_result_message, " should be true\n"); \
        _string_add_str(_cspec->test_result_message, _string_get(_cspec->RESET)); \
/*************************************************************************/ \
        /* Craft the string builder for text output (miss the color codes) */ \
        _string_add_str(_cspec->assert_result, "|> `"); \
        _string_add_str(_cspec->assert_result, #test); \
        _string_add_str(_cspec->assert_result, "`"); \
        _string_add_str(_cspec->assert_result, " should be true"); \
/****************************************************************************/ \
        _vector *list_of_strings = _new_vector(); \
        _vector_add(list_of_strings, _string_get(_cspec->position_in_file)); \
        _vector_add(list_of_strings, _string_get(_cspec->assert_result)); \
/****************************************************************************/ \
        _vector_add(_cspec->list_of_asserts, list_of_strings); \
    } \
    \
    /* assert(string_builder) */ \
)

/**
 * @macro: nassert_that
 * @desc: Asserts that a proc returns false
 * @param test -> The test proc to run
 **/
#define nassert_that(test) _BLOCK( \
    _cspec->current_file = __FILE__; \
    _cspec->current_line = __LINE__; \
    _cspec->position_in_file = _new_string(""); \
    _cspec->assert_result = _new_string(""); \
    \
    /* Save the test token */ \
    _cspec->current_assert = #test; \
    \
	if((test)) { \
        _cspec->status_of_test = _FAILING; \
        _write_position_in_file(); \
        \
        /* Craft the string builder for terminal output */ \
        _string_add_str(_cspec->test_result_message, _string_get(_cspec->display_tab)); \
        _string_add_str(_cspec->test_result_message, _string_get(_cspec->WHITE)); \
        _string_add_str(_cspec->test_result_message, "    "); \
        _string_add_str(_cspec->test_result_message, _string_get(_cspec->position_in_file)); \
        _string_add_str(_cspec->test_result_message, "\n"); \
        _string_add_str(_cspec->test_result_message, _string_get(_cspec->display_tab)); \
        _string_add_str(_cspec->test_result_message, "        |> "); \
        _string_add_str(_cspec->test_result_message, _string_get(_cspec->RED)); \
        _string_add_str(_cspec->test_result_message, "`"); \
        _string_add_str(_cspec->test_result_message, #test); \
        _string_add_str(_cspec->test_result_message, "`"); \
        _string_add_str(_cspec->test_result_message, _string_get(_cspec->WHITE)); \
        _string_add_str(_cspec->test_result_message, " should be false\n"); \
        _string_add_str(_cspec->test_result_message, _string_get(_cspec->RESET)); \
/*************************************************************************/ \
        /* Craft the string builder for text output (miss the color codes) */ \
        _string_add_str(_cspec->assert_result, "|> `"); \
        _string_add_str(_cspec->assert_result, #test); \
        _string_add_str(_cspec->assert_result, "`"); \
        _string_add_str(_cspec->assert_result, " should be false"); \
/****************************************************************************/ \
        _vector *list_of_strings = _new_vector(); \
        _vector_add(list_of_strings, _string_get(_cspec->position_in_file)); \
        _vector_add(list_of_strings, _string_get(_cspec->assert_result)); \
/****************************************************************************/ \
        _vector_add(_cspec->list_of_asserts, list_of_strings); \
    } \
)

/**
 * @macro: export_test_results
 * @desc: Exports the test results to some file type
 * @param vec -> Either passing|failing|skipped|all
 * @param type -> Export type either txt|xml|markdown
 **/
#define export_test_results(name, vec, type) _BLOCK( \
    /* Check for valid type of test export */ \
    if(!__streql(vec, "passing") \
    && !__streql(vec, "failing") \
    && !__streql(vec, "skipped") \
    && !__streql(vec, "all")) { \
        printf("\n%sInput a type of test to export passing|failing|skipped|all%s\n\n", \
        _string_get(_cspec->RED), \
        _string_get(_cspec->RESET)); \
        return 0; \
    } \
    _cspec->type_of_export_tests = _new_string(vec); \
    \
    /* Reset the display tab */ \
    _string_delete(_cspec->display_tab); \
    _string_add_str(_cspec->display_tab, "    "); \
    if(type == "txt") { \
        _cspec->fd = fopen(name, "w+"); \
        _export_to_txt(); \
    } \
    else if(type == "xml") { \
        _cspec->fd = fopen(name, "w+"); \
        _export_to_xml(); \
    } \
    else if(type == "markdown") { \
        _cspec->fd = fopen(name, "w+"); \
        _export_to_md(); \
    } \
    else { \
        printf("\n%sSpecify the export type: `txt|xml|markdown`%s\n\n", \
        _string_get(_cspec->RED), \
        _string_get(_cspec->RESET)); \
        return 0; /* Exit the main function */ \
    } \
)

/**
 * @macro: _compare_values
 * @desc: Runs the actual assertion between 2 values
 * @param actual -> The value passed by the user
 * @param expected -> The value `actual` is tested against
 * @param func -> The assert function according to data type
 **/
#define _compare_values(actual, expected, func) _BLOCK( \
    if(func(actual, expected)) { \
        _cspec->status_of_test = _FAILING; \
        _write_position_in_file(); \
        _write_assert_actual_expected(); \
    } \
)

/**
 * @macro: _compare_array_values
 * @desc: Runs the actual assertion between 2 values
 * @param actual -> The value passed by the user
 * @param expected -> The value `actual` is tested against
 * @param func -> The assert function according to data type
 * @param len -> The length of the array to test
 **/
#define _compare_array_values(actual, expected, func, len) _BLOCK( \
    if(func(actual, expected, len)) { \
        _cspec->status_of_test = _FAILING; \
        _write_position_in_file(); \
        _write_assert_actual_expected(); \
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
        _compare_values(actual, expected, comparison_method); \
    }

#define define_assert_array(name_of_assert, data_type_token, to_string_method, comparison_method, len) \
    static inline void name_of_assert(data_type_token actual, data_type_token expected, size_t len) { \
        to_string_method(actual, expected, len); \
        _compare_array_values(actual, expected, comparison_method, len); \
    }


/**
 * @func: _error_assertion_not_supported
 * @desc: Throw an error message
 **/
static void _error_assertion_not_supported(void) {
    printf("Error. Generic assertions not supported for this compiler.\n");
}

/**
 * @func: _write_position_in_file
 * @desc: Setup file position and line number for the current assert
 **/
static void _write_position_in_file(void) {
    _cspec->position_in_file = _new_string("");
    _string_add_str(_cspec->position_in_file, _cspec->current_file);
    _string_add_str(_cspec->position_in_file, ":");
    _string_add_int(_cspec->position_in_file, _cspec->current_line);
    _string_add_str(_cspec->position_in_file, ":");
}

/**
 * @func: _write_assert_actual_expected
 * @desc: Setup the assert description for printing and exporting
 **/
static void _write_assert_actual_expected(void) {
    _cspec->assert_result = _new_string("");
    _string_add_str(_cspec->test_result_message, _string_get(_cspec->display_tab));
    _string_add_str(_cspec->test_result_message, _string_get(_cspec->WHITE));
    _string_add_str(_cspec->test_result_message, "    ");
    _string_add_str(_cspec->test_result_message, _string_get(_cspec->position_in_file));
    _string_add_str(_cspec->test_result_message, "\n");
    _string_add_str(_cspec->test_result_message, _string_get(_cspec->display_tab));
    _string_add_str(_cspec->test_result_message, "        |> ");
    _string_add_str(_cspec->test_result_message, "`");
    _string_add_str(_cspec->test_result_message, _string_get(_cspec->current_expected));
    _string_add_str(_cspec->test_result_message, "` expected but got ");
    _string_add_str(_cspec->test_result_message, _string_get(_cspec->RED));
    _string_add_str(_cspec->test_result_message, "`");
    _string_add_str(_cspec->test_result_message, _string_get(_cspec->current_actual));
    _string_add_str(_cspec->test_result_message, "`\n");
    _string_add_str(_cspec->test_result_message, _string_get(_cspec->RESET));
/****************************************************************************/
    _string_add_str(_cspec->assert_result, "|> `");
    _string_add_str(_cspec->assert_result, _string_get(_cspec->current_expected));
    _string_add_str(_cspec->assert_result, "` expected but got ");
    _string_add_str(_cspec->assert_result, "`");
    _string_add_str(_cspec->assert_result, _string_get(_cspec->current_actual));
/****************************************************************************/
    _vector *list_of_strings = _new_vector();
    _vector_add(list_of_strings, _string_get(_cspec->position_in_file));
    _vector_add(list_of_strings, _string_get(_cspec->assert_result));
/****************************************************************************/
    _vector_add(_cspec->list_of_asserts, list_of_strings);
}

/**
 * @func: _write_nassert_actual_expected
 * @desc: Setup the `not` assert description for printing and exporting
 **/
static void _write_nassert_actual_expected(void) {
    _cspec->assert_result = _new_string("");
    _string_add_str(_cspec->test_result_message, _string_get(_cspec->display_tab));
    _string_add_str(_cspec->test_result_message, _string_get(_cspec->WHITE));
    _string_add_str(_cspec->test_result_message, "    ");
    _string_add_str(_cspec->test_result_message, _string_get(_cspec->position_in_file));
    _string_add_str(_cspec->test_result_message, "\n");
    _string_add_str(_cspec->test_result_message, _string_get(_cspec->display_tab));
    _string_add_str(_cspec->test_result_message, "        |> ");
    _string_add_str(_cspec->test_result_message, "expected that `");
    _string_add_str(_cspec->test_result_message, _string_get(_cspec->current_expected));
    _string_add_str(_cspec->test_result_message, "` would differ from ");
    _string_add_str(_cspec->test_result_message, _string_get(_cspec->RED));
    _string_add_str(_cspec->test_result_message, "`");
    _string_add_str(_cspec->test_result_message, _string_get(_cspec->current_actual));
    _string_add_str(_cspec->test_result_message, "`");
    _string_add_str(_cspec->test_result_message, _string_get(_cspec->WHITE));
    _string_add_str(_cspec->test_result_message, " but they are the same\n");
    _string_add_str(_cspec->test_result_message, _string_get(_cspec->RESET));
/****************************************************************************/
    _string_add_str(_cspec->assert_result, "|> expected that `");
    _string_add_str(_cspec->assert_result, _string_get(_cspec->current_expected));
    _string_add_str(_cspec->assert_result, "` would differ from ");
    _string_add_str(_cspec->assert_result, "`");
    _string_add_str(_cspec->assert_result, _string_get(_cspec->current_actual));
    _string_add_str(_cspec->assert_result, "`");
    _string_add_str(_cspec->assert_result, " but they are the same");
/****************************************************************************/
    _vector *list_of_strings = _new_vector();
    _vector_add(list_of_strings, _string_get(_cspec->position_in_file));
    _vector_add(list_of_strings, _string_get(_cspec->assert_result));
/****************************************************************************/
    _vector_add(_cspec->list_of_asserts, list_of_strings);
}

/**
 * @func: _insert_it_block_in_list_of_its
 * @desc: Inserts the it block in its designated vector index
 **/
static void _insert_it_block_in_list_of_its(void) {
    /* Setup the it block vector */
    _vector *it_block = _new_vector();
    _vector_add(it_block, _cspec->list_of_asserts);
    _vector_add(it_block, _cspec->name_of_tested_proc);
    _vector_add(it_block, _cspec->status_of_test);

    if(_cspec->in_context_block) {
        /* Insert in a context block */
        if(_vector_length(_cspec->list_of_describes) > 1) {
            /* Stupid indexing for finding the correct nested blocks */
            _vector *desc_block = _vector_get(_cspec->list_of_describes,
                -(_vector_length(_cspec->list_of_describes) - _cspec->inner_nest - 1));
            _vector *list_of_cons = _vector_get(desc_block, 1);
            _vector *con_block = _vector_get(list_of_cons,
                _vector_length(list_of_cons) - 1);
            _vector *list_of_con_its = _vector_get(con_block, 0);
            _vector_add(list_of_con_its, it_block);
        }
        else {
            _vector *desc_block = _vector_get(_cspec->list_of_describes,
                _vector_length(_cspec->list_of_describes) - 1);
            _vector *list_of_cons = _vector_get(desc_block, 1);
            _vector *con_block = _vector_get(list_of_cons,
                _vector_length(list_of_cons) - 1);
            _vector *list_of_con_its = _vector_get(con_block, 0);
            _vector_add(list_of_con_its, it_block);
        }
    }
    else {
        /* Insert in a describe block */
        _vector *desc_block = _vector_get(_cspec->list_of_describes,
            _vector_length(_cspec->list_of_describes) - 1 - _cspec->outer_nest);
        _vector *list_of_desc_its = _vector_get(desc_block, 0);
        _vector_add(list_of_desc_its, it_block);
    }
    /* it([asserts], name, status) */
}

/**
 * @func: _report_time_taken_for_tests
 * @desc: Report the number of tests and time taken while testing
 **/
static void _report_time_taken_for_tests(void) {
    printf("\n%s● %d tests\n%s✓ %d passing\n%s✗ %d failing\n%s- %d skipped\n",
    _string_get(_cspec->YELLOW),
    _cspec->number_of_tests,
    _string_get(_cspec->GREEN),
    _cspec->number_of_passing_tests,
    _string_get(_cspec->RED),
    _cspec->number_of_failing_tests,
    _string_get(_cspec->GRAY),
    _cspec->number_of_skipped_tests,
    _string_get(_cspec->RESET));

    /* Print in seconds if the time is more than 100ms */
    if(_cspec->total_time_taken_for_tests > 100000000) {
        printf("%s★ Finished in %.5f seconds%s\n",
        _string_get(_cspec->CYAN),
        _cspec->total_time_taken_for_tests / 1000000000.0,
        _string_get(_cspec->RESET));
    }
    /* Else print in miliseconds */
    else {
        printf("%s★ Finished in %.5f ms%s\n",
        _string_get(_cspec->CYAN),
        _cspec->total_time_taken_for_tests / 1000000.0,
        _string_get(_cspec->RESET));
    }
}

/**
 * @func: _fprintf_asserts
 * @desc: Writes asserts to an output file
 * @param assert -> The assert string to export
 **/
static void _fprintf_asserts(_vector *assert) {
    fprintf(_cspec->fd, "%s            %s\n                %s\n\n",
    _string_get(_cspec->display_tab),
    _vector_get(assert, 0),
    _vector_get(assert, 1));
}

/**
 * @func: _fprintf_its
 * @desc: Writes it blocks to an output file
 * @param it_block -> The it block to iterate against
 **/
static void _fprintf_its(_vector *it_block) {
    /* Check for type of test */
    if(((int)_vector_get(it_block, 2) == _FAILING)) {
        if(__streql(_string_get(_cspec->type_of_export_tests), "failing")
        || __streql(_string_get(_cspec->type_of_export_tests), "all")) {
            /* Write failing tests */
            fprintf(_cspec->fd, "%s        ✗ it %s\n",
            _string_get(_cspec->display_tab),
            _string_get(_vector_get(it_block, 1)));
            _vector *asserts = _vector_get(it_block, 0);

            /* Call to print the asserts iteratevely for the current it block */
            _vector_map(asserts, _fprintf_asserts);
        }
    }
    else if(((int)_vector_get(it_block, 2) == _SKIPPED)) {
        if(__streql(_string_get(_cspec->type_of_export_tests), "skipped")
        || __streql(_string_get(_cspec->type_of_export_tests), "all")) {
            /* Write skipped tests */
            fprintf(_cspec->fd, "%s        - it %s\n",
            _string_get(_cspec->display_tab),
            _string_get(_vector_get(it_block, 1)));
        }
    }
    else if(((int)_vector_get(it_block, 2) == _PASSING)) {
        /* Write passing tests */
        if(__streql(_string_get(_cspec->type_of_export_tests), "passing")
        || __streql(_string_get(_cspec->type_of_export_tests), "all")) {
            fprintf(_cspec->fd, "%s        ✓ it %s\n",
            _string_get(_cspec->display_tab),
            _string_get(_vector_get(it_block, 1)));
        }
    }
}

/**
 * @func: _fprintf_cons
 * @desc: Writes context blocks to an output file
 * @param con -> The context block to iterate against
 **/
static void _fprintf_cons(_vector *con) {
    _string_add_str(_cspec->display_tab, "    ");

    fprintf(_cspec->fd, "        `%s`\n",
    _string_get(_vector_get(con, 1)));
    _vector *its = _vector_get(con, 0);

    /* Iterate over the it blocks under the context block */
    _vector_map(its, _fprintf_its);
    
    _string_skip(_cspec->display_tab, 4);
}

/**
 * @func: _fprintf_describes
 * @desc: Writes describe blocks to an output file
 * @param desc -> The describe block to iterate against
 **/
static void _fprintf_describes(_vector *desc) {
    fprintf(_cspec->fd, "    `%s`\n",
    _string_get(_vector_get(desc, 2)));

    /* Iterate over the it blocks under the describe block */
    _vector *its = _vector_get(desc, 0);
    _vector_map(its, _fprintf_its);

    /* Iterate over the context blocks under the describe block */
    _vector *cons = _vector_get(desc, 1);
    _vector_map(cons, _fprintf_cons);
}

/**
 * @func: _fprintf_modules
 * @desc: Writes module blocks to an output file
 * @param mod -> The module block to iterate against
 **/
static void _fprintf_modules(_vector *mod) {
    _string_delete(_cspec->display_tab);
    fprintf(_cspec->fd, "\nModule `%s`\n",
    _string_get(_vector_get(mod, 1)));
    _vector *descs = _vector_get(mod, 0);

    /* Iterate over the describe blocks under the module block */
    _vector_map(descs, _fprintf_describes);
}

/**
 * @func: _export_to_txt
 * @desc: Export test results into a txt file
 **/
static void _export_to_txt(void) {
    _vector_map(_cspec->list_of_modules, _fprintf_modules);
    fclose(_cspec->fd);
}

/**
 * @func: _xml_write_asserts
 * @desc: Writes asserts to an xml file
 * @param assert -> The assert block containing strings to print
 **/
static void _xml_write_asserts(_vector *assert) {
    fprintf(_cspec->fd, "%s                <failure>\n",
    _string_get(_cspec->display_tab));
    
    fprintf(_cspec->fd, "%s                    <position>%s</position>\n",
    _string_get(_cspec->display_tab),
    _vector_get(assert, 0));

    fprintf(_cspec->fd, "%s                    <error>%s</error>\n",
    _string_get(_cspec->display_tab),
    _vector_get(assert, 1));

    fprintf(_cspec->fd, "%s                </failure>\n",
    _string_get(_cspec->display_tab));
}

/**
 * @func: _xml_write_its
 * @desc: Writes it blocks to an xml file
 * @param it_block -> The it block to iterate against
 **/
static void _xml_write_its(_vector *it_block) {
    /* Check for type of test */
    if(((int)_vector_get(it_block, 2) == _FAILING)) {
        if(__streql(_string_get(_cspec->type_of_export_tests), "failing")
        || __streql(_string_get(_cspec->type_of_export_tests), "all")) {
            /* Write failing tests */
            fprintf(_cspec->fd, "            %s<it>\n                %s<name>%s</name>\n                %s<status>failing</status>\n",
            _string_get(_cspec->display_tab),
            _string_get(_cspec->display_tab),
            _string_get(_vector_get(it_block, 1)),
            _string_get(_cspec->display_tab));
            _vector *asserts = _vector_get(it_block, 0);

            /* Call to print the asserts iteratevely for the current it block */
            _vector_map(asserts, _xml_write_asserts);
            fprintf(_cspec->fd, "            %s</it>\n",
            _string_get(_cspec->display_tab));
        }
    }
    else if(((int)_vector_get(it_block, 2) == _SKIPPED)) {
        if(__streql(_string_get(_cspec->type_of_export_tests), "skipped")
        || __streql(_string_get(_cspec->type_of_export_tests), "all")) {
            /* Write skipped tests */
            fprintf(_cspec->fd, "            %s<xit>\n                %s<name>%s</name>\n                %s<status>skipped</status>\n",
            _string_get(_cspec->display_tab),
            _string_get(_cspec->display_tab),
            _string_get(_vector_get(it_block, 1)),
            _string_get(_cspec->display_tab));
            fprintf(_cspec->fd, "            %s</xit>\n",
            _string_get(_cspec->display_tab));
        }
    }
    else if(((int)_vector_get(it_block, 2) == _PASSING)) {
        /* Write passing tests */
        if(__streql(_string_get(_cspec->type_of_export_tests), "passing")
        || __streql(_string_get(_cspec->type_of_export_tests), "all")) {
            fprintf(_cspec->fd, "            %s<it>\n                %s<name>%s</name>\n                %s<status>passing</status>\n",
            _string_get(_cspec->display_tab),
            _string_get(_cspec->display_tab),
            _string_get(_vector_get(it_block, 1)),
            _string_get(_cspec->display_tab));
            fprintf(_cspec->fd, "            %s</it>\n",
            _string_get(_cspec->display_tab));
        }
    }
}

/**
 * @func: _xml_write_cons
 * @desc: Writes context blocks to an xml file
 * @param con -> The context block to iterate against
 **/
static void _xml_write_cons(_vector *con) {
    _string_add_str(_cspec->display_tab, "    ");
    fprintf(_cspec->fd, "            <context>\n                <name>%s</name>\n",
    _string_get(_vector_get(con, 1)));
    _vector *its = _vector_get(con, 0);

    /* Iterate over the it blocks under the context block */
    _vector_map(its, _xml_write_its);
    fprintf(_cspec->fd, "            </context>\n");
    _string_skip(_cspec->display_tab, 4);
}

/**
 * @func: _xml_write_describes
 * @desc: Writes describe blocks to an xml file
 * @param desc -> The current describe block to iterate against
 **/
static void _xml_write_describes(_vector *desc) {
    fprintf(_cspec->fd, "        <describe>\n            <name>%s</name>\n",
    _string_get(_vector_get(desc, 2)));

    /* Iterate over the it blocks under the describe block */
    _vector *its = _vector_get(desc, 0);
    _vector_map(its, _xml_write_its);

    /* Iterate over the context blocks under the describe block */
    _vector *cons = _vector_get(desc, 1);
    _vector_map(cons, _xml_write_cons);
    fprintf(_cspec->fd, "        </describe>\n");
}

/**
 * @func: _xml_write_modules
 * @desc: Writes module blocks to an xml file
 * @param mod -> The module block to iterate against
 **/
static void _xml_write_modules(_vector *mod) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    _string_delete(_cspec->display_tab);
    fprintf(_cspec->fd, "    <module>\n        <name>%s</name>\n        <failures>%d</failures>\n        <skipped>%d</skipped>\n        <tests>%d</tests>\n        <timestamp>%d-%02d-%02d %02d:%02d:%02d</timestamp>\n",
    _string_get(_vector_get(mod, 1)),
    _cspec->number_of_failing_tests,
    _cspec->number_of_skipped_tests,
    _cspec->number_of_tests,
    tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    _vector *descs = _vector_get(mod, 0);

    /* Iterate over the describe blocks under the module block */
    _vector_map(descs, _xml_write_describes);
    fprintf(_cspec->fd, "    </module>\n");
}

/**
 * @func: _export_to_xml
 * @desc: Export test results into an xml file
 **/
static void _export_to_xml(void) {
    fprintf(_cspec->fd, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");
    
    if(_cspec->total_time_taken_for_tests > 100000000) {
        fprintf(_cspec->fd, "<modules>\n    <duration>%.5f seconds</duration>\n",
        _cspec->total_time_taken_for_tests / 1000000000.0);
    }
    else {
        fprintf(_cspec->fd, "<modules>\n    <duration>%.5f ms</duration>\n",
        _cspec->total_time_taken_for_tests / 1000000.0);
    }
    
    _vector_map(_cspec->list_of_modules, _xml_write_modules);
    fprintf(_cspec->fd, "</modules>\n");
    fclose(_cspec->fd);
}

/**
 * @func: _export_to_md
 * @desc: Export test results into a markdown file
 **/
static void _export_to_md(void) {
}

/**
 * @func: _signal_handler
 * @desc: Handles errors where undefined behaviour
 *         calls for segfauls or other singals
 * @param signal_id -> The number assign to the signal
 **/
static void _signal_handler(const int signal_id) {
    _string *id = _new_string("");
    _string_add_int(id, signal_id);
    char *sig_description;

    /* Find the correct signal description */
    switch(signal_id) {
        /* SIGINT */ case 2: sig_description = _vector_get(_cspec->signals_vector, 0); break;
        /* SIGILL */ case 4: sig_description = _vector_get(_cspec->signals_vector, 1); break;
        /* SIGABRT */ case 6: sig_description = _vector_get(_cspec->signals_vector, 2); break;
        /* SIGFPE */ case 8: sig_description = _vector_get(_cspec->signals_vector, 3); break;
        /* SIGSEGV */ case 11: sig_description = _vector_get(_cspec->signals_vector, 4); break;
        /* SIGTERM */ case 15: sig_description = _vector_get(_cspec->signals_vector, 5); break;
    }

    /* Craft a response for signal errors */
    _string *sig = _new_string("");
    _string_add_str(sig, _string_get(_cspec->display_tab));
    _string_add_str(sig, _string_get(_cspec->RED));
    _string_add_str(sig, "✗");
    _string_add_str(sig, _string_get(_cspec->WHITE));
    _string_add_str(sig, " it ");
    _string_add_str(sig, _string_get(_cspec->name_of_tested_proc));
    _string_add_str(sig, ":\n");
    _string_add_str(sig, _string_get(_cspec->display_tab));
    _string_add_str(sig, "    ");
    _string_add_str(sig, _cspec->current_file);
    _string_add_str(sig, ":");
    _string_add_int(sig, _cspec->current_line);
    _string_add_str(sig, "\n");
    _string_add_str(sig, _string_get(_cspec->display_tab));
    _string_add_str(sig, "        |> ");
    _string_add_str(sig, "signal: ");
    _string_add_str(sig, _string_get(_cspec->RED));
    _string_add_str(sig, sig_description);
    _string_add_str(sig, _string_get(_cspec->WHITE));
    _string_add_str(sig, " got caught on \n");
    _string_add_str(sig, _string_get(_cspec->display_tab));
    _string_add_str(sig, "          -> ");
    _string_add_str(sig, _string_get(_cspec->RED));
    _string_add_str(sig, "`");

    /* This only works for `assert_that` blocks */
    _string_add_str(sig, _cspec->current_assert);
    /*------------------------------------------*/

    _string_add_str(sig, "`");
    _string_add_str(sig, _string_get(_cspec->RESET));
    printf("%s\n\n", _string_get(sig));
    // signal(signal_id, _signal_handler);
    signal(signal_id, SIG_DFL);
    // longjmp(&_cspec->escape, 1);
}

/**
 * @func: _setup_test_data
 * @desc: Allocates memory for vectors to save test results in
 **/
static void _setup_test_data(void) {
    /* Printf a neat intro */
    printf("\033[38;5;95m/######## ########/\n");
    printf("\033[38;5;95m/##### \033[38;5;89mc\033[38;5;90mS\033[38;5;91mp\033[38;5;92me\033[38;5;93mc\033[0m \033[38;5;95m#####/\n");
    printf("/######## ########/\033[0m\n");
    
    /* Craft the global variable struct */
    _cspec = malloc(sizeof(_cspec_data_struct) + 1);

    _cspec->number_of_tests = 0;
    _cspec->number_of_passing_tests = 0;
    _cspec->number_of_failing_tests = 0;
    _cspec->number_of_skipped_tests = 0;
    _cspec->total_time_taken_for_tests = 0;
    _cspec->status_of_test = _PASSING;
    // jmp_buf _cspec->escape;

    _cspec->test_result_message = NULL;
    _cspec->name_of_tested_proc = NULL;
    _cspec->name_of_describe = NULL;
    _cspec->name_of_context = NULL;

    _cspec->current_assert = NULL;
    _cspec->current_file = NULL;
    _cspec->current_line = NULL;
    _cspec->current_actual = NULL;
    _cspec->current_expected = NULL;
    _cspec->position_in_file = NULL;
    _cspec->assert_result = NULL;
    _cspec->type_of_tests = NULL;
    _cspec->type_of_export_tests = NULL;

    /** Before and after procs **/
    _cspec->before_func = NULL;
    _cspec->after_func = NULL;

    _cspec->display_tab = NULL;
    _cspec->signals_vector = _new_vector();
    _cspec->inner_nest = -1;
    _cspec->outer_nest = 0;
    _cspec->in_context_block = false;

    /** Colors **/
    _cspec->GREEN = _new_string("\033[38;5;78m");
    _cspec->RED = _new_string("\033[1;31m");
    _cspec->YELLOW = _new_string("\033[38;5;11m");
    _cspec->PURPLE = _new_string("\033[38;5;207m");
    _cspec->CYAN = _new_string("\033[1;36m");
    _cspec->GRAY = _new_string("\033[38;5;244m");
    _cspec->WHITE = _new_string("\033[38;5;254m");
    _cspec->RESET = _new_string("\033[0m");
    _cspec->BACK_PURPLE = _new_string("\033[48;5;89m");

    _cspec->list_of_modules = _new_vector();
    _cspec->list_of_describes = _new_vector();
    _cspec->list_of_contexts = _new_vector();
    _cspec->list_of_its = _new_vector();
    _cspec->list_of_asserts = _new_vector();

    /* Signals that can be processed */
    _vector_add(_cspec->signals_vector, "`SIGINT | Interrupt`");
    _vector_add(_cspec->signals_vector, "`SIGILL | Illegal instruction`");
    _vector_add(_cspec->signals_vector, "`SIGABRT | Abort`");
    _vector_add(_cspec->signals_vector, "`SIGFPE | Floating - Point arithmetic exception`");
    _vector_add(_cspec->signals_vector, "`SIGSEGV | Segmentation Violation`");
    _vector_add(_cspec->signals_vector, "`SIGTERM | Termination`");

    /* Setup the signals */
    signal(SIGINT, _signal_handler);
    signal(SIGILL, _signal_handler);
    signal(SIGABRT, _signal_handler);
    signal(SIGFPE, _signal_handler);
    signal(SIGSEGV, _signal_handler);
    signal(SIGTERM, _signal_handler);
}


/**
 * @macro: _to_string_int_write
 * @desc: Writes actual and expected values
 * @param actual -> The value passed by the user
 * @param expected -> The value `actual` is tested against
 **/
#define _to_string_int_write(actual, expected) _BLOCK( \
    _cspec->current_actual = _new_string(""); \
    _cspec->current_expected = _new_string(""); \
    _string_add_int(_cspec->current_actual, actual); \
    _string_add_int(_cspec->current_expected, expected); \
)

/**
 * @func: _int_comparison
 * @desc: A function that compares integers for assertions
 * @param actual -> The value passed by the user
 * @param expected -> The value `actual` is tested against
 * @return a boolean
 **/
static bool _int_comparison(int actual, int expected) {
    return actual == expected;
}

/**
 * @func: _call_assert_that_int
 * @desc: Assert that the expected integer is equal to the result
 * @param actual -> The value passed by the user
 * @param expected -> The value `actual` is tested against
 **/
define_assert(_call_assert_that_int, int, _to_string_int_write, !_int_comparison)

/**
 * @func: _call_nassert_that_int
 * @desc: Assert that the expected integer is different than the result
 * @param actual -> The value passed by the user
 * @param expected -> The value `actual` is tested against
 **/
define_assert(_call_nassert_that_int, int, _to_string_int_write, _int_comparison)

/**
 * @macro: assert_that_int
 * @desc: Assert that the expected integer is equal to the result
 * @param actual -> The actual value
 * @param expected -> The expected int
 **/
#define assert_that_int(inner) _BLOCK( \
    _cspec->current_file = __FILE__; \
    _cspec->current_line = __LINE__; \
    _call_assert_that_int(inner); \
)

/**
 * @macro: nassert_that_int
 * @desc: Assert that the expected integer is different than the result
 * @param actual -> The actual value
 * @param expected -> The expected int
 **/
#define nassert_that_int(inner) _BLOCK( \
    _cspec->current_file = __FILE__; \
    _cspec->current_line = __LINE__; \
    _call_nassert_that_int(inner); \
)


/**
 * @macro: _to_string_double_write
 * @desc: Writes actual and expected values
 * @param actual -> The value passed by the user
 * @param expected -> The value `actual` is tested against
 **/
#define _to_string_double_write(actual, expected) _BLOCK( \
    _cspec->current_actual = _new_string(""); \
    _cspec->current_expected = _new_string(""); \
    _string_add_double_precision(_cspec->current_actual, actual); \
    _string_add_double_precision(_cspec->current_expected, expected); \
)

/**
 * @func: _double_comparison
 * @desc: A function that compares doubles for assertions
 * @param actual -> The value passed by the user
 * @param expected -> The value `actual` is tested against
 * @return a boolean
 **/
static bool _double_comparison(double actual, double expected) {
    /* Calculate the margin to which the difference
        is too big so the test fails */
    return _fabs(actual - expected) < 1E-12;
}

/**
 * @func: _call_assert_that_double
 * @desc: Assert that the expected double is different than the result
 * @param actual -> The value passed by the user
 * @param expected -> The value `actual` is tested against
 **/
define_assert(_call_assert_that_double, double, _to_string_double_write, !_double_comparison)

/**
 * @func: _call_nassert_that_double
 * @desc: Assert that the expected double is different than the result
 * @param actual -> The value passed by the user
 * @param expected -> The value `actual` is tested against
 **/
define_assert(_call_nassert_that_double, double, _to_string_double_write, _double_comparison)

/**
 * @macro: assert_that_double
 * @desc: Assert that the expected double is different than the result
 * @param actual -> The actual value
 * @param expected -> The expected double
 **/
#define assert_that_double(inner) _BLOCK( \
    _cspec->current_file = __FILE__; \
    _cspec->current_line = __LINE__; \
    _call_assert_that_double(inner); \
)

/**
 * @macro: nassert_that_double
 * @desc: Assert that the expected double is different than the result
 * @param actual -> The actual value
 * @param expected -> The expected double
 **/
#define nassert_that_double(inner) _BLOCK( \
    _cspec->current_file = __FILE__; \
    _cspec->current_line = __LINE__; \
    _call_nassert_that_double(inner); \
)


/**
 * @macro: _to_string_charptr_write
 * @desc: Writes actual and expected values
 * @param actual -> The value passed by the user
 * @param expected -> The value `actual` is tested against
 **/
#define _to_string_charptr_write(actual, expected) _BLOCK( \
    _cspec->current_actual = _new_string(actual); \
    _cspec->current_expected = _new_string(expected); \
)

/**
 * @func: _charptr_comparison
 * @desc: A function that compares char pointers for assertions
 * @param actual -> The value passed by the user
 * @param expected -> The value `actual` is tested against
 * @return a boolean
 **/
static bool _charptr_comparison(char *actual, char *expected) {
    return __streql(expected, actual);
}

/**
 * @func: _call_assert_that_charptr
 * @desc: Assert that the expected string is equal to the result
 * @param actual -> The value passed by the user
 * @param expected -> The value `actual` is tested against
 **/
define_assert(_call_assert_that_charptr, char*, _to_string_charptr_write, !_charptr_comparison)

/**
 * @func: _call_nassert_that_charptr
 * @desc: Assert that the expected string is different than the result
 * @param actual -> The value passed by the user
 * @param expected -> The value `actual` is tested against
 **/
define_assert(_call_nassert_that_charptr, char*, _to_string_charptr_write, _charptr_comparison)

/**
 * @macro: assert_that_charptr
 * @desc: Assert that the expected string is equal to the result
 * @param actual -> The actual value
 * @param expected -> The expected string
 **/
#define assert_that_charptr(inner) _BLOCK( \
    _cspec->current_file = __FILE__; \
    _cspec->current_line = __LINE__; \
    _call_assert_that_charptr(inner); \
)

/**
 * @macro: nassert_that_charptr
 * @desc: Assert that the expected string is different than the result
 * @param actual -> The actual value
 * @param expected -> The expected string
 **/
#define nassert_that_charptr(inner) _BLOCK( \
    _cspec->current_file = __FILE__; \
    _cspec->current_line = __LINE__; \
    _call_nassert_that_charptr(inner); \
)


/**
 * @func: _to_string_int_array_write
 * @desc: Writes actual and expected values
 * @param actual -> The value passed by the user
 * @param expected -> The value `actual` is tested against
 * @param len -> The length of the arrays
 **/
static void _to_string_int_array_write(int *actual, int *expected, size_t len) {
    _cspec->current_actual = _new_string("[");
    _cspec->current_expected = _new_string("[");

    for(int i = 0; i < len - 1; i++) {
        _string_add_int(_cspec->current_actual, actual[i]);
        _string_add_str(_cspec->current_actual, ", ");

        _string_add_int(_cspec->current_expected, expected[i]);
        _string_add_str(_cspec->current_expected, ", ");
    }
    _string_add_int(_cspec->current_actual, actual[len - 1]);
    _string_add_str(_cspec->current_actual, "]");

    _string_add_int(_cspec->current_expected, expected[len - 1]);
    _string_add_str(_cspec->current_expected, "]");
}

/**
 * @func: _int_array_comparison
 * @desc: Compares two int arrays for equality in elements
 * @param actual -> The value passed by the user
 * @param expected -> The value `actual` is tested against
 * @param len -> The length of the arrays
 * @return a boolean
 **/
static bool _int_array_comparison(int *actual, int *expected, size_t len) {
    /* Check for members */
    for(int i = 0; i < len; i++) {
        if(actual[i] != expected[i]) return false;
    }

    /* All elements are equal */
    return true;
}

/**
 * @func: _call_assert_that_int_array
 * @desc: Assertion of two int arrays
 **/
define_assert_array(_call_assert_that_int_array, int*, _to_string_int_array_write, !_int_array_comparison, len)

/**
 * @func: _call_nassert_that_int_array
 * @desc: Negative assertion of two int arrays
 **/
define_assert_array(_call_nassert_that_int_array, int*, _to_string_int_array_write, _int_array_comparison, len)

/**
 * @macro: assert_that_int_array
 * @desc: Assert that the expected int array equals to the the result
 * @param actual -> The actual value
 * @param expected -> The expected value
 **/
#define assert_that_int_array(inner) _BLOCK( \
    _cspec->current_file = __FILE__; \
    _cspec->current_line = __LINE__; \
    _call_assert_that_int_array(inner); \
)

/**
 * @macro: nassert_that_int_array
 * @desc: Assert that the expected int array differs from the result
 * @param actual -> The actual value
 * @param expected -> The expected value
 **/
#define nassert_that_int_array(inner) _BLOCK( \
    _cspec->current_file = __FILE__; \
    _cspec->current_line = __LINE__; \
    _call_nassert_that_int_array(inner); \
)


/**
 * @func: _to_string_double_array_write
 * @desc: Writes the actual and expected values
 * @param actual -> The value passed by the user
 * @param expected -> The value `actual` is tested against
 * @param len -> The length of the arrays
 **/
static void _to_string_double_array_write(double *actual, double *expected, size_t len) {
    _cspec->current_actual = _new_string("[");
    _cspec->current_expected = _new_string("[");

    for(int i = 0; i < len - 1; i++) {
        _string_add_double_precision(_cspec->current_actual, actual[i]);
        _string_add_str(_cspec->current_actual, ", ");

        _string_add_double_precision(_cspec->current_expected, expected[i]);
        _string_add_str(_cspec->current_expected, ", ");
    }
    _string_add_double_precision(_cspec->current_actual, actual[len - 1]);
    _string_add_str(_cspec->current_actual, "]");

    _string_add_double_precision(_cspec->current_expected, expected[len - 1]);
    _string_add_str(_cspec->current_expected, "]");
}

/**
 * @func: _double_array_comparison
 * @desc: Conpares two double arrays for equality in elements
 * @param actual -> The value passed by the user
 * @param expected -> The value `actual` is tested against
 * @param len -> The length of the arrays
 * @return a boolean
 **/
static bool _double_array_comparison(double *actual, double *expected, size_t len) {
    /* Check for members */
    for(int i = 0; i < len; i++) {
        if(_fabs(actual[i] - expected[i]) > 1E-12) return false;
    }

    /* All elements are equal */
    return true;
}

/**
 * @func: _call_assert_that_double_array
 * @desc: Assertion of two int arrays
 **/
define_assert_array(
    _call_assert_that_double_array,
    double*,
    _to_string_double_array_write,
    !_double_array_comparison,
    len
)

/**
 * @func: _call_nassert_that_double_array
 * @desc: Negative assertion of two double arrays
 **/
define_assert_array(
    _call_nassert_that_double_array,
    double*,
    _to_string_double_array_write,
    _double_array_comparison,
    len
)

/**
 * @macro: assert_that_double_array
 * @desc: Assert that the expected double array equals to the result
 * @param actual -> The actual value
 * @param expected -> The expected value
 **/
#define assert_that_double_array(inner) _BLOCK( \
    _cspec->current_file = __FILE__; \
    _cspec->current_line = __LINE__; \
    _call_assert_that_double_array(inner); \
)

/**
 * @macro: nassert_that_double_array
 * @desc: Assert that the expected double array differs from the result
 * @param actual -> The actual value
 * @param expected -> The expected value
 **/
#define nassert_that_double_array(inner) _BLOCK( \
    _cspec->current_file = __FILE__; \
    _cspec->current_line = __LINE__; \
    _call_nassert_that_double_array(inner); \
)


/**
 * @func: _to_string_charptr_array_write
 * @desc: Writes actual and expected values
 * @param actual -> The value passed by the user
 * @param expected -> The value `actual` is tested against
 * @param len -> The length of the arrays
 **/
static void _to_string_charptr_array_write(char **actual, char **expected, size_t len) {
    _cspec->current_actual = _new_string("[");
    _cspec->current_expected = _new_string("[");

    for(int i = 0; i < len - 1; i++) {
        _string_add_str(_cspec->current_actual, actual[i]);
        _string_add_str(_cspec->current_actual, ", ");

        _string_add_str(_cspec->current_expected, expected[i]);
        _string_add_str(_cspec->current_expected, ", ");
    }
    _string_add_str(_cspec->current_actual, actual[len - 1]);
    _string_add_str(_cspec->current_actual, "]");

    _string_add_str(_cspec->current_expected, expected[len - 1]);
    _string_add_str(_cspec->current_expected, "]");
}

/**
 * @func: _charptr_array_comparison
 * @desc: Compares two string arrays for equality in elements
 * @param actual -> The value passed by the user
 * @param expected -> The value `actual` is tested against
 * @param len -> The length of the arrays
 * @return a boolean
 **/
static bool _charptr_array_comparison(char **actual, char **expected, size_t len) {
    /* Check for members */
    for(int i = 0; i < len; i++) {
        if(!__streql(actual[i], expected[i])) return false;
    }

    /* All elements are equal */
    return true;
}

/**
 * @func: _call_assert_that_charptr_array
 * @desc: Assertion of two int arrays
 **/
define_assert_array(
    _call_assert_that_charptr_array,
    double*,
    _to_string_charptr_array_write,
    !_charptr_array_comparison,
    len
)

/**
 * @func: _call_nassert_that_charptr_array
 * @desc: Negative assertion of two double arrays
 **/
define_assert_array(
    _call_nassert_that_charptr_array,
    double*,
    _to_string_charptr_array_write,
    _charptr_array_comparison,
    len
)

/**
 * @macro: assert_that_charptr_array
 * @desc: Assert that the expected charptr array equals to the result
 * @param actual -> The actual value
 * @param expected -> The expected value
 **/
#define assert_that_charptr_array(inner) _BLOCK( \
    _cspec->current_file = __FILE__; \
    _cspec->current_line = __LINE__; \
    _call_assert_that_charptr_array(inner); \
)

/**
 * @macro: nassert_that_charptr_array
 * @desc: Assert that the expected charptr array differs from the result
 * @param actual -> The actual value
 * @param expected -> The expected value
 **/
#define nassert_that_charptr_array(inner) _BLOCK( \
    _cspec->current_file = __FILE__; \
    _cspec->current_line = __LINE__; \
    _call_nassert_that_charptr_array(inner); \
)


#endif
