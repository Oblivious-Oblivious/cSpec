#ifndef __FUNCTIONALITY_H_
#define __FUNCTIONALITY_H_

/*
    #include "includes.h"
    #include "interface.h"
    #include "data_structures.h"
    #include <time.h>
*/

/**
 * @func: cspec_error_assertion_not_supported
 * @desc: Throw an error message
 **/
static void cspec_error_assertion_not_supported(void) {
    printf("Error. Generic assertions not supported for this compiler.\n");
}

/**
 * @func: cspec_write_position_in_file
 * @desc: Setup file position and line number for the current assert
 **/
static void cspec_write_position_in_file(void) {
    cspec->position_in_file = cspec_string_new("");
    cspec_string_add_str(cspec->position_in_file, cspec->current_file);
    cspec_string_add_str(cspec->position_in_file, ":");
    cspec_string_add_int(cspec->position_in_file, cspec->current_line);
    cspec_string_add_str(cspec->position_in_file, ":");
}

/**
 * @func: cspec_write_assert_actual_expected
 * @desc: Setup the assert description for printing and exporting
 **/
static void cspec_write_assert_actual_expected(void) {
    cspec_vector *list_of_strings;
    cspec->assert_result = cspec_string_new("");
    cspec_string_add_str(cspec->test_result_message, cspec_string_get(cspec->display_tab));
    cspec_string_add_str(cspec->test_result_message, cspec_string_get(cspec->WHITE));
    cspec_string_add_str(cspec->test_result_message, "    ");
    cspec_string_add_str(cspec->test_result_message, cspec_string_get(cspec->position_in_file));
    cspec_string_add_str(cspec->test_result_message, "\n");
    cspec_string_add_str(cspec->test_result_message, cspec_string_get(cspec->display_tab));
    cspec_string_add_str(cspec->test_result_message, "        |> ");
    cspec_string_add_str(cspec->test_result_message, "`");
    cspec_string_add_str(cspec->test_result_message, cspec_string_get(cspec->current_expected));
    cspec_string_add_str(cspec->test_result_message, "` expected but got ");
    cspec_string_add_str(cspec->test_result_message, cspec_string_get(cspec->RED));
    cspec_string_add_str(cspec->test_result_message, "`");
    cspec_string_add_str(cspec->test_result_message, cspec_string_get(cspec->current_actual));
    cspec_string_add_str(cspec->test_result_message, "`\n");
    cspec_string_add_str(cspec->test_result_message, cspec_string_get(cspec->RESET));
/****************************************************************************/
    cspec_string_add_str(cspec->assert_result, "|> `");
    cspec_string_add_str(cspec->assert_result, cspec_string_get(cspec->current_expected));
    cspec_string_add_str(cspec->assert_result, "` expected but got ");
    cspec_string_add_str(cspec->assert_result, "`");
    cspec_string_add_str(cspec->assert_result, cspec_string_get(cspec->current_actual));
/****************************************************************************/
    list_of_strings = cspec_vector_new();
    cspec_vector_add(list_of_strings, cspec_string_get(cspec->position_in_file));
    cspec_vector_add(list_of_strings, cspec_string_get(cspec->assert_result));
/****************************************************************************/
    cspec_vector_add(cspec->list_of_asserts, list_of_strings);
}

/**
 * @func: cspec_write_nassert_actual_expected
 * @desc: Setup the `not` assert description for printing and exporting
 **/
