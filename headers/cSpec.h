#ifndef __CSPEC_H_
#define __CSPEC_H_

#if defined(_WIN32)
    #include <Windows.h>
    #if defined(_MSC_VER) && _MSC_VER < 1900
        #define snprintf _snprintf
        #define __func__ __FUNCTION__
    #endif
#elif defined(__unix__) \
|| defined(__unix) \
|| defined(unix) \
|| defined(__APPLE__) \
&& defined (__MACH__)
    #if !defined(_POSIX_C_SOURCE) \
    || _POSIX_C_SOURCE < 200112L
        #undef _POSIX_C_SOURCE
        #define _POSIX_C_SOURCE 200112L
    #endif
    
    #include <unistd.h>
    #include <time.h>
    #include <sys/time.h>
    #include <sys/resource.h>
    #include <sys/times.h>
    #include <string.h>

    #define HAVE_POSIX_TIMER
    #ifdef CLOCK_MONOTONIC
        #define CLOCKID CLOCK_MONOTONIC
    #endif

    #if defined(__MACH__) && defined(__APPLE__)
        #define HAVE_MACH_TIMER
        #include <mach/mach.h>
        #include <mach/mach_time.h>
    #endif

    #if __GNUC__ >= 5 && !defined(__STDC_VERSION)
        #define __func__ __extension__ __FUNCTION__
    #endif
#else
    #error "Unknown OS"
#endif

#include "../../cSuite.h"

/**
 * @param PASSING -> Set for passing tests
 * @param FAILING -> Set for failing tests
 **/
#define PASSING true
#define FAILING false

#define MAXIMUM_LENGTH_OF_RESULT_MESSAGE 1024
#define FLOAT_COMPARISON_ACCURACY 1E-12

#define fabs(value) ((value < 0) ? (-value) : (value))

static int number_of_tests = 0;
static int number_of_failing_tests = 0;
static int number_of_asserts = 0;
static int status_of_test = FAILING;

/**
 * @param total_time_taken_for_tests -> The timer used to measure time to run tests
 * @param test_result_message -> The outcome returned from the test
 **/
static double total_time_taken_for_tests = 0;
hashmap *describes_hashmap;
stack *describes_stack;

string *test_result_message;
string *name_of_tested_proc;
string *name_of_describe;
string *display_tab;

/**
 * @macro: before
 * @desc: Expands to a setup proc that gets executed before the tests
 * @param setup_proc -> The proc to run
 **/
#define before(setup_proc) BLOCK( \
    setup_proc; \
)

/**
 * @marco: after
 * @desc: Expands to a teardown proc that gets executed after the tests
 * @param teardown_proc -> The proc to run
 **/
#define after(teardown_proc) BLOCK( \
    teardown_proc; \
)

/**
 * @marco: describe
 * @desc: Expands to a block execution where tests are encompassed
 * @param object_name -> The name of the unit to describe
 * @param proc -> The proc to extend to
 **/
#define describe(object_name, proc) BLOCK( \
    name_of_describe = new_string(object_name); \
    \
    vector *vector_of_tests = new_vector(); \
    vector_add(vector_of_tests, new_vector()); \
    vector_add(vector_of_tests, new_vector()); \
    \
    hashmap_add(describes_hashmap, string_get(name_of_describe), vector_of_tests); \
    /* string_add_str(display_tab, "    "); */ \
    stack_push(describes_stack, name_of_describe); \
    proc; \
    stack_pop(describes_stack); \
    name_of_describe = stack_peek(describes_stack); \
    /* string_shorten(display_tab, string_length(display_tab) - 4); */ \
)

/**
 * @macro: it
 * @desc: Expands to a test run and saves all data gathered
 * @param proc_name -> The name of test to run
 * @param proc -> The actual test code
 **/
#define it(proc_name, proc) BLOCK( \
    /* printf("IT BLOCK INSIDE %s\n", string_get(name_of_describe)); */ \
	status_of_test = FAILING; \
    name_of_tested_proc = new_string(proc_name); \
    \
    double start_test_timer = get_timer(); \
    proc; \
    double end_test_timer = get_timer(); \
    \
	number_of_tests++; \
	if(!(status_of_test)) { \
		number_of_failing_tests++; \
        string *new_test_message = string_dup(test_result_message); \
        vector_add(vector_get(hashmap_get(describes_hashmap, \
        string_get(name_of_describe)), 1), new_test_message); \
        \
	} \
    else { \
        string *new_test_message = string_dup(test_result_message); \
        vector_add(vector_get(hashmap_get(describes_hashmap, \
        string_get(name_of_describe)), 0), new_test_message); \
    } \
    total_time_taken_for_tests += end_test_timer - start_test_timer; \
)

