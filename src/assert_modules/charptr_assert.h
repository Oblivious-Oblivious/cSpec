#ifndef __CHARPTR_ASSERT_H_
#define __CHARPTR_ASSERT_H_

/* #include "expression_assert.h" */

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

#endif