static void cspec_write_nassert_actual_expected(void) {
    cspec_vector *list_of_strings;
    cspec->assert_result = cspec_string_new("");
    cspec_string_add_str(cspec->test_result_message, cspec_string_get(cspec->display_tab));
    cspec_string_add_str(cspec->test_result_message, cspec_string_get(cspec->WHITE));
    cspec_string_add_str(cspec->test_result_message, "    ");
    cspec_string_add_str(cspec->test_result_message, cspec_string_get(cspec->position_in_file));
    cspec_string_add_str(cspec->test_result_message, "\n");
    cspec_string_add_str(cspec->test_result_message, cspec_string_get(cspec->display_tab));
    cspec_string_add_str(cspec->test_result_message, "        |> ");
    cspec_string_add_str(cspec->test_result_message, "expected that `");
    cspec_string_add_str(cspec->test_result_message, cspec_string_get(cspec->current_expected));
    cspec_string_add_str(cspec->test_result_message, "` would differ from ");
    cspec_string_add_str(cspec->test_result_message, cspec_string_get(cspec->RED));
    cspec_string_add_str(cspec->test_result_message, "`");
    cspec_string_add_str(cspec->test_result_message, cspec_string_get(cspec->current_actual));
    cspec_string_add_str(cspec->test_result_message, "`");
    cspec_string_add_str(cspec->test_result_message, cspec_string_get(cspec->WHITE));
    cspec_string_add_str(cspec->test_result_message, " but they are the same\n");
    cspec_string_add_str(cspec->test_result_message, cspec_string_get(cspec->RESET));
/****************************************************************************/
    cspec_string_add_str(cspec->assert_result, "|> expected that `");
    cspec_string_add_str(cspec->assert_result, cspec_string_get(cspec->current_expected));
    cspec_string_add_str(cspec->assert_result, "` would differ from ");
    cspec_string_add_str(cspec->assert_result, "`");
    cspec_string_add_str(cspec->assert_result, cspec_string_get(cspec->current_actual));
    cspec_string_add_str(cspec->assert_result, "`");
    cspec_string_add_str(cspec->assert_result, " but they are the same");
/****************************************************************************/
    list_of_strings = cspec_vector_new();
    cspec_vector_add(list_of_strings, cspec_string_get(cspec->position_in_file));
    cspec_vector_add(list_of_strings, cspec_string_get(cspec->assert_result));
/****************************************************************************/
    cspec_vector_add(cspec->list_of_asserts, list_of_strings);
}

/**
 * @func: cspec_insert_it_block_in_list_of_its
 * @desc: Inserts the it block in its designated vector index
 **/
static void cspec_insert_it_block_in_list_of_its(void) {
    /* Setup the it block vector */
    cspec_vector *it_block = cspec_vector_new();
    cspec_vector_add(it_block, cspec->list_of_asserts);
    cspec_vector_add(it_block, cspec->name_of_tested_proc);
    cspec_vector_add(it_block, cspec->status_of_test);

    if(cspec->in_context_block) {
        /* Insert in a context block */
        if(cspec_vector_length(cspec->list_of_describes) > 1) {
            /* Stupid indexing for finding the correct nested blocks */
            cspec_vector *desc_block = (cspec_vector*)cspec_vector_get(cspec->list_of_describes,
                -(cspec_vector_length(cspec->list_of_describes) - cspec->inner_nest - 1));
            cspec_vector *list_of_cons = (cspec_vector*)cspec_vector_get(desc_block, 1);
            cspec_vector *con_block = (cspec_vector*)cspec_vector_get(list_of_cons,
                cspec_vector_length(list_of_cons) - 1);
            cspec_vector *list_of_con_its = (cspec_vector*)cspec_vector_get(con_block, 0);
            cspec_vector_add(list_of_con_its, it_block);
        }
        else {
            cspec_vector *desc_block = (cspec_vector*)cspec_vector_get(cspec->list_of_describes,
                cspec_vector_length(cspec->list_of_describes) - 1);
            cspec_vector *list_of_cons = (cspec_vector*)cspec_vector_get(desc_block, 1);
            cspec_vector *con_block = (cspec_vector*)cspec_vector_get(list_of_cons,
                cspec_vector_length(list_of_cons) - 1);
            cspec_vector *list_of_con_its = (cspec_vector*)cspec_vector_get(con_block, 0);
            cspec_vector_add(list_of_con_its, it_block);
        }
    }
    else {
        /* Insert in a describe block */
        cspec_vector *desc_block = (cspec_vector*)cspec_vector_get(cspec->list_of_describes,
            cspec_vector_length(cspec->list_of_describes) - 1 - cspec->outer_nest);
        cspec_vector *list_of_desc_its = (cspec_vector*)cspec_vector_get(desc_block, 0);
        cspec_vector_add(list_of_desc_its, it_block);
    }
}

/**
 * @func: cspec_report_time_taken_for_tests
 * @desc: Report the number of tests and time taken while testing
 **/
