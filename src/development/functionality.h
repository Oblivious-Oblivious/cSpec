#ifndef __FUNCTIONALITY_H_
#define __FUNCTIONALITY_H_

#include "includes.h"
#include "interface.h"
#include "data_structures.h"

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
 * @macro: _to_string_charptr_write
 * @desc: Writes actual and expected values
 * @param actual -> The value passed by the user
 * @param expected -> The value `actual` is tested against
 **/
#define _to_string_charptr_write(actual, expected) _BLOCK( \
    _cspec->current_actual = _new_string(""); \
    _cspec->current_expected = _new_string(""); \
    _string_add_str(_cspec->current_actual, actual); \
    _string_add_str(_cspec->current_expected, expected); \
)

/**
 * @func: _fabs
 * @desc: Abs for floats
 * @param value -> The value to get `abs` for
 * @return Absolute value
 **/
static double _fabs(double value) {
    return value < 0 ? (-value) : (value);
}

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
 * @func: _double_comparison
 * @desc: A function that compares doubles for assertions
 * @param actual -> The value passed by the user
 * @param expected -> The value `actual` is tested against
 * @return a boolean
 **/
static bool _double_comparison(double actual, double expected) {
    /* Calculate the margin to which the difference
        is too big so the test fails */
    return _fabs(actual - expected) > 1E-12;
}

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
 * @func: _error_assertion_not_supported
 * @desc: Throw an error message
 **/
static void _error_assertion_not_supported(void) {
    printf("Error. Generic assertions not supported for this compiler.\n");
}

/**
 * @func: _write_position_in_file
 * @desc: Setup file position and line number for the current assert
 **/
static void _write_position_in_file(void) {
    _cspec->position_in_file = _new_string("");
    _string_add_str(_cspec->position_in_file, _cspec->current_file);
    _string_add_str(_cspec->position_in_file, ":");
    _string_add_int(_cspec->position_in_file, _cspec->current_line);
    _string_add_str(_cspec->position_in_file, ":");
}

/**
 * @func: _write_assert_actual_expected
 * @desc: Setup the assert description for printing and exporting
 **/
static void _write_assert_actual_expected(void) {
    _cspec->assert_result = _new_string("");
    _string_add_str(_cspec->test_result_message, _string_get(_cspec->display_tab));
    _string_add_str(_cspec->test_result_message, _string_get(_cspec->WHITE));
    _string_add_str(_cspec->test_result_message, "    ");
    _string_add_str(_cspec->test_result_message, _string_get(_cspec->position_in_file));
    _string_add_str(_cspec->test_result_message, "\n");
    _string_add_str(_cspec->test_result_message, _string_get(_cspec->display_tab));
    _string_add_str(_cspec->test_result_message, "        |> ");
    _string_add_str(_cspec->test_result_message, "`");
    _string_add_str(_cspec->test_result_message, _string_get(_cspec->current_expected));
    _string_add_str(_cspec->test_result_message, "` expected but got ");
    _string_add_str(_cspec->test_result_message, _string_get(_cspec->RED));
    _string_add_str(_cspec->test_result_message, "`");
    _string_add_str(_cspec->test_result_message, _string_get(_cspec->current_actual));
    _string_add_str(_cspec->test_result_message, "`\n");
    _string_add_str(_cspec->test_result_message, _string_get(_cspec->RESET));
/****************************************************************************/
    _string_add_str(_cspec->assert_result, "|> `");
    _string_add_str(_cspec->assert_result, _string_get(_cspec->current_expected));
    _string_add_str(_cspec->assert_result, "` expected but got ");
    _string_add_str(_cspec->assert_result, "`");
    _string_add_str(_cspec->assert_result, _string_get(_cspec->current_actual));
/****************************************************************************/
    _vector *list_of_strings = _new_vector();
    _vector_add(list_of_strings, _string_get(_cspec->position_in_file));
    _vector_add(list_of_strings, _string_get(_cspec->assert_result));
/****************************************************************************/
    _vector_add(_cspec->list_of_asserts, list_of_strings);
}

/**
 * @func: _write_nassert_actual_expected
 * @desc: Setup the `not` assert description for printing and exporting
 **/
