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
#include <stdlib.h> /* malloc, realloc */
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

typedef struct {
  size_t size;
  size_t capacity;
} _cspec_vector_header;

#define _cspec_vector_get_header(self)   ((_cspec_vector_header *)(self) - 1)
#define _cspec_vector_selfptr_size(self) sizeof(*(self)) // NOLINT
#define _cspec_vector_grow(self, n) \
  (*(void **)&(self) =              \
     _cspec_vector_growf((self), _cspec_vector_selfptr_size(self), (n), 0))
#define _cspec_vector_maybegrow(self, n)                    \
  ((!(self) || _cspec_vector_get_header(self)->size + (n) > \
                 _cspec_vector_get_header(self)->capacity)  \
     ? (_cspec_vector_grow(self, n), 0)                     \
     : 0)
#define cspec_vector_initialize(self) _cspec_vector_maybegrow(self, 1)
#define cspec_vector_add_n(self, item, n)             \
  ((item) != NULL ? _cspec_vector_maybegrow(self, n), \
   memmove(                                           \
     (self) + cspec_vector_size(self),                \
     (item),                                          \
     (n) * ((item) != NULL ? sizeof((self)[0]) : 0)   \
   ),                                                 \
   _cspec_vector_get_header(self)->size += (n)        \
                  : 0)
#define cspec_vector_size(self) \
  ((self) ? (size_t)_cspec_vector_get_header(self)->size : 0)
#define cspec_vector_size_signed(self) \
  ((self) ? (ptrdiff_t)_cspec_vector_get_header(self)->size : 0)
#define cspec_vector_capacity(self) \
  ((self) ? (size_t)_cspec_vector_get_header(self)->capacity : 0)
static void *_cspec_vector_growf(
  void *self, size_t elemsize, size_t addlen, size_t min_cap
) {
  void *b;
  size_t min_len = cspec_vector_size_signed(self) + addlen;

  if(min_len > min_cap) {
    min_cap = min_len;
  }

  if(min_cap <= cspec_vector_capacity(self)) {
    return self;
  }

  if(min_cap < 2 * cspec_vector_capacity(self)) {
    min_cap = 2 * cspec_vector_capacity(self);
  } else if(min_cap < 4) {
    min_cap = 4;
  }

  b = realloc(
    (self) ? _cspec_vector_get_header(self) : 0,
    elemsize * min_cap + sizeof(_cspec_vector_header)
  );
  b = (char *)b + sizeof(_cspec_vector_header);

  if(self == NULL) {
    _cspec_vector_get_header(b)->size = 0;
  }

  _cspec_vector_get_header(b)->capacity = min_cap;

  return b;
}
#define cspec_vector_free(self)                                     \
  ((void)((self) ? realloc(_cspec_vector_get_header(self), 0) : 0), \
   (self) = NULL)

#define cspec_string_add(self, other)                   \
  do {                                                  \
    if(self == NULL && other != NULL) {                 \
      cspec_vector_initialize(self);                    \
    }                                                   \
    cspec_vector_add_n(self, other, strlen(other) + 1); \
    if(other != NULL) {                                 \
      cspec_string_ignore_last(self, 1);                \
    }                                                   \
  } while(0)
#define cspec_string_size(self) cspec_vector_size(self)
#define cspec_string_shorten(self, _len)                \
  do {                                                  \
    ptrdiff_t len = (ptrdiff_t)(_len);                  \
    if((self) != NULL) {                                \
      if(len < 0) {                                     \
        _cspec_vector_get_header(self)->size = 0;       \
      } else if(len < cspec_vector_size_signed(self)) { \
        _cspec_vector_get_header(self)->size = len;     \
      }                                                 \
      (self)[cspec_string_size(self)] = '\0';           \
    }                                                   \
  } while(0)
#define cspec_string_ignore_last(self, len) \
  cspec_string_shorten(self, cspec_string_size(self) - len)
static char *cspec_string_new(const char *initial_string) {
  char *self = NULL;
  cspec_string_add(self, initial_string);
  return self;
}
static void _cspec_string_internal_addf(char **self, const char *f, ...) {
  signed int result = 0;
  char buf[4096];
  va_list args;

  va_start(args, f)
    ;
    result = vsnprintf(buf, sizeof(buf), f, args);
  va_end(args);

  if(result >= 0) {
    cspec_string_add(*self, buf);
  }
}
#define cspec_string_addf(self, f, ...) \
  _cspec_string_internal_addf(&self, f, __VA_ARGS__)
