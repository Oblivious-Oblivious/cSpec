/**
 * A lightweight, compile time unit testing library for TDD and BDD models,
 * heavily inspired by ruby's RSpec.
 *
 * Copyright (C) 2024 Athanasios Papapostolou (oblivious)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef __CSPEC_H_
#define __CSPEC_H_

#include <stddef.h> /* size_t, ptrdiff_t */
#include <stdio.h>  /* printf, snprintf */
#include <stdlib.h> /* malloc, calloc, realloc, free */
#include <string.h> /* strlen, strncmp, memmove */

#if defined(_WIN32)
  #include <Windows.h>
#elif defined(__unix__)
  #if defined(__clang__)
    #define CSPEC_CLOCKID 0
  #endif

  #include <time.h>

  #ifdef CLOCK_MONOTONIC
    #define CSPEC_CLOCKID CLOCK_MONOTONIC
  #endif
#elif defined(__MACH__) && defined(__APPLE__)
  #include <mach/mach.h>
  #include <mach/mach_time.h>
  #include <time.h>
#else
  #include <time.h>
#endif

/**
 * @desc: A cross platform timer function for profiling
 * @return The time in nanoseconds
 */
static size_t cspec_timer(void) {
  static size_t is_init = 0;

/* Cross platform definition */
#if defined(_WIN32)
  static LARGE_INTEGER win_frequency;
  if(0 == is_init) {
    QueryPerformanceFrequency(&win_frequency);
    is_init = 1;
  }
  LARGE_INTEGER now;
  QueryPerformanceCounter(&now);
  return (1e9 * now.QuadPart) / win_frequency.QuadPart;
#elif defined(__APPLE__)
  static mach_timebase_info_data_t info;
  size_t now;
  if(0 == is_init) {
    mach_timebase_info(&info);
    is_init = 1;
  }
  now = mach_absolute_time();
  now *= info.numer;
  now /= info.denom;
  return now;
#elif defined(__linux)
  static struct timespec linux_rate;
  size_t now;
  struct timespec spec;
  if(0 == is_init) {
    clock_getres(CSPEC_CLOCKID, &linux_rate);
    is_init = 1;
  }
  clock_gettime(CSPEC_CLOCKID, &spec);
  now = spec.tv_sec * 1.0e9 + spec.tv_nsec;
  return now;
#endif
}

/** @brief -> A 'big' enough size to hold both 1 and 0 */
#define cspec_bool  unsigned char
#define cspec_true  1
#define cspec_false 0

/**
 * @desc: A mutable string of characters used to dynamically build a string.
 * @param str -> The str char* we construct our string into
 * @param alloced -> The total sized allocated for the string
 * @param len -> The total len of the string
 */
typedef struct cspec_string {
  char *str;
  size_t alloced;
  size_t len;
} cspec_string;

/**
 * @desc: Ensure there is enough space for data being added plus a NULL
 *terminator
 * @param sb -> The string builder to use
 * @param add_len -> he len that needs to be added *not* including a NULL
 *terminator
 */
static void cspec_string_ensure_space(cspec_string *sb, size_t add_len) {
  if(sb == NULL || add_len == 0) {
    return;
  }

  /* If out allocated space is big enough */
  if(sb->alloced >= sb->len + add_len + 1) {
    return;
  }

  while(sb->alloced < sb->len + add_len + 1) {
    /* Doubling growth strategy */
    sb->alloced <<= 1;
    if(sb->alloced >= 0) {
      /* Left shift of max bits will go to 0. An unsigned type set to
       * -1 will return the maximum possible size. However, we should
       *  have run out of memory well before we need to do this. Since
       *  this is the theoretical maximum total system memory we don't
       *  have a flag saying we can't grow any more because it should
       *  be impossible to get to this point */
      sb->alloced--;
    }
  }
  sb->str = (char *)realloc(sb->str, sb->alloced);
}

/**
 * @desc: Add a string to the builder
 * @param sb -> The string builder to use
 * @param str -> The string to add
 */
static void cspec_string_add_str(cspec_string *sb, const char *str) {
  const char *ptr;
  size_t len;

  if(sb == NULL || str == NULL || *str == '\0') {
    return;
  }

  ptr = str;
  while(*ptr) {
    ++ptr;
  }
  len = ptr - str;

  cspec_string_ensure_space(sb, len);
  memmove(sb->str + sb->len, str, len);

  /* Reset len and NULL terminate */
  sb->len += len;
  sb->str[sb->len] = '\0';
}

/**
 * @desc: Create an str builder
 * @param initial_string -> The initial string to set
 * @return The str builder
 */
static cspec_string *cspec_string_new(const char *initial_string) {
  cspec_string *sb;
  sb          = (cspec_string *)calloc(1, sizeof(*sb));
  sb->str     = (char *)malloc(32);
  *sb->str    = '\0';
  sb->alloced = 32;
  sb->len     = 0;
  cspec_string_add_str(sb, initial_string);
  return sb;
}

/**
 * @desc: Add an integer to the builder
 * @param sb -> The string builder to use
 * @param val -> The integer to add
 */
static void cspec_string_add_int(cspec_string *sb, int val) {
  char str[1024];

  if(sb == NULL) {
    return;
  }

  snprintf(str, sizeof(str), "%d", val);
  cspec_string_add_str(sb, str);
}

static void cspec_string_add_size_t(cspec_string *sb, size_t val) {
  char str[1024];

  if(sb == NULL) {
    return;
  }

  snprintf(str, sizeof(str), "%zu", val);
  cspec_string_add_str(sb, str);
}

