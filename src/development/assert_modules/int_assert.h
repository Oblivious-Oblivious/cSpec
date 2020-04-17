#ifndef __INT_ASSERT_H_
#define __INT_ASSERT_H_

#include "../interface.h"
#include "../functionality.h"


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

#endif