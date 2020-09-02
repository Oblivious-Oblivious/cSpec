#ifndef __INTERFACE_H_
#define __INTERFACE_H_





/**
 * @param _PASSING -> Set for passing tests
 * @param _FAILING -> Set for failing tests
 * @param _SKIPPED -> Set for skipeed tests
 **/
#define _PASSING 1
#define _FAILING 0
#define _SKIPPED -1

/**
 * @macro: _BLOCK
 * @desc: Expands to a do while loop that runs once
 *      Usefull for executing naked blocks
 * @param ... -> The block of code to run
 **/
#define _BLOCK(...) do { \
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
 * @macro: spec
 * @desc: Call the main automation function
 **/
#define spec(...) \
    int main(int argc, char **argv) { \
        _BLOCK(__VA_ARGS__); \
    }

/**
 * @macro: spec_suite
 * @desc: A simple function definition for running test suites
 * @param ... -> The block of modules to run
 **/
#define spec_suite(...) \
    /* Define a function to run tests modules at */ \
    static void run_spec_suite(char *type_of_tests) { \
        _setup_test_data(); \
        _cspec->type_of_tests = _new_string(type_of_tests); \
        \
        \
        /* Check for valid test type */ \
        if(!__streql(type_of_tests, "passing") \
        && !__streql(type_of_tests, "failing") \
        && !__streql(type_of_tests, "skipped") \
        && !__streql(type_of_tests, "all")) { \
            printf("\n\033[1;31mInput a type of test to log passing|failing|skipped|all\033[0m\n\n"); \
            return; /* Exit the function */ \
        } \
        \
        \
        _BLOCK(__VA_ARGS__); \
        _report_time_taken_for_tests(); \
        \
        \
    }

/**
 * @macro: module
 * @desc: Expands to a function definition of the test suite
 * @param suite_name -> The name for the new module of tests
 * @param ... -> The block to define
 **/
#define module(suite_name, ...) \
    /* Define a static method for a module */ \
    static void suite_name(void) { \
        _cspec->name_of_module = _new_string(#suite_name); \
        _cspec->list_of_describes = _new_vector(); \
        \
        \
        /* Print module information */ \
        printf("\n%s%sModule `%s`%s\n", \
        _string_get(_cspec->BACK_PURPLE), \
        _string_get(_cspec->YELLOW), \
        _string_get(_cspec->name_of_module), \
        _string_get(_cspec->RESET)); \
        _cspec->display_tab = _new_string(""); \
        \
        \
        _BLOCK(__VA_ARGS__); /* Run describes */ \
        \
        \
        /* Construct a module and save for exporting */ \
        _vector *mod = _new_vector(); \
        _vector_add(mod, _cspec->list_of_describes); \
        _vector_add(mod, _cspec->name_of_module); \
        _vector_add(_cspec->list_of_modules, mod); \
        \
        /* mod([describes], name) */ \
    }

/**
 * @macro: before
 * @desc: Expands to a setup proc that gets executed before the tests
 * @param ... -> The proc to run
 **/
#define before(...) _BLOCK( \
    __VA_ARGS__ \
)

/**
 * @marco: after
 * @desc: Expands to a teardown proc that gets executed after the tests
 * @param ... -> The proc to run
 **/
#define after(...) _BLOCK( \
    __VA_ARGS__ \
)

/**
 * @macro: before_each
 * @desc: Sets the argument to a function to run before each it block
 * @param func -> The func to set
 **/
#define before_each(func) _BLOCK( \
    _cspec->before_func = func; \
)

/**
 * @marco: after_each
 * @desc: Sets the argument to a function to run after each it block
 * @param func -> The func to set
 **/
#define after_each(func) _BLOCK( \
    _cspec->after_func = func; \
)

/**
 * @macro: _describe
 * @desc: Defines a `describe` level block
 * @param __VA_ARGS__
 **/