/**
 * @desc: Add a double to the builder
 * @param sb -> The string builder to use
 * @param val -> The double to add
 */
static void cspec_string_add_double_precision(cspec_string *sb, double val) {
  char str[1024];

  if(sb == NULL) {
    return;
  }

  /* Use %g for minimum precision on printing floats */
  snprintf(str, sizeof(str), "%g", val);
  cspec_string_add_str(sb, str);
}

/**
 * @desc: A pointer to the internal buffer with the builder's stirng data
 * @param sb -> The string builder to use
 * @return A pointer to the internal cspec_string data
 */
static char *cspec_string_get(cspec_string *sb) {
  if(sb == NULL) {
    return NULL;
  }
  return sb->str;
}

/**
 * @desc: Remove data from the end of the builder
 * @param sb -> The string builder to use
 * @param len -> The new len of the string, anything after this len is removed
 */
static void cspec_string_shorten(cspec_string *sb, size_t len) {
  if(sb == NULL || len >= sb->len) {
    return;
  }

  /* Reset the len and NULL terminate, ingoring
      all values right to the NULL from memory */
  sb->len          = len;
  sb->str[sb->len] = '\0';
}

/**
 * @desc: Clear the builder
 * @param sb -> The string builder to use
 */
static void cspec_string_delete(cspec_string *sb) {
  if(sb == NULL) {
    return;
  }
  cspec_string_shorten(sb, 0);
}

/**
 * @desc: Remove data from the beginning of the builder
 * @param sb -> The cspec_string builder to use
 * @param len -> The len to remove
 */
static void cspec_string_skip(cspec_string *sb, size_t len) {
  if(sb == NULL || len == 0) {
    return;
  }

  if(len >= sb->len) {
    /* If we choose to drop more bytes than the
        string has simply clear the string */
    cspec_string_delete(sb);
    return;
  }

  sb->len -= len;

  /* +1 to move the NULL. */
  memmove(sb->str, sb->str + len, sb->len + 1);
}

/**
 * @brief Abs for floats
 * @param value -> The value to get `abs` for
 * @return Absolute value
 */
static double cspec_fabs(double value) { return value < 0 ? -value : value; }

/**
 * @brief A simple function definition for running test suites
 * @param type_of_tests -> passing|failing|skipped|all
 * @param ... -> The block of modules to run
 */
#define cspec_run_suite(type_of_tests, ...)             \
  do {                                                  \
    if(strncmp(type_of_tests, "passing", 7) &&          \
       strncmp(type_of_tests, "failing", 7) &&          \
       strncmp(type_of_tests, "skipped", 7) &&          \
       strncmp(type_of_tests, "all", 3)) {              \
      printf("\n\033[1;31mInput a type of test to log " \
             "passing|failing|skipped|all\033[0m\n\n"); \
    } else {                                            \
      cspec_setup_test_data(type_of_tests);             \
      __VA_ARGS__;                                      \
      cspec_report_time_taken_for_tests();              \
    }                                                   \
  } while(0)

/**
 * @brief Expands to a function definition of the test suite
 * @param suite_name -> The name for the new module of tests
 * @param ... -> The block to define
 */
#define module(suite_name, ...)                \
  /* Define a static method for a module */    \
  static void suite_name(void) {               \
    printf(                                    \
      "\n%s%sModule `%s`%s\n",                 \
      cspec->BACK_PURPLE,                      \
      cspec->YELLOW,                           \
      #suite_name,                             \
      cspec->RESET                             \
    );                                         \
    cspec->display_tab = cspec_string_new(""); \
    __VA_ARGS__; /* Run describes */           \
  }

/**
 * @brief Expands to a setup proc that gets executed before the tests
 * @param ... -> The proc to run
 */
#define before(...) __VA_ARGS__

/**
 * @brief Expands to a teardown proc that gets executed after the tests
 * @param ... -> The proc to run
 */
#define after(...) __VA_ARGS__

/**
 * @brief Sets the argument to a function to run before each it block
 * @param func -> The func to set
 */
#define before_each(func) cspec->before_func = func

/**
 * @brief Sets the argument to a function to run after each it block
 * @param func -> The func to set
 */
#define after_each(func) cspec->after_func = func

#define cspec_describe_context_block(object_name, color, ...) \
  do {                                                        \
    cspec_string_add_str(cspec->display_tab, "    ");         \
    printf(                                                   \
      "%s%s`%s`%s\n",                                         \
      cspec_string_get(cspec->display_tab),                   \
      color,                                                  \
      object_name,                                            \
      cspec->RESET                                            \
    );                                                        \
    __VA_ARGS__;                                              \
    cspec_string_skip(cspec->display_tab, 4);                 \
  } while(0)

/**
 * @brief Expands to a block execution where tests are encompassed
 * @param object_name -> The name of the unit to describe
 * @param ... -> The proc to extend to
 */
#define describe(object_name, ...) \
  cspec_describe_context_block(object_name, cspec->PURPLE, __VA_ARGS__)

/**
 * @brief Aliasing for Describe
 * @param object_name -> The name of the unit to describe
 * @param ... -> The proc to extend to
 */
#define context(object_name, ...) \
  cspec_describe_context_block(object_name, cspec->YELLOW, __VA_ARGS__)

/**
 * @brief Temporarily disables a test
 * @param proc_name -> The name of test to run
 * @param ... -> The actual test code
 */
