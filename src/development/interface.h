#ifndef __INTERFACE_H_
#define __INTERFACE_H_

/* #include "helpers.h" */

/**
 * @macro: spec
 * @desc: Call the main automation function
 **/
#define spec(...) \
    int main(int argc, char **argv) { \
        CSPEC_BLOCK(__VA_ARGS__); \
    }

/**
 * @macro: spec_suite
 * @desc: A simple function definition for running test suites
 * @param ... -> The block of modules to run
 **/
#define spec_suite(...) \
    /* Define a function to run tests modules at */ \
    static void run_spec_suite(char *type_of_tests) { \
        cspec_setup_test_data(); \
        cspec->type_of_tests = cspec_string_new(type_of_tests); \
        \
        \
        /* Check for valid test type */ \
        if(!cspec_streql(type_of_tests, "passing") \
        && !cspec_streql(type_of_tests, "failing") \
        && !cspec_streql(type_of_tests, "skipped") \
        && !cspec_streql(type_of_tests, "all")) { \
            printf("\n\033[1;31mInput a type of test to log passing|failing|skipped|all\033[0m\n\n"); \
            return; /* Exit the function */ \
        } \
        \
        \
        CSPEC_BLOCK(__VA_ARGS__); \
        cspec_report_time_taken_for_tests(); \
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
        cspec_vector *mod; \
        \
        cspec->name_of_module = cspec_string_new(#suite_name); \
        cspec->list_of_describes = cspec_vector_new(); \
        \
        \
        /* Print module information */ \
        printf("\n%s%sModule `%s`%s\n", \
        cspec_string_get(cspec->BACK_PURPLE), \
        cspec_string_get(cspec->YELLOW), \
        cspec_string_get(cspec->name_of_module), \
        cspec_string_get(cspec->RESET)); \
        cspec->display_tab = cspec_string_new(""); \
        \
        \
        CSPEC_BLOCK(__VA_ARGS__); /* Run describes */ \
        \
        \
        /* Construct a module and save for exporting */ \
        mod = cspec_vector_new(); \
        cspec_vector_add(mod, cspec->list_of_describes); \
        cspec_vector_add(mod, cspec->name_of_module); \
        cspec_vector_add(cspec->list_of_modules, mod); \
    }

/**
 * @macro: before
 * @desc: Expands to a setup proc that gets executed before the tests
 * @param ... -> The proc to run
 **/
#define before(...) CSPEC_BLOCK( \
    __VA_ARGS__ \
)

/**
 * @marco: after
 * @desc: Expands to a teardown proc that gets executed after the tests
 * @param ... -> The proc to run
 **/
#define after(...) CSPEC_BLOCK( \
    __VA_ARGS__ \
)

/**
 * @macro: before_each
 * @desc: Sets the argument to a function to run before each it block
 * @param func -> The func to set
 **/
#define before_each(func) CSPEC_BLOCK( \
    cspec->before_func = func; \
)

/**
 * @marco: after_each
 * @desc: Sets the argument to a function to run after each it block
 * @param func -> The func to set
 **/
#define after_each(func) CSPEC_BLOCK( \
    cspec->after_func = func; \
)

/**
 * @macro: cspec_describe
 * @desc: Defines a `describe` level block
 * @param __VA_ARGS__
 **/
#define cspec_describe(...) CSPEC_BLOCK( \
    if(cspec->inner_nest == -1) cspec->outer_nest = 0; \
    \
    /* On every describe block we nest inwards */ \
    cspec->inner_nest++; \
    \
    __VA_ARGS__ \
    \
    /* Reset nests */ \
    cspec->outer_nest++; \
    cspec->inner_nest--; \
)

/**
 * @marco: describe
 * @desc: Expands to a block execution where tests are encompassed
 * @param object_name -> The name of the unit to describe
 * @param ... -> The proc to extend to
 **/
