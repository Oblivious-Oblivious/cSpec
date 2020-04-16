#ifndef __INT_ASSERT_H_
#define __INT_ASSERT_H_

/**
 * @func: _call_assert_that_int
 * @desc: Assert that the expected integer is equal to the result
 * @param actual -> The value passed by the user
 * @param expected -> The value `actual` is tested against
 **/
define_assert(_call_assert_that_int, int, _string_add_int, !_int_comparison)

/**
 * @func: _call_nassert_that_int
 * @desc: Assert that the expected integer is different than the result
 * @param actual -> The value passed by the user
 * @param expected -> The value `actual` is tested against
 **/
define_assert(_call_nassert_that_int, int, _string_add_int, _int_comparison)

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