static void cspec_report_time_taken_for_tests(void) {
    printf("\n%s● %d tests\n%s✓ %d passing\n%s✗ %d failing\n%s- %d skipped%s\n",
    cspec_string_get(cspec->YELLOW),
    cspec->number_of_tests,
    cspec_string_get(cspec->GREEN),
    cspec->number_of_passing_tests,
    cspec_string_get(cspec->RED),
    cspec->number_of_failing_tests,
    cspec_string_get(cspec->GRAY),
    cspec->number_of_skipped_tests,
    cspec_string_get(cspec->RESET));

    /* Print in seconds if the time is more than 100ms */
    if(cspec->total_time_taken_for_tests > 100000000) {
        printf("%s★ Finished in %.5f seconds%s\n",
        cspec_string_get(cspec->CYAN),
        cspec->total_time_taken_for_tests / 1000000000.0,
        cspec_string_get(cspec->RESET));
    }
    /* Else print in miliseconds */
    else {
        printf("%s★ Finished in %.5f ms%s\n",
        cspec_string_get(cspec->CYAN),
        cspec->total_time_taken_for_tests / 1000000.0,
        cspec_string_get(cspec->RESET));
    }
}

/**
 * @func: cspec_fprintf_asserts
 * @desc: Writes asserts to an output file
 * @param assert -> The assert string to export
 **/
static void cspec_fprintf_asserts(cspec_vector *assert) {
    fprintf(cspec->fd, "%s            %s\n                %s\n\n",
    cspec_string_get(cspec->display_tab),
    (char*)cspec_vector_get(assert, 0),
    (char*)cspec_vector_get(assert, 1));
}

/**
 * @func: cspec_fprintf_its
 * @desc: Writes it blocks to an output file
 * @param it_block -> The it block to iterate against
 **/
static void cspec_fprintf_its(cspec_vector *it_block) {
    /* Check for type of test */
    if(((long)cspec_vector_get(it_block, 2) == CSPEC_FAILING)) {
        if(cspec_streql(cspec_string_get(cspec->type_of_export_tests), "failing")
        || cspec_streql(cspec_string_get(cspec->type_of_export_tests), "all")) {
            /* Write failing tests */
            cspec_vector *asserts;
            fprintf(cspec->fd, "%s        ✗ it %s\n",
            cspec_string_get(cspec->display_tab),
            cspec_string_get((cspec_string*)cspec_vector_get(it_block, 1)));
            asserts = (cspec_vector*)cspec_vector_get(it_block, 0);

            /* Call to print the asserts iteratevely for the current it block */
            cspec_vector_map(asserts, (cspec_lambda)cspec_fprintf_asserts);
        }
    }
    else if(((long)cspec_vector_get(it_block, 2) == CSPEC_SKIPPED)) {
        if(cspec_streql(cspec_string_get(cspec->type_of_export_tests), "skipped")
        || cspec_streql(cspec_string_get(cspec->type_of_export_tests), "all")) {
            /* Write skipped tests */
            fprintf(cspec->fd, "%s        - it %s\n",
            cspec_string_get(cspec->display_tab),
            cspec_string_get((cspec_string*)cspec_vector_get(it_block, 1)));
        }
    }
    else if(((long)cspec_vector_get(it_block, 2) == CSPEC_PASSING)) {
        /* Write passing tests */
        if(cspec_streql(cspec_string_get(cspec->type_of_export_tests), "passing")
        || cspec_streql(cspec_string_get(cspec->type_of_export_tests), "all")) {
            fprintf(cspec->fd, "%s        ✓ it %s\n",
            cspec_string_get(cspec->display_tab),
            cspec_string_get((cspec_string*)cspec_vector_get(it_block, 1)));
        }
    }
}

/**
 * @func: cspec_fprintf_cons
 * @desc: Writes context blocks to an output file
 * @param con -> The context block to iterate against
 **/
static void cspec_fprintf_cons(cspec_vector *con) {
    cspec_vector *its;

    cspec_string_add_str(cspec->display_tab, "    ");

    fprintf(cspec->fd, "        `%s`\n",
    cspec_string_get((cspec_string*)cspec_vector_get(con, 1)));
    its = (cspec_vector*)cspec_vector_get(con, 0);

    /* Iterate over the it blocks under the context block */
    cspec_vector_map(its, (cspec_lambda)cspec_fprintf_its);
    
    cspec_string_skip(cspec->display_tab, 4);
}