static void _write_nassert_actual_expected(void) {
    _cspec->assert_result = _new_string("");
    _string_add_str(_cspec->test_result_message, _string_get(_cspec->display_tab));
    _string_add_str(_cspec->test_result_message, _string_get(_cspec->WHITE));
    _string_add_str(_cspec->test_result_message, "    ");
    _string_add_str(_cspec->test_result_message, _string_get(_cspec->position_in_file));
    _string_add_str(_cspec->test_result_message, "\n");
    _string_add_str(_cspec->test_result_message, _string_get(_cspec->display_tab));
    _string_add_str(_cspec->test_result_message, "        |> ");
    _string_add_str(_cspec->test_result_message, "expected that `");
    _string_add_str(_cspec->test_result_message, _string_get(_cspec->current_expected));
    _string_add_str(_cspec->test_result_message, "` would differ from ");
    _string_add_str(_cspec->test_result_message, _string_get(_cspec->RED));
    _string_add_str(_cspec->test_result_message, "`");
    _string_add_str(_cspec->test_result_message, _string_get(_cspec->current_actual));
    _string_add_str(_cspec->test_result_message, "`");
    _string_add_str(_cspec->test_result_message, _string_get(_cspec->WHITE));
    _string_add_str(_cspec->test_result_message, " but they are the same\n");
    _string_add_str(_cspec->test_result_message, _string_get(_cspec->RESET));
/****************************************************************************/
    _string_add_str(_cspec->assert_result, "|> expected that `");
    _string_add_str(_cspec->assert_result, _string_get(_cspec->current_expected));
    _string_add_str(_cspec->assert_result, "` would differ from ");
    _string_add_str(_cspec->assert_result, "`");
    _string_add_str(_cspec->assert_result, _string_get(_cspec->current_actual));
    _string_add_str(_cspec->assert_result, "`");
    _string_add_str(_cspec->assert_result, " but they are the same");
/****************************************************************************/
    _vector *list_of_strings = _new_vector();
    _vector_add(list_of_strings, _string_get(_cspec->position_in_file));
    _vector_add(list_of_strings, _string_get(_cspec->assert_result));
/****************************************************************************/
    _vector_add(_cspec->list_of_asserts, list_of_strings);
}

/**
 * @func: _insert_it_block_in_list_of_its
 * @desc: Inserts the it block in its designated vector index
 **/
static void _insert_it_block_in_list_of_its(void) {
    /* Setup the it block vector */
    _vector *it_block = _new_vector();
    _vector_add(it_block, _cspec->list_of_asserts);
    _vector_add(it_block, _cspec->name_of_tested_proc);
    _vector_add(it_block, _cspec->status_of_test);

    if(_cspec->in_context_block) {
        /* Insert in a context block */
        if(_vector_length(_cspec->list_of_describes) > 1) {
            /* Stupid indexing for finding the correct nested blocks */
            _vector *desc_block = _vector_get(_cspec->list_of_describes,
                -(_vector_length(_cspec->list_of_describes) - _cspec->inner_nest - 1));
            _vector *list_of_cons = _vector_get(desc_block, 1);
            _vector *con_block = _vector_get(list_of_cons,
                _vector_length(list_of_cons) - 1);
            _vector *list_of_con_its = _vector_get(con_block, 0);
            _vector_add(list_of_con_its, it_block);
        }
        else {
            _vector *desc_block = _vector_get(_cspec->list_of_describes,
                _vector_length(_cspec->list_of_describes) - 1);
            _vector *list_of_cons = _vector_get(desc_block, 1);
            _vector *con_block = _vector_get(list_of_cons,
                _vector_length(list_of_cons) - 1);
            _vector *list_of_con_its = _vector_get(con_block, 0);
            _vector_add(list_of_con_its, it_block);
        }
    }
    else {
        /* Insert in a describe block */
        _vector *desc_block = _vector_get(_cspec->list_of_describes,
            _vector_length(_cspec->list_of_describes) - 1 - _cspec->outer_nest);
        _vector *list_of_desc_its = _vector_get(desc_block, 0);
        _vector_add(list_of_desc_its, it_block);
    }
    /* it([asserts], name, status) */
}

/**
 * @func: _report_time_taken_for_tests
 * @desc: Report the number of tests and time taken while testing
 **/
