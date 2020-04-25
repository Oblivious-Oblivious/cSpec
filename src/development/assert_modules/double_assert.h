#ifndef __DOUBLE_ASSERT_H_
#define __DOUBLE_ASSERT_H_

#include "../interface.h"
#include "../functionality.h"


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

#endif