/**
 * @func: print_string
 * @desc: Handle iterating through vector elements for printing test messages
 * @param element -> The vector element to print
 **/
static void print_string(string *element) {
    printf("%s\n", string_get(element));
}

/**
 * @func: print_passing_tests
 * @desc: Print all elements of the passing tests vector
 * @param vector_of_tests -> The vector of vectors of tests
 **/
#define print_passing_tests(vector_of_tests) BLOCK( \
    vector_map(vector_get(vector_of_tests, 0), print_string); \
)

/**
 * @func: print_failing_tests
 * @desc: Print all vector_of_testss of the failing tests vector
 * @param vector_of_tests -> The vector of vectors of tests
 **/
#define print_failing_tests(vector_of_tests) BLOCK( \
    vector_map(vector_get(vector_of_tests, 1), print_string); \
)

/**
 * @macro: report_test_results
 * @desc: Prints all elements of the desired test vectors
 * @param vector -> Either passing failing or both
 **/
#define report_test_results(vector) BLOCK( \
    for(size_t i = 0; i < describes_hashmap->alloced; i++) { \
        if(describes_hashmap->data[i].in_use != 0) { \
            printf("\033[38;5;207m%s\033[0m\n", \
            (&describes_hashmap->data[i])->key); \
            \
            if(vector == "passing") { \
                print_passing_tests((&describes_hashmap->data[i])->data); \
                printf("\n"); \
            } \
            else if(vector == "failing") { \
                print_failing_tests((&describes_hashmap->data[i])->data); \
            } \
            else if(vector == "both") { \
                print_passing_tests((&describes_hashmap->data[i])->data); \
                printf("\n"); \
                print_failing_tests((&describes_hashmap->data[i])->data); \
            } \
        } \
    } \
)

/**
 * @macro: report_time_taken_for_tests
 * @desc: Report the number of tests, assertions and time taken while testing
 **/
#define report_time_taken_for_tests() BLOCK( \
	printf("\n→ %d tests\n\033[38;5;11m● %d assertions\033[0m\n\033[38;5;78m✓ %d passing\033[0m\n\033[1;31m✗ %d failing\033[0m\n", \
    number_of_tests, number_of_asserts, \
    number_of_tests - number_of_failing_tests, \
    number_of_failing_tests); \
    printf("\033[1;36m★ Finished in %.4f ms\033[0m\n", \
    total_time_taken_for_tests / 1000000.0); \
    \
)

/** @param exit_code -> Used in return **/
#define exit_code number_of_failing_tests

/**
 * @macro: check
 * @desc: Asserts if a test proc is true or not
 * @param test -> The test proc to run
 **/
#define check(test) BLOCK( \
	number_of_asserts++; \
    test_result_message = new_string(""); \
	if(!(test)) { \
        status_of_test = FAILING; \
        string_add_str(test_result_message, string_get(display_tab)); \
        string_add_str(test_result_message, "\033[1;31m✗\033[0m it "); \
        string_add_str(test_result_message, string_get(name_of_tested_proc)); \
        string_add_str(test_result_message, ":\n"); \
        string_add_str(test_result_message, string_get(display_tab)); \
        string_add_str(test_result_message, "    "); \
        string_add_str(test_result_message, __FILE__); \
        string_add_str(test_result_message, ":"); \
        string_add_int(test_result_message, __LINE__); \
        string_add_str(test_result_message, ":\n"); \
        string_add_str(test_result_message, string_get(display_tab)); \
        string_add_str(test_result_message, "        |> "); \
        string_add_str(test_result_message, #test); \
        string_add_str(test_result_message, "\n"); \
	} \
    else { \
        status_of_test = PASSING; \
        string_add_str(test_result_message, string_get(display_tab)); \
        string_add_str(test_result_message, "\033[38;5;78m✓\033[0m it "); \
        string_add_str(test_result_message, string_get(name_of_tested_proc)); \
    } \
)

/**
 * @macro: fail
 * @desc: Asserts a failing test and provides a message
 * @param message -> The message to output
 **/
