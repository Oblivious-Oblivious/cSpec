#ifndef __STRING_ARRAY_ASSERT_H_
#define __STRING_ARRAY_ASSERT_H_

/* #include "expression_assert.h" */

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

#endif
