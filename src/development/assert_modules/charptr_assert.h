#ifndef __CHARPTR_ASSERT_H_
#define __CHARPTR_ASSERT_H_

#include "../interface.h"
#include "../functionality.h"


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
 * @func: _string_comparison
 * @desc: A function that compares char pointers for assertions
 * @param actual -> The value passed by the user
 * @param expected -> The value `actual` is tested against
 * @return a boolean
 **/
static bool _string_comparison(char *actual, char *expected) {
    return __streql(expected, actual);
}

/**
 * @func: _call_assert_that_string
 * @desc: Assert that the expected string is equal to the result
 * @param actual -> The value passed by the user
 * @param expected -> The value `actual` is tested against
 **/
define_assert(_call_assert_that_string, char*, _to_string_charptr_write, !_string_comparison)

/**
 * @func: _call_nassert_that_string
 * @desc: Assert that the expected string is different than the result
 * @param actual -> The value passed by the user
 * @param expected -> The value `actual` is tested against
 **/
define_assert(_call_nassert_that_string, char*, _to_string_charptr_write, _string_comparison)

/**
 * @macro: assert_that_string
 * @desc: Assert that the expected string is equal to the result
 * @param actual -> The actual value
 * @param expected -> The expected string
 **/
#define assert_that_string(inner) _BLOCK( \
    _cspec->current_file = __FILE__; \
    _cspec->current_line = __LINE__; \
    _call_assert_that_string(inner); \
)

/**
 * @macro: nassert_that_string
 * @desc: Assert that the expected string is different than the result
 * @param actual -> The actual value
 * @param expected -> The expected string
 **/
#define nassert_that_string(inner) _BLOCK( \
    _cspec->current_file = __FILE__; \
    _cspec->current_line = __LINE__; \
    _call_nassert_that_string(inner); \
)

#endif