#define cspec_string_delete(self) cspec_string_shorten(self, 0)
#define cspec_string_skip_first(self, _len)                     \
  do {                                                          \
    ptrdiff_t len = (ptrdiff_t)(_len);                          \
    if((self) != NULL) {                                        \
      if(len >= cspec_vector_size_signed(self)) {               \
        cspec_string_delete(self);                              \
      } else if(len > 0) {                                      \
        _cspec_vector_get_header(self)->size -= len;            \
        memmove((self), (self) + len, cspec_string_size(self)); \
      }                                                         \
    }                                                           \
  } while(0)
#define cspec_string_free(self) cspec_vector_free(self)

/**
 * @brief Abs for floats
 * @param value -> The value to get `abs` for
 * @return Absolute value
 */
#define cspec_fabs(value) ((value) < 0 ? -(value) : (value))

/**
 * @brief A simple function definition for running test suites
 * @param type_of_tests -> passing|failing|skipped|all
 * @param ... -> The block of modules to run
 */
#define cspec_run_suite(type_of_tests, ...)             \
  do {                                                  \
    if(strncmp((type_of_tests), "passing", 7) &&        \
       strncmp((type_of_tests), "failing", 7) &&        \
       strncmp((type_of_tests), "skipped", 7) &&        \
       strncmp((type_of_tests), "all", 3)) {            \
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
#define module(suite_name, ...)             \
  /* Define a static method for a module */ \
  static void suite_name(void) {            \
    printf(                                 \
      "\n%s%sModule `%s`%s\n",              \
      cspec->BACK_PURPLE,                   \
      cspec->YELLOW,                        \
      #suite_name,                          \
      cspec->RESET                          \
    );                                      \
    cspec_string_free(cspec->display_tab);  \
    __VA_ARGS__; /* Run describes */        \
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

#define cspec_describe_context_block(object_name, color, ...)              \
  do {                                                                     \
    cspec_string_add(cspec->display_tab, "    ");                          \
    printf(                                                                \
      "%s%s`%s`%s\n", cspec->display_tab, color, object_name, cspec->RESET \
    );                                                                     \
    __VA_ARGS__;                                                           \
    cspec_string_skip_first(cspec->display_tab, 4);                        \
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
    cspec_string_add(cspec->display_tab, "    ");      \
                                                       \
    cspec->number_of_tests++;                          \
    cspec->number_of_skipped_tests++;                  \
    cspec_string_free(cspec->test_result_message);     \
    if(!strncmp(cspec->type_of_tests, "all", 3) ||     \
       !strncmp(cspec->type_of_tests, "skipped", 7)) { \
      printf(                                          \
        "%s%s- %s%s\n",                                \
        cspec->display_tab,                            \
        cspec->GRAY,                                   \
        proc_name,                                     \
        cspec->RESET                                   \
      );                                               \
    }                                                  \
                                                       \
    cspec_string_skip_first(cspec->display_tab, 4);    \
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
    cspec_string_add(cspec->display_tab, "    ");                           \
    cspec->number_of_tests++;                                               \
    cspec_string_free(cspec->test_result_message);                          \
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
          cspec->display_tab,                                               \
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
          cspec->display_tab,                                               \
          cspec->RED,                                                       \
          cspec->RESET,                                                     \
          proc_name,                                                        \
          cspec->test_result_message,                                       \
          cspec->RESET                                                      \
        );                                                                  \
      }                                                                     \
    }                                                                       \
                                                                            \
    cspec->total_time_taken_for_tests += end_test_timer - start_test_timer; \
    cspec_string_skip_first(cspec->display_tab, 4);                         \
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

  char *test_result_message;
  char *display_tab;

  const char *type_of_tests;
  const char *current_file;
  size_t current_line;
  char *current_actual;
  char *current_expected;
  char *position_in_file;

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
      _cspec_write_position_in_file();                                 \
      _cspec_write_assert();                                           \
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
      _cspec_write_position_in_file();                                      \
      _cspec_write_assert();                                                \
    }                                                                       \
  }

/**
 * @brief Report the number of tests and time taken while testing
 */
