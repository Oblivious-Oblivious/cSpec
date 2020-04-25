#ifndef __DOUBLE_ARRAY_ASSERT_H_
#define __DOUBLE_ARRAY_ASSERT_H_

#include "../interface.h"
#include "../functionality.h"


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

#endif