#define xit(proc_name, ...)                            \
  do {                                                 \
    if(cspec->before_func) {                           \
      (*cspec->before_func)();                         \
    }                                                  \
    cspec_string_add_str(cspec->display_tab, "    ");  \
                                                       \
    cspec->number_of_tests++;                          \
    cspec->number_of_skipped_tests++;                  \
    cspec->test_result_message = cspec_string_new(""); \
    if(!strncmp(cspec->type_of_tests, "all", 3) ||     \
       !strncmp(cspec->type_of_tests, "skipped", 7)) { \
      printf(                                          \
        "%s%s- %s%s\n",                                \
        cspec_string_get(cspec->display_tab),          \
        cspec->GRAY,                                   \
        proc_name,                                     \
        cspec->RESET                                   \
      );                                               \
    }                                                  \
                                                       \
    cspec_string_skip(cspec->display_tab, 4);          \
    if(cspec->after_func) {                            \
      (*cspec->after_func)();                          \
    }                                                  \
  } while(0)

/**
 * @brief Expands to a test run and saves all data gathered
 * @param proc_name -> The name of test to run
 * @param proc -> The actual test code
 */
#define it(proc_name, ...)                                                  \
  do {                                                                      \
    size_t start_test_timer;                                                \
    size_t end_test_timer;                                                  \
    if(cspec->before_func) {                                                \
      (*cspec->before_func)();                                              \
    }                                                                       \
                                                                            \
    cspec_string_add_str(cspec->display_tab, "    ");                       \
    cspec->number_of_tests++;                                               \
    cspec->test_result_message = cspec_string_new("");                      \
                                                                            \
    /* Assume its a passing test */                                         \
    cspec->status_of_test = CSPEC_PASSING;                                  \
    cspec->current_line   = __LINE__;                                       \
    cspec->current_file   = __FILE__;                                       \
                                                                            \
    start_test_timer = cspec_timer();                                       \
    __VA_ARGS__;                                                            \
    end_test_timer = cspec_timer();                                         \
                                                                            \
    if(cspec->status_of_test == CSPEC_PASSING) {                            \
      cspec->number_of_passing_tests++;                                     \
      if(!strncmp(cspec->type_of_tests, "all", 3) ||                        \
         !strncmp(cspec->type_of_tests, "passing", 7)) {                    \
        printf(                                                             \
          "%s%s✓%s it %s%s\n",                                              \
          cspec_string_get(cspec->display_tab),                             \
          cspec->GREEN,                                                     \
          cspec->RESET,                                                     \
          proc_name,                                                        \
          cspec->RESET                                                      \
        );                                                                  \
      }                                                                     \
    } else {                                                                \
      /* Even if 1 of the asserts in the current it block fails, assume we  \
       * have a failing test */                                             \
      cspec->number_of_failing_tests++;                                     \
      if(!strncmp(cspec->type_of_tests, "all", 3) ||                        \
         !strncmp(cspec->type_of_tests, "failing", 7)) {                    \
        printf(                                                             \
          "%s%s✗%s it %s:\n%s%s\n",                                         \
          cspec_string_get(cspec->display_tab),                             \
          cspec->RED,                                                       \
          cspec->RESET,                                                     \
          proc_name,                                                        \
          cspec_string_get(cspec->test_result_message),                     \
          cspec->RESET                                                      \
        );                                                                  \
      }                                                                     \
    }                                                                       \
                                                                            \
    cspec->total_time_taken_for_tests += end_test_timer - start_test_timer; \
    cspec_string_skip(cspec->display_tab, 4);                               \
    if(cspec->after_func) {                                                 \
      (*cspec->after_func)();                                               \
    }                                                                       \
  } while(0)

/**
 * @brief Global variables grouped in container
 * @param number_of_tests -> The total number of tests performed
 * @param number_of_passing_tests -> Counts the passing tests
 * @param number_of_failing_tests -> Counts the failing tests
 * @param number_of_skipped_tests -> Counts the skipped tests
 * @param total_time_taken_for_tests -> The total time taken for tests
 * @param status_of_test -> Either CSPEC_PASSING|CSPEC_FAILING
 *
 * @param test_result_message -> The string builder we construct for assertions
 * @param display_tab -> 4 space overhead for a nicer display of test results
 *
 * @param type_of_tests -> The type of tests we want to display
 * @param current_file -> Current __FILE__ used for tracking assert positions
 * @param current_line -> Current value of the __LINE__ macro
 * @param current_actual -> Current actual value token
 * @param current_expected -> Current expected value token
 * @param position_in_file -> A string containing __FILE__ and __LINE__ results
 * @param assert_result -> Result description of the current assert
 *
 * @param before_func -> A function pointer to be executed before it blocks
 * @param after_func -> A function pointer to be executed after it blocks
 *
 * @param COLORS -> Terminal string color codes
 */
typedef struct cspec_data_struct {
  size_t number_of_tests;
  size_t number_of_passing_tests;
  size_t number_of_failing_tests;
  size_t number_of_skipped_tests;
  size_t total_time_taken_for_tests;
  cspec_bool status_of_test;

  cspec_string *test_result_message;
  cspec_string *display_tab;

  const char *type_of_tests;
  const char *current_file;
  size_t current_line;
  cspec_string *current_actual;
  cspec_string *current_expected;
  cspec_string *position_in_file;
  cspec_string *assert_result;

  void (*before_func)(void);
  void (*after_func)(void);

  const char *GREEN;
  const char *RED;
  const char *YELLOW;
  const char *PURPLE;
  const char *CYAN;
  const char *GRAY;
  const char *WHITE;
  const char *RESET;
  const char *BACK_PURPLE;
} cspec_data_struct;