#define fail(message) BLOCK( \
	number_of_asserts++; \
    test_result_message = new_string(""); \
    status_of_test = FAILING; \
    string_add_str(test_result_message, string_get(display_tab)); \
    string_add_str(test_result_message, "\033[1;31m✗\033[0m it "); \
    string_add_str(test_result_message, string_get(name_of_tested_proc)); \
    string_add_str(test_result_message, ":\n"); \
    string_add_str(test_result_message, string_get(display_tab)); \
    string_add_str(test_result_message, "    "); \
    string_add_str(test_result_message, __FILE__); \
    string_add_str(test_result_message, ":"); \
    string_add_int(test_result_message, __LINE__); \
    string_add_str(test_result_message, ":\n"); \
    string_add_str(test_result_message, string_get(display_tab)); \
    string_add_str(test_result_message, "        |> "); \
    string_add_str(test_result_message, #message); \
    string_add_str(test_result_message, "\n"); \
)

/**
 * @macro: assert
 * @desc: Assert a test to be equal to a provided result
 * @param test -> The test proc to check
 * @param message -> A description of the expected result
 **/
#define assert(test, message) BLOCK( \
	number_of_asserts++; \
    test_result_message = new_string(""); \
	if(!(test)) { \
        status_of_test = FAILING; \
        string_add_str(test_result_message, string_get(display_tab)); \
        string_add_str(test_result_message, "\033[1;31m✗\033[0m it "); \
        string_add_str(test_result_message, string_get(name_of_tested_proc)); \
        string_add_str(test_result_message, ":\n"); \
        string_add_str(test_result_message, string_get(display_tab)); \
        string_add_str(test_result_message, "    "); \
        string_add_str(test_result_message, __FILE__); \
        string_add_str(test_result_message, ":"); \
        string_add_int(test_result_message, __LINE__); \
        string_add_str(test_result_message, ":\n"); \
        string_add_str(test_result_message, string_get(display_tab)); \
        string_add_str(test_result_message, "        |> "); \
        string_add_str(test_result_message, message); \
        string_add_str(test_result_message, "\n"); \
	} \
    else { \
        status_of_test = PASSING; \
        string_add_str(test_result_message, string_get(display_tab)); \
        string_add_str(test_result_message, "\033[38;5;78m✓\033[0m it "); \
        string_add_str(test_result_message, string_get(name_of_tested_proc)); \
    } \
)

/**
 * @macro: assert_int_equality
 * @desc: Assert that the expected integer is equal to the result
 * @param expected -> The expected int
 * @param actual -> The actual value
 **/
#define assert_int_equality(expected, actual) BLOCK( \
	number_of_asserts++; \
    test_result_message = new_string(""); \
	int temp_expected = (expected); \
	int temp_actual_value = (actual); \
	if(temp_expected != temp_actual_value) { \
		status_of_test = FAILING; \
        string_add_str(test_result_message, string_get(display_tab)); \
        string_add_str(test_result_message, "\033[1;31m✗\033[0m it "); \
        string_add_str(test_result_message, string_get(name_of_tested_proc)); \
        string_add_str(test_result_message, ":\n"); \
        string_add_str(test_result_message, string_get(display_tab)); \
        string_add_str(test_result_message, "    "); \
        string_add_str(test_result_message, __FILE__); \
        string_add_str(test_result_message, ":"); \
        string_add_int(test_result_message, __LINE__); \
        string_add_str(test_result_message, ":\n"); \
        string_add_str(test_result_message, string_get(display_tab)); \
        string_add_str(test_result_message, "        |> "); \
        string_add_str(test_result_message, "\""); \
        string_add_int(test_result_message, temp_expected); \
        string_add_str(test_result_message, "\""); \
        string_add_str(test_result_message, " expected but got \""); \
        string_add_int(test_result_message, temp_actual_value); \
        string_add_str(test_result_message, "\"\n"); \
	} \
    else { \
        status_of_test = PASSING; \
        string_add_str(test_result_message, string_get(display_tab)); \
        string_add_str(test_result_message, "\033[38;5;78m✓\033[0m it "); \
        string_add_str(test_result_message, string_get(name_of_tested_proc)); \
    } \
)

/**
 * @macro: assert_double_equality
 * @desc: Assert that the expected double is equal to the result
 * @param expected -> The expected double
 * @param actual -> The actual value
 **/
