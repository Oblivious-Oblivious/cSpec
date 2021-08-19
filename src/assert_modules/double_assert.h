#ifndef __DOUBLE_ASSERT_H_
#define __DOUBLE_ASSERT_H_

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

#endif