#define describe(object_name, ...) CSPEC_BLOCK( \
    cspec_vector *describe_block; \
    \
    cspec_string_add_str(cspec->display_tab, "    "); \
    cspec->list_of_its = cspec_vector_new(); \
    cspec->list_of_contexts = cspec_vector_new(); \
    cspec->name_of_describe = cspec_string_new(object_name); \
    \
    \
    /* Display describe data */ \
    printf("%s%s`%s`%s\n", \
    cspec_string_get(cspec->display_tab), \
    cspec_string_get(cspec->PURPLE), \
    object_name, \
    cspec_string_get(cspec->RESET)); \
    \
    \
    /* Construct the describe block */ \
    describe_block = cspec_vector_new(); \
    cspec_vector_add(describe_block, cspec_vector_dup(cspec->list_of_its)); \
    cspec_vector_add(describe_block, cspec_vector_dup(cspec->list_of_contexts)); \
    cspec_vector_add(describe_block, cspec->name_of_describe); \
    cspec_vector_add(cspec->list_of_describes, describe_block); \
    \
    \
    cspec_describe(__VA_ARGS__); \
    \
    \
    cspec_string_skip(cspec->display_tab, 4); \
)

/**
 * @macro: context
 * @desc: Basically aliasing for descibe
 * @param object_name -> The name of the unit to describe
 * @param ... -> The proc to extend to
 **/
#define context(object_name, ...) CSPEC_BLOCK( \
    cspec_vector *context_block; \
    cspec_vector *desc_block; \
    cspec_vector *list_of_cons; \
    \
    cspec->in_context_block = true; \
    cspec_string_add_str(cspec->display_tab, "    "); \
    cspec->list_of_its = cspec_vector_new(); \
    cspec->name_of_context = cspec_string_new(object_name); \
    \
    \
    /* Display context data */ \
    printf("%s%s`%s`%s\n", \
    cspec_string_get(cspec->display_tab), \
    cspec_string_get(cspec->YELLOW), \
    object_name, \
    cspec_string_get(cspec->WHITE)); \
    \
    \
    /* Construct data for export */ \
    context_block = cspec_vector_new(); \
    cspec_vector_add(context_block, cspec_vector_dup(cspec->list_of_its)); \
    cspec_vector_add(context_block, cspec->name_of_context); \
    \
    /* Grab the specific describe block using `inner_nest` */ \
    desc_block = cspec_vector_get(cspec->list_of_describes, cspec->inner_nest); \
    list_of_cons = cspec_vector_get(desc_block, 1); \
    cspec_vector_add(list_of_cons, context_block); \
    \
    \
    cspec_describe(__VA_ARGS__); \
    \
    \
    cspec_string_skip(cspec->display_tab, 4); \
    cspec->in_context_block = false; \
)

/**
 * @macro: xit
 * @desc: Temporarily disables a test
 * @param proc_name -> The name of test to run
 * @param ... -> The actual test code
 **/
#define xit(proc_name, ...) CSPEC_BLOCK( \
    size_t start_test_timer; \
    size_t end_test_timer; \
    \
    /* Execute a `before` function */ \
    if(cspec->before_func) (*cspec->before_func)(); \
    cspec_string_add_str(cspec->display_tab, "    "); \
    \
    /* Setup for a skipped it block */ \
    cspec->number_of_tests++; \
    cspec->number_of_skipped_tests++; \
    cspec->test_result_message = cspec_string_new(""); \
    cspec->name_of_tested_proc = cspec_string_new(proc_name); \
    cspec->list_of_asserts = cspec_vector_new(); \
    cspec->status_of_test = CSPEC_SKIPPED; \
    \
    /* Dummy timers */ \
    start_test_timer = cspec_get_timer(); \
    end_test_timer = cspec_get_timer(); \
    \
    /* Check for valid test type */ \
    if((cspec_streql(cspec_string_get(cspec->type_of_tests), "all")) \
    || (cspec_streql(cspec_string_get(cspec->type_of_tests), "skipped"))) { \
        /* Display data about the skipped it */ \
        printf("%s%s- %s%s\n", \
        cspec_string_get(cspec->display_tab), \
        cspec_string_get(cspec->GRAY), \
        cspec_string_get(cspec->name_of_tested_proc), \
        cspec_string_get(cspec->RESET)); \
        \
        \
    } \
    cspec->total_time_taken_for_tests += end_test_timer - start_test_timer; \
    \
    \
    cspec_insert_it_block_in_list_of_its(); \
    \
    \
    /* Reset the display tab and execute the `after_func` */ \
    cspec_string_skip(cspec->display_tab, 4); \
    if(cspec->after_func) (*cspec->after_func)(); \
)

/**
 * @macro: it
 * @desc: Expands to a test run and saves all data gathered
 * @param proc_name -> The name of test to run
 * @param proc -> The actual test code
 **/