#define assert_double_equality(expected, actual) BLOCK( \
	number_of_asserts++; \
    test_result_message = new_string(""); \
	double temp_expected = (expected); \
	double temp_actual_value = (actual); \
	if(fabs(temp_expected - temp_actual_value) > FLOAT_COMPARISON_ACCURACY) { \
		status_of_test = FAILING; \
        string_add_str(test_result_message, string_get(display_tab)); \
        string_add_str(test_result_message, "\033[1;31m✗\033[0m it "); \
        string_add_str(test_result_message, string_get(name_of_tested_proc)); \
        string_add_str(test_result_message, ":\n"); \
        string_add_str(test_result_message, string_get(display_tab)); \
        string_add_str(test_result_message, "    "); \
        string_add_str(test_result_message, __FILE__); \
        string_add_str(test_result_message, ":"); \
        string_add_int(test_result_message, __LINE__); \
        string_add_str(test_result_message, ":\n"); \
        string_add_str(test_result_message, string_get(display_tab)); \
        string_add_str(test_result_message, "        |> "); \
        string_add_str(test_result_message, "\""); \
        string_add_double_precision(test_result_message, temp_expected); \
        string_add_str(test_result_message, "\""); \
        string_add_str(test_result_message, " expected but got \""); \
        string_add_double_precision(test_result_message, temp_actual_value); \
        string_add_str(test_result_message, "\"\n"); \
	} \
    else { \
        status_of_test = PASSING; \
        string_add_str(test_result_message, string_get(display_tab)); \
        string_add_str(test_result_message, "\033[38;5;78m✓\033[0m it "); \
        string_add_str(test_result_message, string_get(name_of_tested_proc)); \
    } \
)

/**
 * @macro: assert_string_equality
 * @desc: Assert that the expected string is equal to the result
 * @param expected -> The expected string
 * @param actual -> The actual value
 **/
#define assert_string_equality(expected, actual) BLOCK( \
	const char *temp_expected = expected; \
	const char *temp_actual_value = actual; \
	number_of_asserts++; \
    test_result_message = new_string(""); \
	if(!temp_expected) temp_expected = "NULL"; \
	if(!temp_actual_value) temp_actual_value = "NULL"; \
	if(strcmp(temp_expected, temp_actual_value)) { \
		status_of_test = FAILING; \
        string_add_str(test_result_message, string_get(display_tab)); \
        string_add_str(test_result_message, "\033[1;31m✗\033[0m it "); \
        string_add_str(test_result_message, string_get(name_of_tested_proc)); \
        string_add_str(test_result_message, ":\n"); \
        string_add_str(test_result_message, string_get(display_tab)); \
        string_add_str(test_result_message, "    "); \
        string_add_str(test_result_message, __FILE__); \
        string_add_str(test_result_message, ":"); \
        string_add_int(test_result_message, __LINE__); \
        string_add_str(test_result_message, ":\n"); \
        string_add_str(test_result_message, string_get(display_tab)); \
        string_add_str(test_result_message, "        |> "); \
        string_add_str(test_result_message, "\""); \
        string_add_str(test_result_message, temp_expected); \
        string_add_str(test_result_message, "\""); \
        string_add_str(test_result_message, " expected but got \""); \
        string_add_str(test_result_message, temp_actual_value); \
        string_add_str(test_result_message, "\"\n"); \
	} \
    else { \
        status_of_test = PASSING; \
        string_add_str(test_result_message, string_get(display_tab)); \
        string_add_str(test_result_message, "\033[38;5;78m✓\033[0m it "); \
        string_add_str(test_result_message, string_get(name_of_tested_proc)); \
    } \
)

/**
 * @func: setup_test_data
 * @desc: Allocates memory for vectors to save test results in
 **/
static void setup_test_data() {
    describes_hashmap = new_hashmap();
    describes_stack = new_stack();
    display_tab = new_string("    ");
}

/**
 * @func: get_timer
 * @desc: A cross platform timer function for profiling value
 * @return The time in nanoseconds
 **/
static unsigned long long get_timer() {
    static unsigned long long is_init = 0;
    #if defined(__APPLE__)
        static mach_timebase_info_data_t info;
        if (0 == is_init)
        {
            mach_timebase_info(&info);
            is_init = 1;
        }
        unsigned long long now;
        now = mach_absolute_time();
        now *= info.numer;
        now /= info.denom;
        return now;
    #elif defined(__linux)
        static struct timespec linux_rate;
        if (0 == is_init)
        {
            clock_getres(CLOCKID, &linux_rate);
            is_init = 1;
        }
        unsigned long long now;
        struct timespec spec;
        clock_gettime(CLOCKID, &spec);
        now = spec.tv_sec * 1.0e9 + spec.tv_nsec;
        return now;
    #elif defined(_WIN32)
        static LARGE_INTEGER win_frequency;
        if (0 == is_init)
        {
            QueryPerformanceFrequency(&win_frequency);
            is_init = 1;
        }
        LARGE_INTEGER now;
        QueryPerformanceCounter(&now);
        return (unsigned long long)((1e9 * now.QuadPart) / win_frequency.QuadPart);
    #endif
}

#endif