/**
 * @func: cspec_fprintf_describes
 * @desc: Writes describe blocks to an output file
 * @param desc -> The describe block to iterate against
 **/
static void cspec_fprintf_describes(cspec_vector *desc) {
    cspec_vector *its;

    fprintf(cspec->fd, "    `%s`\n",
    cspec_string_get((cspec_string*)cspec_vector_get(desc, 2)));

    /* Iterate over the it blocks under the describe block */
    its = (cspec_vector*)cspec_vector_get(desc, 0);
    cspec_vector_map(its, (cspec_lambda)cspec_fprintf_its);

    /* Iterate over the context blocks under the describe block */
    cspec_vector *cons = (cspec_vector*)cspec_vector_get(desc, 1);
    cspec_vector_map(cons, (cspec_lambda)cspec_fprintf_cons);
}

/**
 * @func: cspec_fprintf_modules
 * @desc: Writes module blocks to an output file
 * @param mod -> The module block to iterate against
 **/
static void cspec_fprintf_modules(cspec_vector *mod) {
    cspec_vector *descs;
    
    cspec_string_delete(cspec->display_tab);
    fprintf(cspec->fd, "\nModule `%s`\n",
    cspec_string_get((cspec_string*)cspec_vector_get(mod, 1)));
    descs = (cspec_vector*)cspec_vector_get(mod, 0);

    /* Iterate over the describe blocks under the module block */
    cspec_vector_map(descs, (cspec_lambda)cspec_fprintf_describes);
}

/**
 * @func: cspec_export_to_txt
 * @desc: Export test results into a txt file
 **/
static void cspec_export_to_txt(void) {
    cspec_vector_map(cspec->list_of_modules, (cspec_lambda)cspec_fprintf_modules);
    fclose(cspec->fd);
}

/**
 * @func: cspec_xml_write_asserts
 * @desc: Writes asserts to an xml file
 * @param assert -> The assert block containing strings to print
 **/
static void cspec_xml_write_asserts(cspec_vector *assert) {
    fprintf(cspec->fd, "%s                <failure>\n",
    cspec_string_get(cspec->display_tab));
    
    fprintf(cspec->fd, "%s                    <position>%s</position>\n",
    cspec_string_get(cspec->display_tab),
    (char*)cspec_vector_get(assert, 0));

    fprintf(cspec->fd, "%s                    <error>%s</error>\n",
    cspec_string_get(cspec->display_tab),
    (char*)cspec_vector_get(assert, 1));

    fprintf(cspec->fd, "%s                </failure>\n",
    cspec_string_get(cspec->display_tab));
}

/**
 * @func: cspec_xml_write_its
 * @desc: Writes it blocks to an xml file
 * @param it_block -> The it block to iterate against
 **/
static void cspec_xml_write_its(cspec_vector *it_block) {
    /* Check for type of test */
    if(((long)cspec_vector_get(it_block, 2) == CSPEC_FAILING)) {
        if(cspec_streql(cspec_string_get(cspec->type_of_export_tests), "failing")
        || cspec_streql(cspec_string_get(cspec->type_of_export_tests), "all")) {
            cspec_vector *asserts;
            /* Write failing tests */
            fprintf(cspec->fd, "            %s<it>\n                %s<name>%s</name>\n                %s<status>failing</status>\n",
            cspec_string_get(cspec->display_tab),
            cspec_string_get(cspec->display_tab),
            cspec_string_get((cspec_string*)cspec_vector_get(it_block, 1)),
            cspec_string_get(cspec->display_tab));
            asserts = (cspec_vector*)cspec_vector_get(it_block, 0);

            /* Call to print the asserts iteratevely for the current it block */
            cspec_vector_map(asserts, (cspec_lambda)cspec_xml_write_asserts);
            fprintf(cspec->fd, "            %s</it>\n",
            cspec_string_get(cspec->display_tab));
        }
    }
    else if(((long)cspec_vector_get(it_block, 2) == CSPEC_SKIPPED)) {
        if(cspec_streql(cspec_string_get(cspec->type_of_export_tests), "skipped")
        || cspec_streql(cspec_string_get(cspec->type_of_export_tests), "all")) {
            /* Write skipped tests */
            fprintf(cspec->fd, "            %s<xit>\n                %s<name>%s</name>\n                %s<status>skipped</status>\n",
            cspec_string_get(cspec->display_tab),
            cspec_string_get(cspec->display_tab),
            cspec_string_get((cspec_string*)cspec_vector_get(it_block, 1)),
            cspec_string_get(cspec->display_tab));
            fprintf(cspec->fd, "            %s</xit>\n",
            cspec_string_get(cspec->display_tab));
        }
    }
    else if(((long)cspec_vector_get(it_block, 2) == CSPEC_PASSING)) {
        /* Write passing tests */
        if(cspec_streql(cspec_string_get(cspec->type_of_export_tests), "passing")
        || cspec_streql(cspec_string_get(cspec->type_of_export_tests), "all")) {
            fprintf(cspec->fd, "            %s<it>\n                %s<name>%s</name>\n                %s<status>passing</status>\n",
            cspec_string_get(cspec->display_tab),
            cspec_string_get(cspec->display_tab),
            cspec_string_get((cspec_string*)cspec_vector_get(it_block, 1)),
            cspec_string_get(cspec->display_tab));
            fprintf(cspec->fd, "            %s</it>\n",
            cspec_string_get(cspec->display_tab));
        }
    }
}

