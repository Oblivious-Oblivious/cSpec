#ifndef __EXPRESSION_ASSERT_H
#define __EXPRESSION_ASSERT_H

/* #include "../functionality.h" */

/**
 * @macro: assert_that
 * @desc: Asserts that a proc returns true
 * @param test -> The test proc to run
 **/
#define assert_that(test) CSPEC_BLOCK( \
    cspec->current_file = __FILE__; \
    cspec->current_line = __LINE__; \
    cspec->position_in_file = cspec_string_new(""); \
    cspec->assert_result = cspec_string_new(""); \
    \
    /* Save the test token */ \
    cspec->current_assert = #test; \
    \
    /* Assert a custom block of code */ \
    if(!(test)) { \
        cspec->status_of_test = CSPEC_FAILING; \
        cspec_write_position_in_file(); \
        \
        /* Craft the string builder for terminal output */ \
        cspec_string_add_str(cspec->test_result_message, cspec_string_get(cspec->display_tab)); \
        cspec_string_add_str(cspec->test_result_message, cspec_string_get(cspec->WHITE)); \
        cspec_string_add_str(cspec->test_result_message, "    "); \
        cspec_string_add_str(cspec->test_result_message, cspec_string_get(cspec->position_in_file)); \
        cspec_string_add_str(cspec->test_result_message, "\n"); \
        cspec_string_add_str(cspec->test_result_message, cspec_string_get(cspec->display_tab)); \
        cspec_string_add_str(cspec->test_result_message, "        |> "); \
        cspec_string_add_str(cspec->test_result_message, cspec_string_get(cspec->RED)); \
        cspec_string_add_str(cspec->test_result_message, "`"); \
        cspec_string_add_str(cspec->test_result_message, #test); \
        cspec_string_add_str(cspec->test_result_message, "`"); \
        cspec_string_add_str(cspec->test_result_message, cspec_string_get(cspec->WHITE)); \
        cspec_string_add_str(cspec->test_result_message, " should be true\n"); \
        cspec_string_add_str(cspec->test_result_message, cspec_string_get(cspec->RESET)); \
/*************************************************************************/ \
        /* Craft the string builder for text output (miss the color codes) */ \
        cspec_string_add_str(cspec->assert_result, "|> `"); \
        cspec_string_add_str(cspec->assert_result, #test); \
        cspec_string_add_str(cspec->assert_result, "`"); \
        cspec_string_add_str(cspec->assert_result, " should be true"); \
/****************************************************************************/ \
        cspec_vector *list_of_strings = cspec_vector_new(); \
        cspec_vector_add(list_of_strings, cspec_string_get(cspec->position_in_file)); \
        cspec_vector_add(list_of_strings, cspec_string_get(cspec->assert_result)); \
/****************************************************************************/ \
        cspec_vector_add(cspec->list_of_asserts, list_of_strings); \
    } \
)

/**
 * @macro: nassert_that
 * @desc: Asserts that a proc returns false
 * @param test -> The test proc to run
 **/
#define nassert_that(test) CSPEC_BLOCK( \
    cspec->current_file = __FILE__; \
    cspec->current_line = __LINE__; \
    cspec->position_in_file = cspec_string_new(""); \
    cspec->assert_result = cspec_string_new(""); \
    \
    /* Save the test token */ \
    cspec->current_assert = #test; \
    \
	if((test)) { \
        cspec->status_of_test = CSPEC_FAILING; \
        cspec_write_position_in_file(); \
        \
        /* Craft the string builder for terminal output */ \
        cspec_string_add_str(cspec->test_result_message, cspec_string_get(cspec->display_tab)); \
        cspec_string_add_str(cspec->test_result_message, cspec_string_get(cspec->WHITE)); \
        cspec_string_add_str(cspec->test_result_message, "    "); \
        cspec_string_add_str(cspec->test_result_message, cspec_string_get(cspec->position_in_file)); \
        cspec_string_add_str(cspec->test_result_message, "\n"); \
        cspec_string_add_str(cspec->test_result_message, cspec_string_get(cspec->display_tab)); \
        cspec_string_add_str(cspec->test_result_message, "        |> "); \
        cspec_string_add_str(cspec->test_result_message, cspec_string_get(cspec->RED)); \
        cspec_string_add_str(cspec->test_result_message, "`"); \
        cspec_string_add_str(cspec->test_result_message, #test); \
        cspec_string_add_str(cspec->test_result_message, "`"); \
        cspec_string_add_str(cspec->test_result_message, cspec_string_get(cspec->WHITE)); \
        cspec_string_add_str(cspec->test_result_message, " should be false\n"); \
        cspec_string_add_str(cspec->test_result_message, cspec_string_get(cspec->RESET)); \
/*************************************************************************/ \
        /* Craft the string builder for text output (miss the color codes) */ \
        cspec_string_add_str(cspec->assert_result, "|> `"); \
        cspec_string_add_str(cspec->assert_result, #test); \
        cspec_string_add_str(cspec->assert_result, "`"); \
        cspec_string_add_str(cspec->assert_result, " should be false"); \
/****************************************************************************/ \
        cspec_vector *list_of_strings = cspec_vector_new(); \
        cspec_vector_add(list_of_strings, cspec_string_get(cspec->position_in_file)); \
        cspec_vector_add(list_of_strings, cspec_string_get(cspec->assert_result)); \
/****************************************************************************/ \
        cspec_vector_add(cspec->list_of_asserts, list_of_strings); \
    } \
)

#endif