static void _report_time_taken_for_tests(void) {
    printf("\n%s● %d tests\n%s✓ %d passing\n%s✗ %d failing\n%s- %d skipped\n",
    _string_get(_cspec->YELLOW),
    _cspec->number_of_tests,
    _string_get(_cspec->GREEN),
    _cspec->number_of_passing_tests,
    _string_get(_cspec->RED),
    _cspec->number_of_failing_tests,
    _string_get(_cspec->GRAY),
    _cspec->number_of_skipped_tests,
    _string_get(_cspec->RESET));

    /* Print in seconds if the time is more than 100ms */
    if(_cspec->total_time_taken_for_tests > 100000000) {
        printf("%s★ Finished in %.5f seconds%s\n",
        _string_get(_cspec->CYAN),
        _cspec->total_time_taken_for_tests / 1000000000.0,
        _string_get(_cspec->RESET));
    }
    /* Else print in miliseconds */
    else {
        printf("%s★ Finished in %.5f ms%s\n",
        _string_get(_cspec->CYAN),
        _cspec->total_time_taken_for_tests / 1000000.0,
        _string_get(_cspec->RESET));
    }
}

/**
 * @func: _fprintf_asserts
 * @desc: Writes asserts to an output file
 * @param assert -> The assert string to export
 **/
static void _fprintf_asserts(_vector *assert) {
    fprintf(_cspec->fd, "%s            %s\n                %s\n\n",
    _string_get(_cspec->display_tab),
    _vector_get(assert, 0),
    _vector_get(assert, 1));
}

/**
 * @func: _fprintf_its
 * @desc: Writes it blocks to an output file
 * @param it_block -> The it block to iterate against
 **/
static void _fprintf_its(_vector *it_block) {
    /* Check for type of test */
    if(((int)_vector_get(it_block, 2) == _FAILING)) {
        if(__streql(_string_get(_cspec->type_of_export_tests), "failing")
        || __streql(_string_get(_cspec->type_of_export_tests), "all")) {
            /* Write failing tests */
            fprintf(_cspec->fd, "%s        ✗ it %s\n",
            _string_get(_cspec->display_tab),
            _string_get(_vector_get(it_block, 1)));
            _vector *asserts = _vector_get(it_block, 0);

            /* Call to print the asserts iteratevely for the current it block */
            _vector_map(asserts, _fprintf_asserts);
        }
    }
    else if(((int)_vector_get(it_block, 2) == _SKIPPED)) {
        if(__streql(_string_get(_cspec->type_of_export_tests), "skipped")
        || __streql(_string_get(_cspec->type_of_export_tests), "all")) {
            /* Write skipped tests */
            fprintf(_cspec->fd, "%s        - it %s\n",
            _string_get(_cspec->display_tab),
            _string_get(_vector_get(it_block, 1)));
        }
    }
    else if(((int)_vector_get(it_block, 2) == _PASSING)) {
        /* Write passing tests */
        if(__streql(_string_get(_cspec->type_of_export_tests), "passing")
        || __streql(_string_get(_cspec->type_of_export_tests), "all")) {
            fprintf(_cspec->fd, "%s        ✓ it %s\n",
            _string_get(_cspec->display_tab),
            _string_get(_vector_get(it_block, 1)));
        }
    }
}

/**
 * @func: _fprintf_cons
 * @desc: Writes context blocks to an output file
 * @param con -> The context block to iterate against
 **/
static void _fprintf_cons(_vector *con) {
    _string_add_str(_cspec->display_tab, "    ");

    fprintf(_cspec->fd, "        `%s`\n",
    _string_get(_vector_get(con, 1)));
    _vector *its = _vector_get(con, 0);

    /* Iterate over the it blocks under the context block */
    _vector_map(its, _fprintf_its);
    
    _string_skip(_cspec->display_tab, 4);
}

/**
 * @func: _fprintf_describes
 * @desc: Writes describe blocks to an output file
 * @param desc -> The describe block to iterate against
 **/
static void _fprintf_describes(_vector *desc) {
    fprintf(_cspec->fd, "    `%s`\n",
    _string_get(_vector_get(desc, 2)));

    /* Iterate over the it blocks under the describe block */
    _vector *its = _vector_get(desc, 0);
    _vector_map(its, _fprintf_its);

    /* Iterate over the context blocks under the describe block */
    _vector *cons = _vector_get(desc, 1);
    _vector_map(cons, _fprintf_cons);
}