/**
 * @func: cspec_xml_write_cons
 * @desc: Writes context blocks to an xml file
 * @param con -> The context block to iterate against
 **/
static void cspec_xml_write_cons(cspec_vector *con) {
    cspec_vector *its;
    cspec_string_add_str(cspec->display_tab, "    ");
    fprintf(cspec->fd, "            <context>\n                <name>%s</name>\n",
    cspec_string_get((cspec_string*)cspec_vector_get(con, 1)));
    its = (cspec_vector*)cspec_vector_get(con, 0);

    /* Iterate over the it blocks under the context block */
    cspec_vector_map(its, (cspec_lambda)cspec_xml_write_its);
    fprintf(cspec->fd, "            </context>\n");
    cspec_string_skip(cspec->display_tab, 4);
}

/**
 * @func: cspec_xml_write_describes
 * @desc: Writes describe blocks to an xml file
 * @param desc -> The current describe block to iterate against
 **/
static void cspec_xml_write_describes(cspec_vector *desc) {
    cspec_vector *its;
    cspec_vector *cons;

    fprintf(cspec->fd, "        <describe>\n            <name>%s</name>\n",
    cspec_string_get((cspec_string*)cspec_vector_get(desc, 2)));

    /* Iterate over the it blocks under the describe block */
    its = (cspec_vector*)cspec_vector_get(desc, 0);
    cspec_vector_map(its, (cspec_lambda)cspec_xml_write_its);

    /* Iterate over the context blocks under the describe block */
    cons = (cspec_vector*)cspec_vector_get(desc, 1);
    cspec_vector_map(cons, (cspec_lambda)cspec_xml_write_cons);
    fprintf(cspec->fd, "        </describe>\n");
}

/**
 * @func: cspec_xml_write_modules
 * @desc: Writes module blocks to an xml file
 * @param mod -> The module block to iterate against
 **/
static void cspec_xml_write_modules(cspec_vector *mod) {
    cspec_vector *descs;

    time_t t = time(NULL);
    struct tm calc_time;
    localtime_r(&calc_time, &t);

    cspec_string_delete(cspec->display_tab);
    fprintf(cspec->fd, "    <module>\n        <name>%s</name>\n        <failures>%d</failures>\n        <skipped>%d</skipped>\n        <tests>%d</tests>\n        <timestamp>%d-%02d-%02d %02d:%02d:%02d</timestamp>\n",
    cspec_string_get((cspec_string*)cspec_vector_get(mod, 1)),
    cspec->number_of_failing_tests,
    cspec->number_of_skipped_tests,
    cspec->number_of_tests,
    calc_time.tm_year + 1900, calc_time.tm_mon + 1, calc_time.tm_mday, calc_time.tm_hour, calc_time.tm_min, calc_time.tm_sec);
    descs = (cspec_vector*)cspec_vector_get(mod, 0);

    /* Iterate over the describe blocks under the module block */
    cspec_vector_map(descs, (cspec_lambda)cspec_xml_write_describes);
    fprintf(cspec->fd, "    </module>\n");
}