#define cspec_report_time_taken_for_tests()                               \
  do {                                                                    \
    printf(                                                               \
      "\n%s● %zu tests\n%s✓ %zu passing\n%s✗ %zu failing\n%s- %zu " \
      "skipped%s\n",                                                      \
      cspec->YELLOW,                                                      \
      cspec->number_of_tests,                                             \
      cspec->GREEN,                                                       \
      cspec->number_of_passing_tests,                                     \
      cspec->RED,                                                         \
      cspec->number_of_failing_tests,                                     \
      cspec->GRAY,                                                        \
      cspec->number_of_skipped_tests,                                     \
      cspec->RESET                                                        \
    );                                                                    \
                                                                          \
    /* Print in seconds if the time is more than 100ms */                 \
    if(cspec->total_time_taken_for_tests > 100000000) {                   \
      printf(                                                             \
        "%s★ Finished in %.5f seconds%s\n",                               \
        cspec->CYAN,                                                      \
        cspec->total_time_taken_for_tests / 1000000000.0,                 \
        cspec->RESET                                                      \
      );                                                                  \
    } /* Else print in miliseconds */                                     \
    else {                                                                \
      printf(                                                             \
        "%s★ Finished in %.5f ms%s\n",                                    \
        cspec->CYAN,                                                      \
        cspec->total_time_taken_for_tests / 1000000.0,                    \
        cspec->RESET                                                      \
      );                                                                  \
    }                                                                     \
  } while(0)

/**
 * @brief Allocates memory for vectors to save test results in
 */
#define cspec_setup_test_data(type)                                        \
  do {                                                                     \
    printf("\033[38;5;95m/######## ########/\n");                          \
    printf(                                                                \
      "\033[38;5;95m/##### "                                               \
      "\033[38;5;89mc\033[38;5;90mS\033[38;5;91mp\033[38;5;92me\033[38;5;" \
      "93mc\033[0m \033[38;5;95m#####/\n"                                  \
    );                                                                     \
    printf("/######## ########/\033[0m\n");                                \
                                                                           \
    cspec = (cspec_data_struct *)malloc(sizeof(cspec_data_struct));        \
                                                                           \
    cspec->number_of_tests            = 0;                                 \
    cspec->number_of_passing_tests    = 0;                                 \
    cspec->number_of_failing_tests    = 0;                                 \
    cspec->number_of_skipped_tests    = 0;                                 \
    cspec->total_time_taken_for_tests = 0;                                 \
    cspec->status_of_test             = CSPEC_PASSING;                     \
                                                                           \
    cspec->test_result_message = NULL;                                     \
                                                                           \
    cspec->current_file     = NULL;                                        \
    cspec->current_line     = 0;                                           \
    cspec->current_actual   = NULL;                                        \
    cspec->current_expected = NULL;                                        \
    cspec->position_in_file = NULL;                                        \
    cspec->type_of_tests    = (type);                                      \
                                                                           \
    cspec->before_func = NULL;                                             \
    cspec->after_func  = NULL;                                             \
                                                                           \
    cspec->display_tab = NULL;                                             \
                                                                           \
    cspec->GREEN       = "\033[38;5;78m";                                  \
    cspec->RED         = "\033[38;5;203m";                                 \
    cspec->YELLOW      = "\033[38;5;11m";                                  \
    cspec->PURPLE      = "\033[1;38;5;175m";                               \
    cspec->CYAN        = "\033[38;5;51m";                                  \
    cspec->GRAY        = "\033[38;5;244m";                                 \
    cspec->RESET       = "\033[0m";                                        \
    cspec->BACK_PURPLE = "\033[48;5;96m";                                  \
  } while(0)


/** Assertions */

#define _cspec_clear_assertion_data()             \
  do {                                            \
    cspec->current_file = __FILE__;               \
    cspec->current_line = __LINE__;               \
    cspec_string_delete(cspec->position_in_file); \
    cspec_string_delete(cspec->current_actual);   \
    cspec_string_delete(cspec->current_expected); \
  } while(0)

#define _cspec_construct_actual_expected(format, actual, expected)  \
  do {                                                              \
    _cspec_clear_assertion_data();                                  \
    cspec_string_addf(cspec->current_actual, format, (actual));     \
    cspec_string_addf(cspec->current_expected, format, (expected)); \
  } while(0)