/**
 * @func: _fprintf_modules
 * @desc: Writes module blocks to an output file
 * @param mod -> The module block to iterate against
 **/
static void _fprintf_modules(_vector *mod) {
    _string_delete(_cspec->display_tab);
    fprintf(_cspec->fd, "\nModule `%s`\n",
    _string_get(_vector_get(mod, 1)));
    _vector *descs = _vector_get(mod, 0);

    /* Iterate over the describe blocks under the module block */
    _vector_map(descs, _fprintf_describes);
}

/**
 * @func: _export_to_txt
 * @desc: Export test results into a txt file
 **/
static void _export_to_txt(void) {
    _vector_map(_cspec->list_of_modules, _fprintf_modules);
    fclose(_cspec->fd);
}

/**
 * @func: _xml_write_asserts
 * @desc: Writes asserts to an xml file
 * @param assert -> The assert block containing strings to print
 **/
static void _xml_write_asserts(_vector *assert) {
    fprintf(_cspec->fd, "%s                <failure>\n",
    _string_get(_cspec->display_tab));
    
    fprintf(_cspec->fd, "%s                    <position line=\"%s\" />\n",
    _string_get(_cspec->display_tab),
    _vector_get(assert, 0));

    fprintf(_cspec->fd, "%s                    <error message=\"%s\" />\n",
    _string_get(_cspec->display_tab),
    _vector_get(assert, 1));

    fprintf(_cspec->fd, "%s                </failure>\n",
    _string_get(_cspec->display_tab));
}

/**
 * @func: _xml_write_its
 * @desc: Writes it blocks to an xml file
 * @param it_block -> The it block to iterate against
 **/
static void _xml_write_its(_vector *it_block) {
    /* Check for type of test */
    if(((int)_vector_get(it_block, 2) == _FAILING)) {
        if(__streql(_string_get(_cspec->type_of_export_tests), "failing")
        || __streql(_string_get(_cspec->type_of_export_tests), "all")) {
            /* Write failing tests */
            fprintf(_cspec->fd, "            %s<it name=\"%s\", status=\"failing\">\n",
            _string_get(_cspec->display_tab),
            _string_get(_vector_get(it_block, 1)));
            _vector *asserts = _vector_get(it_block, 0);

            /* Call to print the asserts iteratevely for the current it block */
            _vector_map(asserts, _xml_write_asserts);
            fprintf(_cspec->fd, "            %s<it/>\n",
            _string_get(_cspec->display_tab));
        }
    }
    else if(((int)_vector_get(it_block, 2) == _SKIPPED)) {
        if(__streql(_string_get(_cspec->type_of_export_tests), "skipped")
        || __streql(_string_get(_cspec->type_of_export_tests), "all")) {
            /* Write skipped tests */
            fprintf(_cspec->fd, "            %s<xit name=\"%s\", status=\"skipped\" />\n",
            _string_get(_cspec->display_tab),
            _string_get(_vector_get(it_block, 1)));
        }
    }
    else if(((int)_vector_get(it_block, 2) == _PASSING)) {
        /* Write passing tests */
        if(__streql(_string_get(_cspec->type_of_export_tests), "passing")
        || __streql(_string_get(_cspec->type_of_export_tests), "all")) {
            fprintf(_cspec->fd, "            %s<it name=\"%s\", status=\"passing\" />\n",
            _string_get(_cspec->display_tab),
            _string_get(_vector_get(it_block, 1)));
        }
    }
}

/**
 * @func: _xml_write_cons
 * @desc: Writes context blocks to an xml file
 * @param con -> The context block to iterate against
 **/
static void _xml_write_cons(_vector *con) {
    _string_add_str(_cspec->display_tab, "    ");
    fprintf(_cspec->fd, "            <context name=\"%s\">\n",
    _string_get(_vector_get(con, 1)));
    _vector *its = _vector_get(con, 0);

    /* Iterate over the it blocks under the context block */
    _vector_map(its, _xml_write_its);
    fprintf(_cspec->fd, "            </context>\n");
    _string_skip(_cspec->display_tab, 4);
}

/**
 * @func: _xml_write_describes
 * @desc: Writes describe blocks to an xml file
 * @param desc -> The current describe block to iterate against
 **/
