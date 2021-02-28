#ifndef __INT_ASSERT_H_
#define __INT_ASSERT_H_

/* #include "expression_assert.h" */

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