#define _cspec_write_position_in_file()         \
  do {                                          \
    cspec_string_free(cspec->position_in_file); \
    cspec_string_addf(                          \
      cspec->position_in_file,                  \
      "%s:%zu:",                                \
      cspec->current_file,                      \
      cspec->current_line                       \
    );                                          \
  } while(0)

#define _cspec_write_assert()                                      \
  do {                                                             \
    cspec->status_of_test = CSPEC_FAILING;                         \
    _cspec_write_position_in_file();                               \
    cspec_string_addf(                                             \
      cspec->test_result_message,                                  \
      "%s%s    %s\n%s        |> `%s` expected but got %s`%s`%s\n", \
      cspec->display_tab,                                          \
      cspec->RESET,                                                \
      cspec->position_in_file,                                     \
      cspec->display_tab,                                          \
      cspec->current_expected,                                     \
      cspec->RED,                                                  \
      cspec->current_actual,                                       \
      cspec->RESET                                                 \
    );                                                             \
  } while(0)

#define _cspec_write_nassert()                                         \
  do {                                                                 \
    cspec->status_of_test = CSPEC_FAILING;                             \
    _cspec_write_position_in_file();                                   \
    cspec_string_addf(                                                 \
      cspec->test_result_message,                                      \
      "%s%s    %s\n%s        |> expected that `%s` would differ from " \
      "%s`%s`%s but they are the same\n",                              \
      cspec->display_tab,                                              \
      cspec->RESET,                                                    \
      cspec->position_in_file,                                         \
      cspec->display_tab,                                              \
      cspec->current_expected,                                         \
      cspec->RED,                                                      \
      cspec->current_actual,                                           \
      cspec->RESET                                                     \
    );                                                                 \
  } while(0)

#define _cspec_assert_that_array(                                        \
  actual, expected, len_of_array, _format, comparison, output_function   \
)                                                                        \
  do {                                                                   \
    size_t i;                                                            \
    cspec->current_actual   = cspec_string_new("[");                     \
    cspec->current_expected = cspec_string_new("[");                     \
                                                                         \
    char *format = cspec_string_new(_format);                            \
    cspec_string_add(format, ", ");                                      \
    for(i = 0; i < (len_of_array) - 1; i++) {                            \
      cspec_string_addf(cspec->current_actual, format, (actual)[i]);     \
      cspec_string_addf(cspec->current_expected, format, (expected)[i]); \
    }                                                                    \
    cspec_string_ignore_last(format, 2);                                 \
    cspec_string_add(format, "]");                                       \
    cspec_string_addf(                                                   \
      cspec->current_actual, format, (actual)[(len_of_array) - 1]        \
    );                                                                   \
    cspec_string_addf(                                                   \
      cspec->current_expected, format, (expected)[(len_of_array) - 1]    \
    );                                                                   \
    for(i = 0; i < (len_of_array) - 1; i++) {                            \
      if(comparison((actual)[i], (expected)[i])) {                       \
        output_function();                                               \
        break;                                                           \
      }                                                                  \
    }                                                                    \
  } while(0)

/**
 * @brief Asserts that a proc returns true
 * @param test -> The test proc to run
 */
#define assert_that(test)                                     \
  do {                                                        \
    _cspec_clear_assertion_data();                            \
    if(!(test)) {                                             \
      cspec->status_of_test = CSPEC_FAILING;                  \
      _cspec_write_position_in_file();                        \
      cspec_string_addf(                                      \
        cspec->test_result_message,                           \
        "%s%s    %s\n%s        |> %s`%s`%s should be true\n", \
        cspec->display_tab,                                   \
        cspec->RESET,                                         \
        cspec->position_in_file,                              \
        cspec->display_tab,                                   \
        cspec->RED,                                           \
        #test,                                                \
        cspec->RESET                                          \
      );                                                      \
    }                                                         \
  } while(0)

/**
 * @brief Asserts that a proc returns false
 * @param test -> The test proc to run
 */
#define nassert_that(test)                                     \
  do {                                                         \
    _cspec_clear_assertion_data();                             \
    if((test)) {                                               \
      cspec->status_of_test = CSPEC_FAILING;                   \
      _cspec_write_position_in_file();                         \
      cspec_string_addf(                                       \
        cspec->test_result_message,                            \
        "%s%s    %s\n%s        |> %s`%s`%s should be false\n", \
        cspec->display_tab,                                    \
        cspec->RESET,                                          \
        cspec->position_in_file,                               \
        cspec->display_tab,                                    \
        cspec->RED,                                            \
        #test,                                                 \
        cspec->RESET                                           \
      );                                                       \
    }                                                          \
  } while(0)