#define it(proc_name, ...) CSPEC_BLOCK( \
    /* Execute a `before` function */ \
    if(cspec->before_func) (*cspec->before_func)(); \
    \
    /* Setup for an it block */ \
    cspec_string_add_str(cspec->display_tab, "    "); \
	cspec->number_of_tests++; \
    cspec->test_result_message = cspec_string_new(""); \
    cspec->name_of_tested_proc = cspec_string_new(proc_name); \
    cspec->list_of_asserts = cspec_vector_new(); \
    \
    /* Assume its a passing test */ \
    cspec->status_of_test = CSPEC_PASSING; \
    cspec->current_line = __LINE__; \
    cspec->current_file = __FILE__; \
    \
    size_t start_test_timer = cspec_get_timer(); \
    \
    /* Execute asserts */ \
    __VA_ARGS__ \
    /* jmp_buf cspec->escape; */ \
    /* setjmp(&cspec->escape); */ \
    size_t end_test_timer = cspec_get_timer(); \
    \
    if(cspec->status_of_test) { \
        /* In case all asserts pass we print passing test information */ \
        cspec->number_of_passing_tests++; \
        if((cspec_streql(cspec_string_get(cspec->type_of_tests), "all")) \
        || (cspec_streql(cspec_string_get(cspec->type_of_tests), "passing"))) { \
            printf("%s%s✓%s it %s%s\n", \
            cspec_string_get(cspec->display_tab), \
            cspec_string_get(cspec->GREEN), \
            cspec_string_get(cspec->WHITE), \
            cspec_string_get(cspec->name_of_tested_proc), \
            cspec_string_get(cspec->RESET)); \
        } \
	} \
    else { \
        /* Even if 1 of the asserts in the current it block
            fails, assume we have a failing test */ \
		cspec->number_of_failing_tests++; \
        if((cspec_streql(cspec_string_get(cspec->type_of_tests), "all")) \
        || (cspec_streql(cspec_string_get(cspec->type_of_tests), "failing"))) { \
            printf("%s%s✗%s it %s:\n%s%s\n", \
            cspec_string_get(cspec->display_tab), \
            cspec_string_get(cspec->RED), \
            cspec_string_get(cspec->WHITE), \
            cspec_string_get(cspec->name_of_tested_proc), \
            cspec_string_get(cspec->test_result_message), \
            cspec_string_get(cspec->RESET)); \
        } \
    } \
    /* Calculate the total time */ \
    cspec->total_time_taken_for_tests += end_test_timer - start_test_timer; \
    \
    \
    cspec_insert_it_block_in_list_of_its(); \
    \
    \
    /* Reset the display tab and execute the `after_func` */ \
    cspec_string_skip(cspec->display_tab, 4); \
    if(cspec->after_func) (*cspec->after_func)(); \
)

/**
 * @macro: export_test_results
 * @desc: Exports the test results to some file type
 * @param vec -> Either passing|failing|skipped|all
 * @param type -> Export type either txt|xml|markdown
 **/
#define export_test_results(name, vec, type) CSPEC_BLOCK( \
    /* Check for valid type of test export */ \
    if(!cspec_streql(vec, "passing") \
    && !cspec_streql(vec, "failing") \
    && !cspec_streql(vec, "skipped") \
    && !cspec_streql(vec, "all")) { \
        printf("\n%sInput a type of test to export passing|failing|skipped|all%s\n\n", \
        cspec_string_get(cspec->RED), \
        cspec_string_get(cspec->RESET)); \
        return 0; \
    } \
    cspec->type_of_export_tests = cspec_string_new(vec); \
    \
    /* Reset the display tab */ \
    cspec_string_delete(cspec->display_tab); \
    cspec_string_add_str(cspec->display_tab, "    "); \
    if(type == "txt") { \
        cspec->fd = fopen(name, "w+"); \
        cspec_export_to_txt(); \
    } \
    else if(type == "xml") { \
        cspec->fd = fopen(name, "w+"); \
        cspec_export_to_xml(); \
    } \
    else if(type == "markdown") { \
        cspec->fd = fopen(name, "w+"); \
        cspec_export_to_md(); \
    } \
    else { \
        printf("\n%sSpecify the export type: `txt|xml|markdown`%s\n\n", \
        cspec_string_get(cspec->RED), \
        cspec_string_get(cspec->RESET)); \
        return 0; /* Exit the main function */ \
    } \
)

#endif
