#ifndef __DOUBLE_ASSERT_H_
#define __DOUBLE_ASSERT_H_

#include "../interface.h"
#include "../functionality.h"


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