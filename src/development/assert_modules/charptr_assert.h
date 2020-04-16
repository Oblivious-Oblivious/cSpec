#ifndef __CHARPTR_ASSERT_H_
#define __CHARPTR_ASSERT_H_

/**
 * @func: _call_assert_that_string
 * @desc: Assert that the expected string is equal to the result
 * @param actual -> The value passed by the user
 * @param expected -> The value `actual` is tested against
 **/
define_assert(_call_assert_that_string, char*, _string_add_str, !_string_comparison)

/**
 * @func: _call_nassert_that_string
 * @desc: Assert that the expected string is different than the result
 * @param actual -> The value passed by the user
 * @param expected -> The value `actual` is tested against
 **/
define_assert(_call_nassert_that_string, char*, _string_add_str, _string_comparison)

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