#define _describe(...) _BLOCK( \
    if(_cspec->inner_nest == -1) _cspec->outer_nest = 0; \
    \
    /* On every describe block we nest inwards */ \
    _cspec->inner_nest++; \
    \
    __VA_ARGS__ \
    \
    /* Reset nests */ \
    _cspec->outer_nest++; \
    _cspec->inner_nest--; \
)

/**
 * @marco: describe
 * @desc: Expands to a block execution where tests are encompassed
 * @param object_name -> The name of the unit to describe
 * @param ... -> The proc to extend to
 **/
#define describe(object_name, ...) _BLOCK( \
    _string_add_str(_cspec->display_tab, "    "); \
    _cspec->list_of_its = _new_vector(); \
    _cspec->list_of_contexts = _new_vector(); \
    _cspec->name_of_describe = _new_string(object_name); \
    \
    \
    /* Display describe data */ \
    printf("%s%s`%s`%s\n", \
    _string_get(_cspec->display_tab), \
    _string_get(_cspec->PURPLE), \
    object_name, \
    _string_get(_cspec->RESET)); \
    \
    \
    /* Construct the describe block */ \
    _vector *describe_block = _new_vector(); \
    _vector_add(describe_block, _vector_dup(_cspec->list_of_its)); \
    _vector_add(describe_block, _vector_dup(_cspec->list_of_contexts)); \
    _vector_add(describe_block, _cspec->name_of_describe); \
    _vector_add(_cspec->list_of_describes, describe_block); \
    \
    \
    _describe(__VA_ARGS__); \
    \
    \
    _string_skip(_cspec->display_tab, 4); \
    \
    /* desc([its], [contexts], name)*/ \
)

/**
 * @macro: context
 * @desc: Basically aliasing for descibe
 * @param object_name -> The name of the unit to describe
 * @param ... -> The proc to extend to
 **/
#define context(object_name, ...) _BLOCK( \
    _cspec->in_context_block = true; \
    _string_add_str(_cspec->display_tab, "    "); \
    _cspec->list_of_its = _new_vector(); \
    _cspec->name_of_context = _new_string(object_name); \
    \
    \
    /* Display context data */ \
    printf("%s%s`%s`%s\n", \
    _string_get(_cspec->display_tab), \
    _string_get(_cspec->YELLOW), \
    object_name, \
    _string_get(_cspec->WHITE)); \
    \
    \
    /* Construct data for export */ \
    _vector *context_block = _new_vector(); \
    _vector_add(context_block, _vector_dup(_cspec->list_of_its)); \
    _vector_add(context_block, _cspec->name_of_context); \
    \
    /* Grab the specific describe block using `inner_nest` */ \
    _vector *desc_block = _vector_get(_cspec->list_of_describes, _cspec->inner_nest); \
    _vector *list_of_cons = _vector_get(desc_block, 1); \
    _vector_add(list_of_cons, context_block); \
    \
    \
    _describe(__VA_ARGS__); \
    \
    \
    _string_skip(_cspec->display_tab, 4); \
    _cspec->in_context_block = false; \
    \
    /* con([its], name) */ \
)

/**
 * @macro: xit
 * @desc: Temporarily disables a test
 * @param proc_name -> The name of test to run
 * @param ... -> The actual test code
 **/