static cspec_data_struct *cspec;

/**
 * @param CSPEC_PASSING -> Set for passing tests
 * @param CSPEC_FAILING -> Set for failing tests
 */
#define CSPEC_PASSING cspec_true
#define CSPEC_FAILING cspec_false

#define is    ==
#define isnot !=

#define equals ,
#define to
#define with
#define array_size ,

/**
 * @brief Defines a compile time assertion for extended data types
 * @param name_of_assert -> The name of the new assertion
 * @param data_type_token -> The data type of the input variables
 * @param to_string_method -> Custom way to write data type as a string
 * @param comparison_method -> Custom way of comparing new data types for
 *asserts
 */
#define cspec_define_assert(                                           \
  name_of_assert, data_type_token, to_string_method, comparison_method \
)                                                                      \
  static void name_of_assert(                                          \
    data_type_token actual, data_type_token expected                   \
  ) {                                                                  \
    to_string_method(actual, expected);                                \
    if(comparison_method(actual, expected)) {                          \
      cspec->status_of_test = CSPEC_FAILING;                           \
      cspec_write_position_in_file();                                  \
      cspec_write_assert_actual_expected();                            \
    }                                                                  \
  }

#define cspec_define_assert_array(                                          \
  name_of_assert, data_type_token, to_string_method, comparison_method, len \
)                                                                           \
  static void name_of_assert(                                               \
    data_type_token actual, data_type_token expected, size_t len            \
  ) {                                                                       \
    to_string_method(actual, expected, len);                                \
    if(comparison_method(actual, expected, len)) {                          \
      cspec->status_of_test = CSPEC_FAILING;                                \
      cspec_write_position_in_file();                                       \
      cspec_write_assert_actual_expected();                                 \
    }                                                                       \
  }

/**
 * @brief Setup file position and line number for the current assert
 */
static void cspec_write_position_in_file(void) {
  cspec->position_in_file = cspec_string_new("");
  cspec_string_add_str(cspec->position_in_file, cspec->current_file);
  cspec_string_add_str(cspec->position_in_file, ":");
  cspec_string_add_int(cspec->position_in_file, cspec->current_line);
  cspec_string_add_str(cspec->position_in_file, ":");
}

/**
 * @brief Setup the assert description for printing and exporting
 */
static void cspec_write_assert_actual_expected(void) {
  cspec->assert_result = cspec_string_new("");
  cspec_string_add_str(
    cspec->test_result_message, cspec_string_get(cspec->display_tab)
  );
  cspec_string_add_str(cspec->test_result_message, cspec->RESET);
  cspec_string_add_str(cspec->test_result_message, "    ");
  cspec_string_add_str(
    cspec->test_result_message, cspec_string_get(cspec->position_in_file)
  );
  cspec_string_add_str(cspec->test_result_message, "\n");
  cspec_string_add_str(
    cspec->test_result_message, cspec_string_get(cspec->display_tab)
  );
  cspec_string_add_str(cspec->test_result_message, "        |> ");
  cspec_string_add_str(cspec->test_result_message, "`");
  cspec_string_add_str(
    cspec->test_result_message, cspec_string_get(cspec->current_expected)
  );
  cspec_string_add_str(cspec->test_result_message, "` expected but got ");
  cspec_string_add_str(cspec->test_result_message, cspec->RED);
  cspec_string_add_str(cspec->test_result_message, "`");
  cspec_string_add_str(
    cspec->test_result_message, cspec_string_get(cspec->current_actual)
  );
  cspec_string_add_str(cspec->test_result_message, "`\n");
  cspec_string_add_str(cspec->test_result_message, cspec->RESET);
  cspec_string_add_str(cspec->assert_result, "|> `");
  cspec_string_add_str(
    cspec->assert_result, cspec_string_get(cspec->current_expected)
  );
  cspec_string_add_str(cspec->assert_result, "` expected but got ");
  cspec_string_add_str(cspec->assert_result, "`");
  cspec_string_add_str(
    cspec->assert_result, cspec_string_get(cspec->current_actual)
  );
}

/**
 * @brief Report the number of tests and time taken while testing
 */
static void cspec_report_time_taken_for_tests(void) {
  printf(
    "\n%s● %zu tests\n%s✓ %zu passing\n%s✗ %zu failing\n%s- %zu "
    "skipped%s\n",
    cspec->YELLOW,
    cspec->number_of_tests,
    cspec->GREEN,
    cspec->number_of_passing_tests,
    cspec->RED,
    cspec->number_of_failing_tests,
    cspec->GRAY,
    cspec->number_of_skipped_tests,
    cspec->RESET
  );

  /* Print in seconds if the time is more than 100ms */
  if(cspec->total_time_taken_for_tests > 100000000) {
    printf(
      "%s★ Finished in %.5f seconds%s\n",
      cspec->CYAN,
      cspec->total_time_taken_for_tests / 1000000000.0,
      cspec->RESET
    );
  }
  /* Else print in miliseconds */
  else {
    printf(
      "%s★ Finished in %.5f ms%s\n",
      cspec->CYAN,
      cspec->total_time_taken_for_tests / 1000000.0,
      cspec->RESET
    );
  }
}

/**
 * @brief Allocates memory for vectors to save test results in
 */