static void _xml_write_describes(_vector *desc) {
    fprintf(_cspec->fd, "        <describe name=\"%s\">\n",
    _string_get(_vector_get(desc, 2)));

    /* Iterate over the it blocks under the describe block */
    _vector *its = _vector_get(desc, 0);
    _vector_map(its, _xml_write_its);

    /* Iterate over the context blocks under the describe block */
    _vector *cons = _vector_get(desc, 1);
    _vector_map(cons, _xml_write_cons);
    fprintf(_cspec->fd, "        </describe>\n");
}

/**
 * @func: _xml_write_modules
 * @desc: Writes module blocks to an xml file
 * @param mod -> The module block to iterate against
 **/
static void _xml_write_modules(_vector *mod) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    _string_delete(_cspec->display_tab);
    fprintf(_cspec->fd, "    <module name=\"%s\", failures=\"%d\", skipped=\"%d\", tests=\"%d\", timestamp=\"%d-%02d-%02d %02d:%02d:%02d\">\n",
    _string_get(_vector_get(mod, 1)),
    _cspec->number_of_failing_tests,
    _cspec->number_of_skipped_tests,
    _cspec->number_of_tests,
    tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    _vector *descs = _vector_get(mod, 0);

    /* Iterate over the describe blocks under the module block */
    _vector_map(descs, _xml_write_describes);
    fprintf(_cspec->fd, "    </module>\n");
}

/**
 * @func: _export_to_xml
 * @desc: Export test results into an xml file
 **/