#define xit(proc_name, ...) _BLOCK( \
    /* Execute a `before` function */ \
    if(_cspec->before_func) (*_cspec->before_func)(); \
    _string_add_str(_cspec->display_tab, "    "); \
    \
    /* Setup for a skipped it block */ \
    _cspec->number_of_tests++; \
    _cspec->number_of_skipped_tests++; \
    _cspec->test_result_message = _new_string(""); \
    _cspec->name_of_tested_proc = _new_string(proc_name); \
    _cspec->list_of_asserts = _new_vector(); \
    _cspec->status_of_test = _SKIPPED; \
    \
    /* Dummy timers */ \
    size_t start_test_timer = _get_timer(); \
    size_t end_test_timer = _get_timer(); \
    \
    /* Check for valid test type */ \
    if((__streql(_string_get(_cspec->type_of_tests), "all")) \
    || (__streql(_string_get(_cspec->type_of_tests), "skipped"))) { \
        /* Display data about the skipped it */ \
        printf("%s%s- %s%s\n", \
        _string_get(_cspec->display_tab), \
        _string_get(_cspec->GRAY), \
        _string_get(_cspec->name_of_tested_proc), \
        _string_get(_cspec->RESET)); \
        \
        \
    } \
    _cspec->total_time_taken_for_tests += end_test_timer - start_test_timer; \
    \
    \
    _insert_it_block_in_list_of_its(); \
    \
    \
    /* Reset the display tab and execute the `after_func` */ \
    _string_skip(_cspec->display_tab, 4); \
    if(_cspec->after_func) (*_cspec->after_func)(); \
)

/**
 * @macro: it
 * @desc: Expands to a test run and saves all data gathered
 * @param proc_name -> The name of test to run
 * @param proc -> The actual test code
 **/
#define it(proc_name, ...) _BLOCK( \
    /* Execute a `before` function */ \
    if(_cspec->before_func) (*_cspec->before_func)(); \
    \
    /* Setup for an it block */ \
    _string_add_str(_cspec->display_tab, "    "); \
	_cspec->number_of_tests++; \
    _cspec->test_result_message = _new_string(""); \
    _cspec->name_of_tested_proc = _new_string(proc_name); \
    _cspec->list_of_asserts = _new_vector(); \
    \
    /* Assume its a passing test */ \
    _cspec->status_of_test = _PASSING; \
    _cspec->current_line = __LINE__; \
    _cspec->current_file = __FILE__; \
    \
    size_t start_test_timer = _get_timer(); \
    \
    /* Execute asserts */ \
    __VA_ARGS__ \
    /* jmp_buf _cspec->escape; */ \
    /* setjmp(&_cspec->escape); */ \
    size_t end_test_timer = _get_timer(); \
    \
    if(_cspec->status_of_test) { \
        /* In case all asserts pass we print passing test information */ \
        _cspec->number_of_passing_tests++; \
        if((__streql(_string_get(_cspec->type_of_tests), "all")) \
        || (__streql(_string_get(_cspec->type_of_tests), "passing"))) { \
            printf("%s%s✓%s it %s%s\n", \
            _string_get(_cspec->display_tab), \
            _string_get(_cspec->GREEN), \
            _string_get(_cspec->WHITE), \
            _string_get(_cspec->name_of_tested_proc), \
            _string_get(_cspec->RESET)); \
        } \
	} \
    else { \
        /* Even if 1 of the asserts in the current it block
            fails, assume we have a failing test */ \
		_cspec->number_of_failing_tests++; \
        if((__streql(_string_get(_cspec->type_of_tests), "all")) \
        || (__streql(_string_get(_cspec->type_of_tests), "failing"))) { \
            printf("%s%s✗%s it %s:\n%s%s\n", \
            _string_get(_cspec->display_tab), \
            _string_get(_cspec->RED), \
            _string_get(_cspec->WHITE), \
            _string_get(_cspec->name_of_tested_proc), \
            _string_get(_cspec->test_result_message), \
            _string_get(_cspec->RESET)); \
        } \
    } \
    /* Calculate the total time */ \
    _cspec->total_time_taken_for_tests += end_test_timer - start_test_timer; \
    \
    \
    _insert_it_block_in_list_of_its(); \
    \
    \
    /* Reset the display tab and execute the `after_func` */ \
    _string_skip(_cspec->display_tab, 4); \
    if(_cspec->after_func) (*_cspec->after_func)(); \
)


/**
 * @macro: assert_that
 * @desc: Asserts that a proc returns true
 * @param test -> The test proc to run
 **/