static void cspec_setup_test_data(const char *type_of_tests) {
  printf("\033[38;5;95m/######## ########/\n");
  printf("\033[38;5;95m/##### "
         "\033[38;5;89mc\033[38;5;90mS\033[38;5;91mp\033[38;5;92me\033[38;5;"
         "93mc\033[0m \033[38;5;95m#####/\n");
  printf("/######## ########/\033[0m\n");

  cspec = (cspec_data_struct *)malloc(sizeof(cspec_data_struct));

  cspec->number_of_tests            = 0;
  cspec->number_of_passing_tests    = 0;
  cspec->number_of_failing_tests    = 0;
  cspec->number_of_skipped_tests    = 0;
  cspec->total_time_taken_for_tests = 0;
  cspec->status_of_test             = CSPEC_PASSING;

  cspec->test_result_message = NULL;

  cspec->current_file     = NULL;
  cspec->current_line     = 0;
  cspec->current_actual   = NULL;
  cspec->current_expected = NULL;
  cspec->position_in_file = NULL;
  cspec->assert_result    = NULL;
  cspec->type_of_tests    = type_of_tests;

  cspec->before_func = NULL;
  cspec->after_func  = NULL;

  cspec->display_tab = NULL;

  cspec->GREEN       = "\033[38;5;78m";
  cspec->RED         = "\033[1;31m";
  cspec->YELLOW      = "\033[38;5;11m";
  cspec->PURPLE      = "\033[38;5;207m";
  cspec->CYAN        = "\033[1;36m";
  cspec->GRAY        = "\033[38;5;244m";
  cspec->RESET       = "\033[0m";
  cspec->BACK_PURPLE = "\033[48;5;89m";
}

/**
 * @brief Writes actual and expected values
 * @param actual -> The value passed by the user
 * @param expected -> The value `actual` is tested against
 * @param len -> The length of the arrays
 */
static void cspec_to_string_charptr_array_write(
  char **actual, char **expected, size_t len
) {
  size_t i;
  cspec->current_actual   = cspec_string_new("[");
  cspec->current_expected = cspec_string_new("[");

  for(i = 0; i < len - 1; i++) {
    cspec_string_add_str(cspec->current_actual, actual[i]);
    cspec_string_add_str(cspec->current_actual, ", ");

    cspec_string_add_str(cspec->current_expected, expected[i]);
    cspec_string_add_str(cspec->current_expected, ", ");
  }
  cspec_string_add_str(cspec->current_actual, actual[len - 1]);
  cspec_string_add_str(cspec->current_actual, "]");

  cspec_string_add_str(cspec->current_expected, expected[len - 1]);
  cspec_string_add_str(cspec->current_expected, "]");
}

/**
 * @brief Compares two string arrays for equality in elements
 * @param actual -> The value passed by the user
 * @param expected -> The value `actual` is tested against
 * @param len -> The length of the arrays
 * @return a boolean
 */
static cspec_bool
cspec_charptr_array_comparison(char **actual, char **expected, size_t len) {
  size_t i;
  for(i = 0; i < len; i++) {
    if(strncmp(actual[i], expected[i], strlen(expected[i]))) {
      return cspec_false;
    }
  }

  return cspec_true;
}

/**
 * @brief Assertion of two char* arrays
 */
cspec_define_assert_array(
  cspec_call_assert_that_charptr_array,
  char **,
  cspec_to_string_charptr_array_write,
  !cspec_charptr_array_comparison,
  len
)

  /**
   * @brief Negative assertion of two char* arrays
   */
  cspec_define_assert_array(
    cspec_call_nassert_that_charptr_array,
    char **,
    cspec_to_string_charptr_array_write,
    cspec_charptr_array_comparison,
    len
  )

/**
 * @brief Assert that the expected charptr array equals to the result
 * @param actual -> The actual value
 * @param expected -> The expected value
 */
#define assert_that_charptr_array(inner)         \
  do {                                           \
    cspec->current_file = __FILE__;              \
    cspec->current_line = __LINE__;              \
    cspec_call_assert_that_charptr_array(inner); \
  } while(0)

/**
 * @brief Assert that the expected charptr array differs from the result
 * @param actual -> The actual value
 * @param expected -> The expected value
 */
#define nassert_that_charptr_array(inner)         \
  do {                                            \
    cspec->current_file = __FILE__;               \
    cspec->current_line = __LINE__;               \
    cspec_call_nassert_that_charptr_array(inner); \
  } while(0)

  /**
   * @brief Writes actual and expected values
   * @param actual -> The value passed by the user
   * @param expected -> The value `actual` is tested against
   */
  static void cspec_to_string_charptr_write(
    const char *actual, const char *expected
  ) {
  cspec->current_actual   = cspec_string_new(actual);
  cspec->current_expected = cspec_string_new(expected);
}

/**
 * @brief A function that compares char pointers for assertions
 * @param actual -> The value passed by the user
 * @param expected -> The value `actual` is tested against
 * @return a boolean
 */
static cspec_bool
cspec_charptr_comparison(const char *actual, const char *expected) {
  return !strncmp(expected, actual, strlen(expected));
}

/**
 * @brief Assert that the expected string is equal to the result
 * @param actual -> The value passed by the user
 * @param expected -> The value `actual` is tested against
 */