/**
 * @func: cspec_export_to_xml
 * @desc: Export test results into an xml file
 **/
static void cspec_export_to_xml(void) {
    fprintf(cspec->fd, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");
    
    if(cspec->total_time_taken_for_tests > 100000000) {
        fprintf(cspec->fd, "<modules>\n    <duration>%.5f seconds</duration>\n",
        cspec->total_time_taken_for_tests / 1000000000.0);
    }
    else {
        fprintf(cspec->fd, "<modules>\n    <duration>%.5f ms</duration>\n",
        cspec->total_time_taken_for_tests / 1000000.0);
    }
    
    cspec_vector_map(cspec->list_of_modules, (cspec_lambda)cspec_xml_write_modules);
    fprintf(cspec->fd, "</modules>\n");
    fclose(cspec->fd);
}

/**
 * @func: cspec_export_to_md
 * @desc: Export test results into a markdown file
 **/
static void cspec_export_to_md(void) {
    /* TODO COMPLETE EXPORTING TO MD */
}

/**
 * @func: cspec_export_to_html
 * @desc: Export test results into an html file
 **/
static void cspec_export_to_html(void) {
    /* TODO COMPLETE EXPORTING TO HTML */
}

/**
 * @func: cspec_signal_handler
 * @desc: Handles errors where undefined behaviour
 *         calls for segfauls or other singals
 * @param signal_id -> The number assign to the signal
 **/
static void cspec_signal_handler(const int signal_id) {
    cspec_string *id = cspec_string_new("");
    cspec_string_add_int(id, signal_id);
    char *sig_description;

    /* Find the correct signal description */
    switch(signal_id) {
        /* SIGINT */ case 2: sig_description = (char*)cspec_vector_get(cspec->signals_vector, 0); break;
        /* SIGILL */ case 4: sig_description = (char*)cspec_vector_get(cspec->signals_vector, 1); break;
        /* SIGABRT */ case 6: sig_description = (char*)cspec_vector_get(cspec->signals_vector, 2); break;
        /* SIGFPE */ case 8: sig_description = (char*)cspec_vector_get(cspec->signals_vector, 3); break;
        /* SIGSEGV */ case 11: sig_description = (char*)cspec_vector_get(cspec->signals_vector, 4); break;
        /* SIGTERM */ case 15: sig_description = (char*)cspec_vector_get(cspec->signals_vector, 5); break;
    }

    /* Craft a response for signal errors */
    cspec_string *sig = cspec_string_new("");
    cspec_string_add_str(sig, cspec_string_get(cspec->display_tab));
    cspec_string_add_str(sig, cspec_string_get(cspec->RED));
    cspec_string_add_str(sig, "✗");
    cspec_string_add_str(sig, cspec_string_get(cspec->WHITE));
    cspec_string_add_str(sig, " it ");
    cspec_string_add_str(sig, cspec_string_get(cspec->name_of_tested_proc));
    cspec_string_add_str(sig, ":\n");
    cspec_string_add_str(sig, cspec_string_get(cspec->display_tab));
    cspec_string_add_str(sig, "    ");
    cspec_string_add_str(sig, cspec->current_file);
    cspec_string_add_str(sig, ":");
    cspec_string_add_int(sig, cspec->current_line);
    cspec_string_add_str(sig, "\n");
    cspec_string_add_str(sig, cspec_string_get(cspec->display_tab));
    cspec_string_add_str(sig, "        |> ");
    cspec_string_add_str(sig, "signal: ");
    cspec_string_add_str(sig, cspec_string_get(cspec->RED));
    cspec_string_add_str(sig, sig_description);
    cspec_string_add_str(sig, cspec_string_get(cspec->WHITE));
    cspec_string_add_str(sig, " got caught on \n");
    cspec_string_add_str(sig, cspec_string_get(cspec->display_tab));
    cspec_string_add_str(sig, "          -> ");
    cspec_string_add_str(sig, cspec_string_get(cspec->RED));
    cspec_string_add_str(sig, "`");

    /* This only works for `assert_that` blocks */
    cspec_string_add_str(sig, cspec_string_get(cspec->current_assert));
    /*------------------------------------------*/

    cspec_string_add_str(sig, "`");
    cspec_string_add_str(sig, cspec_string_get(cspec->RESET));
    printf("%s\n\n", cspec_string_get(sig));
    /* signal(signal_id, cspec_signal_handler); */
    signal(signal_id, SIG_DFL);
    /* longjmp(&cspec->escape, 1); */
}

/**
 * @func: cspec_setup_test_data
 * @desc: Allocates memory for vectors to save test results in
 **/
static void cspec_setup_test_data(void) {
    /* Printf a neat intro */
    printf("\033[38;5;95m/######## ########/\n");
    printf("\033[38;5;95m/##### \033[38;5;89mc\033[38;5;90mS\033[38;5;91mp\033[38;5;92me\033[38;5;93mc\033[0m \033[38;5;95m#####/\n");
    printf("/######## ########/\033[0m\n");
    
    /* Craft the global variable struct */
    cspec = (cspec_data_struct*)malloc(sizeof(cspec_data_struct) + 1);

    cspec->number_of_tests = 0;
    cspec->number_of_passing_tests = 0;
    cspec->number_of_failing_tests = 0;
    cspec->number_of_skipped_tests = 0;
    cspec->total_time_taken_for_tests = 0;
    cspec->status_of_test = CSPEC_PASSING;
    /* jmp_buf cspec->escape; */

    cspec->test_result_message = NULL;
    cspec->name_of_tested_proc = NULL;
    cspec->name_of_describe = NULL;
    cspec->name_of_context = NULL;

    cspec->current_assert = NULL;
    cspec->current_file = NULL;
    cspec->current_line = 0;
    cspec->current_actual = NULL;
    cspec->current_expected = NULL;
    cspec->position_in_file = NULL;
    cspec->assert_result = NULL;
    cspec->type_of_tests = NULL;
    cspec->type_of_export_tests = NULL;

    /** Before and after procs **/
    cspec->before_func = NULL;
    cspec->after_func = NULL;

    cspec->display_tab = NULL;
    cspec->signals_vector = cspec_vector_new();
    cspec->inner_nest = -1;
    cspec->outer_nest = 0;
    cspec->in_context_block = false;

    /** Colors **/
    cspec->GREEN = cspec_string_new("\033[38;5;78m");
    cspec->RED = cspec_string_new("\033[1;31m");
    cspec->YELLOW = cspec_string_new("\033[38;5;11m");
    cspec->PURPLE = cspec_string_new("\033[38;5;207m");
    cspec->CYAN = cspec_string_new("\033[1;36m");
    cspec->GRAY = cspec_string_new("\033[38;5;244m");
    cspec->WHITE = cspec_string_new("\033[38;5;254m");
    cspec->RESET = cspec_string_new("\033[0m");
    cspec->BACK_PURPLE = cspec_string_new("\033[48;5;89m");

    cspec->list_of_modules = cspec_vector_new();
    cspec->list_of_describes = cspec_vector_new();
    cspec->list_of_contexts = cspec_vector_new();
    cspec->list_of_its = cspec_vector_new();
    cspec->list_of_asserts = cspec_vector_new();

    /* Signals that can be processed */
    cspec_vector_add(cspec->signals_vector, (void*)"`SIGINT | Interrupt`");
    cspec_vector_add(cspec->signals_vector, (void*)"`SIGILL | Illegal instruction`");
    cspec_vector_add(cspec->signals_vector, (void*)"`SIGABRT | Abort`");
    cspec_vector_add(cspec->signals_vector, (void*)"`SIGFPE | Floating - Point arithmetic exception`");
    cspec_vector_add(cspec->signals_vector, (void*)"`SIGSEGV | Segmentation Violation`");
    cspec_vector_add(cspec->signals_vector, (void*)"`SIGTERM | Termination`");

    /* Setup the signals */
    signal(SIGINT, cspec_signal_handler);
    signal(SIGILL, cspec_signal_handler);
    signal(SIGABRT, cspec_signal_handler);
    signal(SIGFPE, cspec_signal_handler);
    signal(SIGSEGV, cspec_signal_handler);
    signal(SIGTERM, cspec_signal_handler);
}

#endif
