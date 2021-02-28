#ifndef __HELPERS_H_
#define __HELPERS_H_

/**
 * @param CSPEC_PASSING -> Set for passing tests
 * @param CSPEC_FAILING -> Set for failing tests
 * @param CSPEC_SKIPPED -> Set for skipeed tests
 **/
#define CSPEC_PASSING 1
#define CSPEC_FAILING 0
#define CSPEC_SKIPPED -1

/**
 * @macro: CSPEC_BLOCK
 * @desc: Expands to a do while loop that runs once
 *      Usefull for executing naked blocks
 * @param ... -> The block of code to run
 **/
#define CSPEC_BLOCK(...) do { \
    __VA_ARGS__ \
} while(0)

#define is ==
#define isnot !=
#define not !
#define and &&
#define or ||
#define equals ,
#define to
#define with
#define length ,

/**
 * @macro: cspec_compare_values
 * @desc: Runs the actual assertion between 2 values
 * @param actual -> The value passed by the user
 * @param expected -> The value `actual` is tested against
 * @param func -> The assert function according to data type
 **/
#define cspec_compare_values(actual, expected, func) CSPEC_BLOCK( \
    if(func(actual, expected)) { \
        cspec->status_of_test = CSPEC_FAILING; \
        cspec_write_position_in_file(); \
        cspec_write_assert_actual_expected(); \
    } \
)

/**
 * @macro: cspec_compare_array_values
 * @desc: Runs the actual assertion between 2 values
 * @param actual -> The value passed by the user
 * @param expected -> The value `actual` is tested against
 * @param func -> The assert function according to data type
 * @param len -> The length of the array to test
 **/
#define cspec_compare_array_values(actual, expected, func, len) CSPEC_BLOCK( \
    if(func(actual, expected, len)) { \
        cspec->status_of_test = CSPEC_FAILING; \
        cspec_write_position_in_file(); \
        cspec_write_assert_actual_expected(); \
    } \
)

/**
 * @macro: define_assert
 * @desc: Defines a compile time assertion for extended data types
 * @param name_of_assert -> The name of the new assertion
 * @param data_type_token -> The data type of the input variables
 * @param to_string_method -> Custom way to write data type as a string
 * @param comparison_method -> Custom way of comparing new data types for asserts
 **/
#define define_assert(name_of_assert, data_type_token, to_string_method, comparison_method) \
    static inline void name_of_assert(data_type_token actual, data_type_token expected) { \
        to_string_method(actual, expected); \
        cspec_compare_values(actual, expected, comparison_method); \
    }

#define define_assert_array(name_of_assert, data_type_token, to_string_method, comparison_method, len) \
    static inline void name_of_assert(data_type_token actual, data_type_token expected, size_t len) { \
        to_string_method(actual, expected, len); \
        cspec_compare_array_values(actual, expected, comparison_method, len); \
    }

#endif