cspec_define_assert(
  cspec_call_assert_that_charptr,
  const char *,
  cspec_to_string_charptr_write,
  !cspec_charptr_comparison
)

  /**
   * @brief Assert that the expected string is different than the result
   * @param actual -> The value passed by the user
   * @param expected -> The value `actual` is tested against
   */
  cspec_define_assert(
    cspec_call_nassert_that_charptr,
    const char *,
    cspec_to_string_charptr_write,
    cspec_charptr_comparison
  )

/**
 * @brief Assert that the expected string is equal to the result
 * @param actual -> The actual value
 * @param expected -> The expected string
 */
#define assert_that_charptr(inner)         \
  do {                                     \
    cspec->current_file = __FILE__;        \
    cspec->current_line = __LINE__;        \
    cspec_call_assert_that_charptr(inner); \
  } while(0)

/**
 * @brief Assert that the expected string is different than the result
 * @param actual -> The actual value
 * @param expected -> The expected string
 */
#define nassert_that_charptr(inner)         \
  do {                                      \
    cspec->current_file = __FILE__;         \
    cspec->current_line = __LINE__;         \
    cspec_call_nassert_that_charptr(inner); \
  } while(0)

  /**
   * @brief Writes the actual and expected values
   * @param actual -> The value passed by the user
   * @param expected -> The value `actual` is tested against
   * @param len -> The length of the arrays
   */
  static void cspec_to_string_double_array_write(
    double *actual, double *expected, size_t len
  ) {
  size_t i;

  cspec->current_actual   = cspec_string_new("[");
  cspec->current_expected = cspec_string_new("[");

  for(i = 0; i < len - 1; i++) {
    cspec_string_add_double_precision(cspec->current_actual, actual[i]);
    cspec_string_add_str(cspec->current_actual, ", ");

    cspec_string_add_double_precision(cspec->current_expected, expected[i]);
    cspec_string_add_str(cspec->current_expected, ", ");
  }
  cspec_string_add_double_precision(cspec->current_actual, actual[len - 1]);
  cspec_string_add_str(cspec->current_actual, "]");

  cspec_string_add_double_precision(cspec->current_expected, expected[len - 1]);
  cspec_string_add_str(cspec->current_expected, "]");
}

/**
 * @brief Conpares two double arrays for equality in elements
 * @param actual -> The value passed by the user
 * @param expected -> The value `actual` is tested against
 * @param len -> The length of the arrays
 * @return a boolean
 */
static cspec_bool
cspec_double_array_comparison(double *actual, double *expected, size_t len) {
  size_t i;
  for(i = 0; i < len; i++) {
    if(cspec_fabs(actual[i] - expected[i]) > 1E-12) {
      return cspec_false;
    }
  }

  return cspec_true;
}

/**
 * @brief Assertion of two double arrays
 */
cspec_define_assert_array(
  cspec_call_assert_that_double_array,
  double *,
  cspec_to_string_double_array_write,
  !cspec_double_array_comparison,
  len
)

  /**
   * @brief Negative assertion of two double arrays
   */
  cspec_define_assert_array(
    cspec_call_nassert_that_double_array,
    double *,
    cspec_to_string_double_array_write,
    cspec_double_array_comparison,
    len
  )

/**
 * @brief Assert that the expected double array equals to the result
 * @param actual -> The actual value
 * @param expected -> The expected value
 */
#define assert_that_double_array(inner)         \
  do {                                          \
    cspec->current_file = __FILE__;             \
    cspec->current_line = __LINE__;             \
    cspec_call_assert_that_double_array(inner); \
  } while(0)

/**
 * @brief Assert that the expected double array differs from the result
 * @param actual -> The actual value
 * @param expected -> The expected value
 */
#define nassert_that_double_array(inner)         \
  do {                                           \
    cspec->current_file = __FILE__;              \
    cspec->current_line = __LINE__;              \
    cspec_call_nassert_that_double_array(inner); \
  } while(0)

  /**
   * @brief Writes actual and expected values
   * @param actual -> The value passed by the user
   * @param expected -> The value `actual` is tested against
   */
  static void cspec_to_string_double_write(double actual, double expected) {
  cspec->current_actual   = cspec_string_new("");
  cspec->current_expected = cspec_string_new("");
  cspec_string_add_double_precision(cspec->current_actual, actual);
  cspec_string_add_double_precision(cspec->current_expected, expected);
}

/**
 * @brief A function that compares doubles for assertions
 * @param actual -> The value passed by the user
 * @param expected -> The value `actual` is tested against
 * @return a boolean
 */
static cspec_bool cspec_double_comparison(double actual, double expected) {
  /* Calculate margin to which the difference is too big so the test fails */
  return cspec_fabs(actual - expected) < 1E-12;
}

/**
 * @brief Assert that the expected double is different than the result
 * @param actual -> The value passed by the user
 * @param expected -> The value `actual` is tested against
 */
cspec_define_assert(
  cspec_call_assert_that_double,
  double,
  cspec_to_string_double_write,
  !cspec_double_comparison
)

  /**
   * @brief Assert that the expected double is different than the result
   * @param actual -> The value passed by the user
   * @param expected -> The value `actual` is tested against
   */
  cspec_define_assert(
    cspec_call_nassert_that_double,
    double,
    cspec_to_string_double_write,
    cspec_double_comparison
  )

/**
 * @brief Assert that the expected double is different than the result
 * @param actual -> The actual value
 * @param expected -> The expected double
 */
#define assert_that_double(inner)         \
  do {                                    \
    cspec->current_file = __FILE__;       \
    cspec->current_line = __LINE__;       \
    cspec_call_assert_that_double(inner); \
  } while(0)