static void _export_to_xml(void) {
    fprintf(_cspec->fd, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");
    
    if(_cspec->total_time_taken_for_tests > 100000000) {
        fprintf(_cspec->fd, "<modules duration=\"%.5f seconds\">\n",
        _cspec->total_time_taken_for_tests / 1000000000.0);
    }
    else {
        fprintf(_cspec->fd, "<modules duration=\"%.5f ms\">\n",
        _cspec->total_time_taken_for_tests / 1000000.0);
    }
    
    _vector_map(_cspec->list_of_modules, _xml_write_modules);
    fprintf(_cspec->fd, "</modules>\n");
    fclose(_cspec->fd);
}

/**
 * @func: _export_to_md
 * @desc: Export test results into a markdown file
 **/
static void _export_to_md(void) {
}

/**
 * @func: _signal_handler
 * @desc: Handles errors where undefined behaviour
 *         calls for segfauls or other singals
 * @param signal_id -> The number assign to the signal
 **/
static void _signal_handler(const int signal_id) {
    _string *id = _new_string("");
    _string_add_int(id, signal_id);
    char *sig_description;

    /* Find the correct signal description */
    switch(signal_id) {
        /* SIGINT */ case 2: sig_description = _vector_get(_cspec->signals_vector, 0); break;
        /* SIGILL */ case 4: sig_description = _vector_get(_cspec->signals_vector, 1); break;
        /* SIGABRT */ case 6: sig_description = _vector_get(_cspec->signals_vector, 2); break;
        /* SIGFPE */ case 8: sig_description = _vector_get(_cspec->signals_vector, 3); break;
        /* SIGSEGV */ case 11: sig_description = _vector_get(_cspec->signals_vector, 4); break;
        /* SIGTERM */ case 15: sig_description = _vector_get(_cspec->signals_vector, 5); break;
    }

    /* Craft a response for signal errors */
    _string *sig = _new_string("");
    _string_add_str(sig, _string_get(_cspec->display_tab));
    _string_add_str(sig, _string_get(_cspec->RED));
    _string_add_str(sig, "✗");
    _string_add_str(sig, _string_get(_cspec->WHITE));
    _string_add_str(sig, " it ");
    _string_add_str(sig, _string_get(_cspec->name_of_tested_proc));
    _string_add_str(sig, ":\n");
    _string_add_str(sig, _string_get(_cspec->display_tab));
    _string_add_str(sig, "    ");
    _string_add_str(sig, _cspec->current_file);
    _string_add_str(sig, ":");
    _string_add_int(sig, _cspec->current_line);
    _string_add_str(sig, "\n");
    _string_add_str(sig, _string_get(_cspec->display_tab));
    _string_add_str(sig, "        |> ");
    _string_add_str(sig, "signal: ");
    _string_add_str(sig, _string_get(_cspec->RED));
    _string_add_str(sig, sig_description);
    _string_add_str(sig, _string_get(_cspec->WHITE));
    _string_add_str(sig, " got caught on \n");
    _string_add_str(sig, _string_get(_cspec->display_tab));
    _string_add_str(sig, "          -> ");
    _string_add_str(sig, _string_get(_cspec->RED));
    _string_add_str(sig, "`");

    /* This only works for `assert_that` blocks */
    _string_add_str(sig, _cspec->current_assert);
    /*------------------------------------------*/

    _string_add_str(sig, "`");
    _string_add_str(sig, _string_get(_cspec->RESET));
    printf("%s\n\n", _string_get(sig));
    // signal(signal_id, _signal_handler);
    signal(signal_id, SIG_DFL);
    // longjmp(&_cspec->escape, 1);
}

/**
 * @func: _setup_test_data
 * @desc: Allocates memory for vectors to save test results in
 **/
static void _setup_test_data(void) {
    /* Printf a neat intro */
    printf("\033[38;5;95m/######## ########/\n");
    printf("\033[38;5;95m/##### \033[38;5;89mc\033[38;5;90mS\033[38;5;91mp\033[38;5;92me\033[38;5;93mc\033[0m \033[38;5;95m#####/\n");
    printf("/######## ########/\033[0m\n");
    
    /* Craft the global variable struct */
    _cspec = malloc(sizeof(_cspec_data_struct) + 1);

    _cspec->number_of_tests = 0;
    _cspec->number_of_passing_tests = 0;
    _cspec->number_of_failing_tests = 0;
    _cspec->number_of_skipped_tests = 0;
    _cspec->total_time_taken_for_tests = 0;
    _cspec->status_of_test = _PASSING;
    // jmp_buf _cspec->escape;

    _cspec->test_result_message = NULL;
    _cspec->name_of_tested_proc = NULL;
    _cspec->name_of_describe = NULL;
    _cspec->name_of_context = NULL;

    _cspec->current_assert = NULL;
    _cspec->current_file = NULL;
    _cspec->current_line = NULL;
    _cspec->current_actual = NULL;
    _cspec->current_expected = NULL;
    _cspec->position_in_file = NULL;
    _cspec->assert_result = NULL;
    _cspec->type_of_tests = NULL;
    _cspec->type_of_export_tests = NULL;

    /** Before and after procs **/
    _cspec->before_func = NULL;
    _cspec->after_func = NULL;

    _cspec->display_tab = NULL;
    _cspec->signals_vector = _new_vector();
    _cspec->inner_nest = -1;
    _cspec->outer_nest = 0;
    _cspec->in_context_block = false;

    /** Colors **/
    _cspec->GREEN = _new_string("\033[38;5;78m");
    _cspec->RED = _new_string("\033[1;31m");
    _cspec->YELLOW = _new_string("\033[38;5;11m");
    _cspec->PURPLE = _new_string("\033[38;5;207m");
    _cspec->CYAN = _new_string("\033[1;36m");
    _cspec->GRAY = _new_string("\033[38;5;244m");
    _cspec->WHITE = _new_string("\033[38;5;254m");
    _cspec->RESET = _new_string("\033[0m");
    _cspec->BACK_PURPLE = _new_string("\033[48;5;89m");

    _cspec->list_of_modules = _new_vector();
    _cspec->list_of_describes = _new_vector();
    _cspec->list_of_contexts = _new_vector();
    _cspec->list_of_its = _new_vector();
    _cspec->list_of_asserts = _new_vector();

    /* Signals that can be processed */
    _vector_add(_cspec->signals_vector, "`SIGINT | Interrupt`");
    _vector_add(_cspec->signals_vector, "`SIGILL | Illegal instruction`");
    _vector_add(_cspec->signals_vector, "`SIGABRT | Abort`");
    _vector_add(_cspec->signals_vector, "`SIGFPE | Floating - Point arithmetic exception`");
    _vector_add(_cspec->signals_vector, "`SIGSEGV | Segmentation Violation`");
    _vector_add(_cspec->signals_vector, "`SIGTERM | Termination`");

    /* Setup the signals */
    signal(SIGINT, _signal_handler);
    signal(SIGILL, _signal_handler);
    signal(SIGABRT, _signal_handler);
    signal(SIGFPE, _signal_handler);
    signal(SIGSEGV, _signal_handler);
    signal(SIGTERM, _signal_handler);
}

#endif