#define _cspec_int_comparison(actual, expected) ((actual) != (expected))
static inline void assert_that_int(int actual, int expected) {
  _cspec_construct_actual_expected("%d", actual, expected);
  if(_cspec_int_comparison(actual, expected)) {
    _cspec_write_assert();
  }
}
static inline void nassert_that_int(int actual, int expected) {
  _cspec_construct_actual_expected("%d", actual, expected);
  if(!_cspec_int_comparison(actual, expected)) {
    _cspec_write_nassert();
  }
}
static inline void
assert_that_int_array(int *actual, int *expected, size_t len) {
  _cspec_assert_that_array(
    actual, expected, len, "%d", _cspec_int_comparison, _cspec_write_assert
  );
}
static inline void
nassert_that_int_array(int *actual, int *expected, size_t len) {
  _cspec_assert_that_array(
    actual, expected, len, "%d", !_cspec_int_comparison, _cspec_write_nassert
  );
}

#define _cspec_size_t_comparison(actual, expected) ((actual) != (expected))
static inline void assert_that_size_t(size_t actual, size_t expected) {
  _cspec_construct_actual_expected("%zu", actual, expected);
  if(_cspec_size_t_comparison(actual, expected)) {
    _cspec_write_assert();
  }
}
static inline void nassert_that_size_t(size_t actual, size_t expected) {
  _cspec_construct_actual_expected("%zu", actual, expected);
  if(!_cspec_size_t_comparison(actual, expected)) {
    _cspec_write_nassert();
  }
}
static inline void
assert_that_size_t_array(size_t *actual, size_t *expected, size_t len) {
  _cspec_assert_that_array(
    actual, expected, len, "%zu", _cspec_size_t_comparison, _cspec_write_assert
  );
}
static inline void
nassert_that_size_t_array(size_t *actual, size_t *expected, size_t len) {
  _cspec_assert_that_array(
    actual,
    expected,
    len,
    "%zu",
    !_cspec_size_t_comparison,
    _cspec_write_nassert
  );
}

#define _cspec_double_comparison(actual, expected) \
  (cspec_fabs(actual - expected) > 1E-12)
static inline void assert_that_double(double actual, double expected) {
  _cspec_construct_actual_expected("%g", actual, expected);
  if(_cspec_double_comparison(actual, expected)) {
    _cspec_write_assert();
  }
}
static inline void nassert_that_double(double actual, double expected) {
  _cspec_construct_actual_expected("%g", actual, expected);
  if(!_cspec_double_comparison(actual, expected)) {
    _cspec_write_nassert();
  }
}
static inline void
assert_that_double_array(double *actual, double *expected, size_t len) {
  _cspec_assert_that_array(
    actual, expected, len, "%g", _cspec_double_comparison, _cspec_write_assert
  );
}
static inline void
nassert_that_double_array(double *actual, double *expected, size_t len) {
  _cspec_assert_that_array(
    actual, expected, len, "%g", !_cspec_double_comparison, _cspec_write_nassert
  );
}

#define _cspec_charptr_comparison(actual, expected) \
  (strncmp(expected, actual, strlen(expected)))
static inline void
assert_that_charptr(const char *actual, const char *expected) {
  _cspec_construct_actual_expected("%s", actual, expected);
  if(_cspec_charptr_comparison(actual, expected)) {
    _cspec_write_assert();
  }
}
static inline void
nassert_that_charptr(const char *actual, const char *expected) {
  _cspec_construct_actual_expected("%s", actual, expected);
  if(!_cspec_charptr_comparison(actual, expected)) {
    _cspec_write_nassert();
  }
}
static inline void
assert_that_charptr_array(char **actual, char **expected, size_t len) {
  _cspec_assert_that_array(
    actual, expected, len, "%s", _cspec_charptr_comparison, _cspec_write_assert
  );
}
static inline void
nassert_that_charptr_array(char **actual, char **expected, size_t len) {
  _cspec_assert_that_array(
    actual,
    expected,
    len,
    "%s",
    !_cspec_charptr_comparison,
    _cspec_write_nassert
  );
}

#endif