/**
 * @brief Assert that the expected double is different than the result
 * @param actual -> The actual value
 * @param expected -> The expected double
 */
#define nassert_that_double(inner)         \
  do {                                     \
    cspec->current_file = __FILE__;        \
    cspec->current_line = __LINE__;        \
    cspec_call_nassert_that_double(inner); \
  } while(0)

/**
 * @brief Asserts that a proc returns true
 * @param test -> The test proc to run
 */
#define assert_that(test)                                                     \
  do {                                                                        \
    cspec->current_file     = __FILE__;                                       \
    cspec->current_line     = __LINE__;                                       \
    cspec->position_in_file = cspec_string_new("");                           \
    cspec->assert_result    = cspec_string_new("");                           \
                                                                              \
    if(!(test)) {                                                             \
      cspec->status_of_test = CSPEC_FAILING;                                  \
      cspec_write_position_in_file();                                         \
                                                                              \
      cspec_string_add_str(                                                   \
        cspec->test_result_message, cspec_string_get(cspec->display_tab)      \
      );                                                                      \
      cspec_string_add_str(cspec->test_result_message, cspec->RESET);         \
      cspec_string_add_str(cspec->test_result_message, "    ");               \
      cspec_string_add_str(                                                   \
        cspec->test_result_message, cspec_string_get(cspec->position_in_file) \
      );                                                                      \
      cspec_string_add_str(cspec->test_result_message, "\n");                 \
      cspec_string_add_str(                                                   \
        cspec->test_result_message, cspec_string_get(cspec->display_tab)      \
      );                                                                      \
      cspec_string_add_str(cspec->test_result_message, "        |> ");        \
      cspec_string_add_str(cspec->test_result_message, cspec->RED);           \
      cspec_string_add_str(cspec->test_result_message, "`");                  \
      cspec_string_add_str(cspec->test_result_message, #test);                \
      cspec_string_add_str(cspec->test_result_message, "`");                  \
      cspec_string_add_str(cspec->test_result_message, cspec->RESET);         \
      cspec_string_add_str(cspec->test_result_message, " should be true\n");  \
      cspec_string_add_str(cspec->test_result_message, cspec->RESET);         \
      cspec_string_add_str(cspec->assert_result, "|> `");                     \
      cspec_string_add_str(cspec->assert_result, #test);                      \
      cspec_string_add_str(cspec->assert_result, "`");                        \
      cspec_string_add_str(cspec->assert_result, " should be true");          \
    }                                                                         \
  } while(0)

/**
 * @brief Asserts that a proc returns false
 * @param test -> The test proc to run
 */
#define nassert_that(test)                                                    \
  do {                                                                        \
    cspec->current_file     = __FILE__;                                       \
    cspec->current_line     = __LINE__;                                       \
    cspec->position_in_file = cspec_string_new("");                           \
    cspec->assert_result    = cspec_string_new("");                           \
                                                                              \
    if((test)) {                                                              \
      cspec->status_of_test = CSPEC_FAILING;                                  \
      cspec_write_position_in_file();                                         \
                                                                              \
      cspec_string_add_str(                                                   \
        cspec->test_result_message, cspec_string_get(cspec->display_tab)      \
      );                                                                      \
      cspec_string_add_str(cspec->test_result_message, cspec->RESET);         \
      cspec_string_add_str(cspec->test_result_message, "    ");               \
      cspec_string_add_str(                                                   \
        cspec->test_result_message, cspec_string_get(cspec->position_in_file) \
      );                                                                      \
      cspec_string_add_str(cspec->test_result_message, "\n");                 \
      cspec_string_add_str(                                                   \
        cspec->test_result_message, cspec_string_get(cspec->display_tab)      \
      );                                                                      \
      cspec_string_add_str(cspec->test_result_message, "        |> ");        \
      cspec_string_add_str(cspec->test_result_message, cspec->RED);           \
      cspec_string_add_str(cspec->test_result_message, "`");                  \
      cspec_string_add_str(cspec->test_result_message, #test);                \
      cspec_string_add_str(cspec->test_result_message, "`");                  \
      cspec_string_add_str(cspec->test_result_message, cspec->RESET);         \
      cspec_string_add_str(cspec->test_result_message, " should be false\n"); \
      cspec_string_add_str(cspec->test_result_message, cspec->RESET);         \
      cspec_string_add_str(cspec->assert_result, "|> `");                     \
      cspec_string_add_str(cspec->assert_result, #test);                      \
      cspec_string_add_str(cspec->assert_result, "`");                        \
      cspec_string_add_str(cspec->assert_result, " should be false");         \
    }                                                                         \
  } while(0)

  /**
   * @brief Writes actual and expected values
   * @param actual -> The value passed by the user
   * @param expected -> The value `actual` is tested against
   * @param len -> The length of the arrays
   */
  static void cspec_to_string_int_array_write(
    int *actual, int *expected, size_t len
  ) {
  size_t i;

  cspec->current_actual   = cspec_string_new("[");
  cspec->current_expected = cspec_string_new("[");

  for(i = 0; i < len - 1; i++) {
    cspec_string_add_int(cspec->current_actual, actual[i]);
    cspec_string_add_str(cspec->current_actual, ", ");

    cspec_string_add_int(cspec->current_expected, expected[i]);
    cspec_string_add_str(cspec->current_expected, ", ");
  }
  cspec_string_add_int(cspec->current_actual, actual[len - 1]);
  cspec_string_add_str(cspec->current_actual, "]");

  cspec_string_add_int(cspec->current_expected, expected[len - 1]);
  cspec_string_add_str(cspec->current_expected, "]");
}

/**
 * @brief Compares two int arrays for equality in elements
 * @param actual -> The value passed by the user
 * @param expected -> The value `actual` is tested against
 * @param len -> The length of the arrays
 * @return a boolean
 */
static cspec_bool
cspec_int_array_comparison(int *actual, int *expected, size_t len) {
  size_t i;
  for(i = 0; i < len; i++) {
    if(actual[i] != expected[i]) {
      return cspec_false;
    }
  }

  return cspec_true;
}

/**
 * @brief Assertion of two int arrays
 */
cspec_define_assert_array(
  cspec_call_assert_that_int_array,
  int *,
  cspec_to_string_int_array_write,
  !cspec_int_array_comparison,
  len
)

  /**
   * @brief Negative assertion of two int arrays
   */
  cspec_define_assert_array(
    cspec_call_nassert_that_int_array,
    int *,
    cspec_to_string_int_array_write,
    cspec_int_array_comparison,
    len
  )

/**
 * @brief Assert that the expected int array equals to the the result
 * @param actual -> The actual value
 * @param expected -> The expected value
 */
#define assert_that_int_array(inner)         \
  do {                                       \
    cspec->current_file = __FILE__;          \
    cspec->current_line = __LINE__;          \
    cspec_call_assert_that_int_array(inner); \
  } while(0)

/**
 * @brief Assert that the expected int array differs from the result
 * @param actual -> The actual value
 * @param expected -> The expected value
 */
#define nassert_that_int_array(inner)         \
  do {                                        \
    cspec->current_file = __FILE__;           \
    cspec->current_line = __LINE__;           \
    cspec_call_nassert_that_int_array(inner); \
  } while(0)

  /**
   * @brief Writes actual and expected values
   * @param actual -> The value passed by the user
   * @param expected -> The value `actual` is tested against
   */
  static void cspec_to_string_int_write(int actual, int expected) {
  cspec->current_actual   = cspec_string_new("");
  cspec->current_expected = cspec_string_new("");
  cspec_string_add_int(cspec->current_actual, actual);
  cspec_string_add_int(cspec->current_expected, expected);
}

static void cspec_to_string_size_t_write(size_t actual, size_t expected) {
  cspec->current_actual   = cspec_string_new("");
  cspec->current_expected = cspec_string_new("");
  cspec_string_add_size_t(cspec->current_actual, actual);
  cspec_string_add_size_t(cspec->current_expected, expected);
}

/**
 * @brief A function that compares integers for assertions
 * @param actual -> The value passed by the user
 * @param expected -> The value `actual` is tested against
 * @return a boolean
 */
static cspec_bool cspec_int_comparison(int actual, int expected) {
  return actual == expected;
}

static cspec_bool cspec_size_t_comparison(size_t actual, size_t expected) {
  return actual == expected;
}

/**
 * @brief Assert that the expected integer is equal to the result
 * @param actual -> The value passed by the user
 * @param expected -> The value `actual` is tested against
 */
cspec_define_assert(
  cspec_call_assert_that_int,
  int,
  cspec_to_string_int_write,
  !cspec_int_comparison
)

  /**
   * @brief Assert that the expected integer is different than the result
   * @param actual -> The value passed by the user
   * @param expected -> The value `actual` is tested against
   */
  cspec_define_assert(
    cspec_call_nassert_that_int,
    int,
    cspec_to_string_int_write,
    cspec_int_comparison
  )

    cspec_define_assert(
      cspec_call_assert_that_size_t,
      size_t,
      cspec_to_string_size_t_write,
      !cspec_size_t_comparison
    )

      cspec_define_assert(
        cspec_call_nassert_that_size_t,
        size_t,
        cspec_to_string_size_t_write,
        cspec_size_t_comparison
      )

/**
 * @brief Assert that the expected integer is equal to the result
 * @param actual -> The actual value
 * @param expected -> The expected int
 */
#define assert_that_int(inner)         \
  do {                                 \
    cspec->current_file = __FILE__;    \
    cspec->current_line = __LINE__;    \
    cspec_call_assert_that_int(inner); \
  } while(0)

/**
 * @brief Assert that the expected integer is different than the result
 * @param actual -> The actual value
 * @param expected -> The expected int
 */
#define nassert_that_int(inner)         \
  do {                                  \
    cspec->current_file = __FILE__;     \
    cspec->current_line = __LINE__;     \
    cspec_call_nassert_that_int(inner); \
  } while(0)

/**
 * @brief Assert that the expected size_t is equal to the result
 * @param actual -> The actual value
 * @param expected -> The expected size_t
 */
#define assert_that_size_t(inner)         \
  do {                                    \
    cspec->current_file = __FILE__;       \
    cspec->current_line = __LINE__;       \
    cspec_call_assert_that_size_t(inner); \
  } while(0)

/**
 * @brief Assert that the expected size_t is different than the result
 * @param actual -> The actual value
 * @param expected -> The expected size_t
 */
#define nassert_that_size_t(inner)         \
  do {                                     \
    cspec->current_file = __FILE__;        \
    cspec->current_line = __LINE__;        \
    cspec_call_nassert_that_size_t(inner); \
  } while(0)

#undef CSPEC_CLOCKID
#undef cspec_define_assert
#undef cspec_define_assert_array

#endif