#define assert_that(test) _BLOCK( \
    _cspec->current_file = __FILE__; \
    _cspec->current_line = __LINE__; \
    _cspec->position_in_file = _new_string(""); \
    _cspec->assert_result = _new_string(""); \
    \
    /* Save the test token */ \
    _cspec->current_assert = #test; \
    \
    /* Assert a custom block of code */ \
	if(!(test)) { \
        _cspec->status_of_test = _FAILING; \
        _write_position_in_file(); \
        \
        /* Craft the string builder for terminal output */ \
        _string_add_str(_cspec->test_result_message, _string_get(_cspec->display_tab)); \
        _string_add_str(_cspec->test_result_message, _string_get(_cspec->WHITE)); \
        _string_add_str(_cspec->test_result_message, "    "); \
        _string_add_str(_cspec->test_result_message, _string_get(_cspec->position_in_file)); \
        _string_add_str(_cspec->test_result_message, "\n"); \
        _string_add_str(_cspec->test_result_message, _string_get(_cspec->display_tab)); \
        _string_add_str(_cspec->test_result_message, "        |> "); \
        _string_add_str(_cspec->test_result_message, _string_get(_cspec->RED)); \
        _string_add_str(_cspec->test_result_message, "`"); \
        _string_add_str(_cspec->test_result_message, #test); \
        _string_add_str(_cspec->test_result_message, "`"); \
        _string_add_str(_cspec->test_result_message, _string_get(_cspec->WHITE)); \
        _string_add_str(_cspec->test_result_message, " should be true\n"); \
        _string_add_str(_cspec->test_result_message, _string_get(_cspec->RESET)); \
/*************************************************************************/ \
        /* Craft the string builder for text output (miss the color codes) */ \
        _string_add_str(_cspec->assert_result, "|> `"); \
        _string_add_str(_cspec->assert_result, #test); \
        _string_add_str(_cspec->assert_result, "`"); \
        _string_add_str(_cspec->assert_result, " should be true"); \
/****************************************************************************/ \
        _vector *list_of_strings = _new_vector(); \
        _vector_add(list_of_strings, _string_get(_cspec->position_in_file)); \
        _vector_add(list_of_strings, _string_get(_cspec->assert_result)); \
/****************************************************************************/ \
        _vector_add(_cspec->list_of_asserts, list_of_strings); \
    } \
    \
    /* assert(string_builder) */ \
)

/**
 * @macro: nassert_that
 * @desc: Asserts that a proc returns false
 * @param test -> The test proc to run
 **/
#define nassert_that(test) _BLOCK( \
    _cspec->current_file = __FILE__; \
    _cspec->current_line = __LINE__; \
    _cspec->position_in_file = _new_string(""); \
    _cspec->assert_result = _new_string(""); \
    \
    /* Save the test token */ \
    _cspec->current_assert = #test; \
    \
	if((test)) { \
        _cspec->status_of_test = _FAILING; \
        _write_position_in_file(); \
        \
        /* Craft the string builder for terminal output */ \
        _string_add_str(_cspec->test_result_message, _string_get(_cspec->display_tab)); \
        _string_add_str(_cspec->test_result_message, _string_get(_cspec->WHITE)); \
        _string_add_str(_cspec->test_result_message, "    "); \
        _string_add_str(_cspec->test_result_message, _string_get(_cspec->position_in_file)); \
        _string_add_str(_cspec->test_result_message, "\n"); \
        _string_add_str(_cspec->test_result_message, _string_get(_cspec->display_tab)); \
        _string_add_str(_cspec->test_result_message, "        |> "); \
        _string_add_str(_cspec->test_result_message, _string_get(_cspec->RED)); \
        _string_add_str(_cspec->test_result_message, "`"); \
        _string_add_str(_cspec->test_result_message, #test); \
        _string_add_str(_cspec->test_result_message, "`"); \
        _string_add_str(_cspec->test_result_message, _string_get(_cspec->WHITE)); \
        _string_add_str(_cspec->test_result_message, " should be false\n"); \
        _string_add_str(_cspec->test_result_message, _string_get(_cspec->RESET)); \
/*************************************************************************/ \
        /* Craft the string builder for text output (miss the color codes) */ \
        _string_add_str(_cspec->assert_result, "|> `"); \
        _string_add_str(_cspec->assert_result, #test); \
        _string_add_str(_cspec->assert_result, "`"); \
        _string_add_str(_cspec->assert_result, " should be false"); \
/****************************************************************************/ \
        _vector *list_of_strings = _new_vector(); \
        _vector_add(list_of_strings, _string_get(_cspec->position_in_file)); \
        _vector_add(list_of_strings, _string_get(_cspec->assert_result)); \
/****************************************************************************/ \
        _vector_add(_cspec->list_of_asserts, list_of_strings); \
    } \
)

/**
 * @macro: export_test_results
 * @desc: Exports the test results to some file type
 * @param vec -> Either passing|failing|skipped|all
 * @param type -> Export type either txt|xml|markdown
 **/
#define export_test_results(name, vec, type) _BLOCK( \
    /* Check for valid type of test export */ \
    if(!__streql(vec, "passing") \
    && !__streql(vec, "failing") \
    && !__streql(vec, "skipped") \
    && !__streql(vec, "all")) { \
        printf("\n%sInput a type of test to export passing|failing|skipped|all%s\n\n", \
        _string_get(_cspec->RED), \
        _string_get(_cspec->RESET)); \
        return 0; \
    } \
    _cspec->type_of_export_tests = _new_string(vec); \
    \
    /* Reset the display tab */ \
    _string_delete(_cspec->display_tab); \
    _string_add_str(_cspec->display_tab, "    "); \
    if(type == "txt") { \
        _cspec->fd = fopen(name, "w+"); \
        _export_to_txt(); \
    } \
    else if(type == "xml") { \
        _cspec->fd = fopen(name, "w+"); \
        _export_to_xml(); \
    } \
    else if(type == "markdown") { \
        _cspec->fd = fopen(name, "w+"); \
        _export_to_md(); \
    } \
    else { \
        printf("\n%sSpecify the export type: `txt|xml|markdown`%s\n\n", \
        _string_get(_cspec->RED), \
        _string_get(_cspec->RESET)); \
        return 0; /* Exit the main function */ \
    } \
)

/**
 * @macro: _compare_values
 * @desc: Runs the actual assertion between 2 values
 * @param actual -> The value passed by the user
 * @param expected -> The value `actual` is tested against
 * @param func -> The assert function according to data type
 **/
#define _compare_values(actual, expected, func) _BLOCK( \
    if(func(actual, expected)) { \
        _cspec->status_of_test = _FAILING; \
        _write_position_in_file(); \
        _write_assert_actual_expected(); \
    } \
)

/**
 * @macro: _compare_array_values
 * @desc: Runs the actual assertion between 2 values
 * @param actual -> The value passed by the user
 * @param expected -> The value `actual` is tested against
 * @param func -> The assert function according to data type
 * @param len -> The length of the array to test
 **/
#define _compare_array_values(actual, expected, func, len) _BLOCK( \
    if(func(actual, expected, len)) { \
        _cspec->status_of_test = _FAILING; \
        _write_position_in_file(); \
        _write_assert_actual_expected(); \
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
        _compare_values(actual, expected, comparison_method); \
    }

#define define_assert_array(name_of_assert, data_type_token, to_string_method, comparison_method, len) \
    static inline void name_of_assert(data_type_token actual, data_type_token expected, size_t len) { \
        to_string_method(actual, expected, len); \
        _compare_array_values(actual, expected, comparison_method, len); \
    }

#endif
