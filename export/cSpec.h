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

/**** INCLUDES ****/
#include <signal.h>       /* signal, kill */
#include <stdint.h>       /* int8_t, int64_t, uint64_t */
#include <stdio.h>        /* FILE, fopen, fprintf, printf, snprintf */
#include <stdlib.h>       /* malloc, calloc, realloc, free */
#include <string.h>       /* fabs, strcmp */
/* #include <setjmp.h> */ /* jmp_buf, setjmp, longjmp */
#include <time.h>         /* time_t, tm */

#if defined(_WIN32)
  #include <Windows.h>
#elif defined(__unix__)
  #if defined(__clang__)
    #define CLOCK_REALTIME 0
    #define CLOCKID        CLOCK_REALTIME
  #endif

  #include <time.h>

  #define HAVE_POSIX_TIMER
  #ifdef CLOCK_MONOTONIC
    #define CLOCKID CLOCK_MONOTONIC
  #endif
#elif defined(__MACH__) && defined(__APPLE__)
  #define HAVE_MACH_TIMER
  #include <mach/mach.h>
  #include <mach/mach_time.h>
#else
  #include <time.h>
#endif

/**
 * @desc: A cross platform timer function for profiling
 * @return The time in nanoseconds
 **/
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
    clock_getres(CLOCKID, &linux_rate);
    is_init = 1;
  }
  clock_gettime(CLOCKID, &spec);
  now = spec.tv_sec * 1.0e9 + spec.tv_nsec;
  return now;
#endif
}

/** @param bool -> A 'big' enough size to hold both 1 and 0 **/
#ifndef bool
  #define bool  unsigned char
  #define true  1
  #define false 0
#endif

/**
 * @desc: A mutable string of characters used to dynamically build a string.
 * @param str -> The str char* we construct our string into
 * @param alloced -> The total sized allocated for the string
 * @param len -> The total len of the string
 **/
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
 **/
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
 **/
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
 **/
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
 **/
static void cspec_string_add_int(cspec_string *sb, int val) {
  char str[1024];

  if(sb == NULL) {
    return;
  }

  snprintf(str, sizeof(str), "%d", val);
  cspec_string_add_str(sb, str);
}

/**
 * @desc: Add a double to the builder
 * @param sb -> The string builder to use
 * @param val -> The double to add
 **/
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
 **/
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
 **/
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
 **/
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
 **/
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
 * @desc: A generic function type used upon iterable data structures
 * @param -> An element belonging to an iterable
 * @return -> A value that satisfies the callee's purpose (map, filter, reduce)
 **/
typedef void *(*cspec_lambda)(void *);

/**
 * @desc: Defines a vector data structure
 * @param items -> A void pointer array that contains the heterogenous elements
 *of the vector
 * @param alloced -> The total capacity of the vector
 * @param len -> The total number of values
 **/
typedef struct cspec_vector {
  void **items;
  size_t alloced;
  size_t len;
} cspec_vector;

/**
 * @desc: Ensure there is enough space for our values in the vector
 * @param v -> The vector to use
 * @param capacity -> The new capacity to set
 **/
static void cspec_vector_ensure_space(cspec_vector *v, size_t capacity) {
  void **items;
  if(v == NULL || capacity == 0) {
    return;
  }

  /* Attempt to reallocate new memory in the items list */
  items = (void **)realloc(v->items, sizeof(void *) * capacity);

  if(items) {
    /* Reset the items in the new memory space */
    v->items   = items;
    v->alloced = capacity;
  }
}

/**
 * @desc: Initializes a vector data structure
 * @return: The newly created vector
 **/
static cspec_vector *cspec_vector_new(void) {
  cspec_vector *v = (cspec_vector *)malloc(sizeof(cspec_vector));
  v->alloced      = 32;
  v->len          = 0;
  v->items        = (void **)malloc(sizeof(void *) * v->alloced);
  return v;
}

/**
 * @desc: Adds a new element in the vector
 * @param v -> The vector to use
 * @param item -> The item to add
 **/
static void cspec_vector_add(cspec_vector *v, void *item) {
  if(v == NULL) {
    return;
  }
  if(v->alloced == v->len) {
    cspec_vector_ensure_space(v, v->alloced * 2);
  }
  v->items[v->len++] = item;
}

/**
 * @desc: Get the value of a specific vector index
 * @param v -> The vector to use
 * @param index -> The index to get the value of
 * @return The value
 **/
static void *cspec_vector_get(cspec_vector *v, size_t index) {
  if(v == NULL) {
    return NULL;
  }
  if(index >= 0 && index < v->len) {
    return v->items[index];
  }
  return NULL;
}

/**
 * @desc: Get the total number of values inserted in the vector
 * @param v -> The vector to use
 * @return: The number of items in the vector
 **/
static size_t cspec_vector_length(cspec_vector *v) {
  if(v == NULL) {
    return 0;
  }
  return v->len;
}

/**
 * @desc: Get a memory duplicate of the passed vector
 * @param v -> The vector to use
 * @return The duplicate vector
 **/
static cspec_vector *cspec_vector_dup(cspec_vector *v) {
  size_t i;
  cspec_vector *dup;

  if(v == NULL) {
    return NULL;
  }

  dup = cspec_vector_new();

  /* Iteratively copy the vector items from one memory location to another */
  for(i = 0; i < cspec_vector_length(v); i++) {
    cspec_vector_add(dup, cspec_vector_get(v, i));
  }
  return dup;
}

/**
 * @desc: Maps all vector elements in iteration using a modifier function
 *pointer
 * @param v -> The vector to map
 * @param modifier -> The modifier function
 * @return The mapped vector duplicate
 **/
static cspec_vector *cspec_vector_map(cspec_vector *v, cspec_lambda modifier) {
  size_t i;
  cspec_vector *dup;
  if(v == NULL || modifier == NULL) {
    return NULL;
  }

  dup = cspec_vector_new();

  for(i = 0; i < cspec_vector_length(v); i++) {
    /* Pass each element through the modifier and add it to the new vector */
    cspec_vector_add(dup, modifier(cspec_vector_get(v, i)));
  }
  return dup;
}

/**
 * @brief Abs for floats
 * @param value -> The value to get `abs` for
 * @return Absolute value
 **/
static double cspec_fabs(double value) { return value < 0 ? -value : value; }

/**
 * @brief A simple function definition for running test suites
 * @param ... -> The block of modules to run
 **/
#define spec_suite(...)                                                        \
  static void run_spec_suite(const char *type_of_tests) {                      \
    cspec_setup_test_data();                                                   \
    cspec->type_of_tests = cspec_string_new(type_of_tests);                    \
                                                                               \
    /* Check for valid test type */                                            \
    if(strcmp(type_of_tests, "passing") && strcmp(type_of_tests, "failing") && \
       strcmp(type_of_tests, "skipped") && strcmp(type_of_tests, "all")) {     \
      printf("\n\033[1;31mInput a type of test to log "                        \
             "passing|failing|skipped|all\033[0m\n\n");                        \
    } else {                                                                   \
      CSPEC_BLOCK(__VA_ARGS__);                                                \
      cspec_report_time_taken_for_tests();                                     \
    }                                                                          \
  }

/**
 * @brief Expands to a function definition of the test suite
 * @param suite_name -> The name for the new module of tests
 * @param ... -> The block to define
 **/
#define module(suite_name, ...)                               \
  /* Define a static method for a module */                   \
  static void suite_name(void) {                              \
    cspec_vector *mod;                                        \
                                                              \
    cspec->name_of_module    = cspec_string_new(#suite_name); \
    cspec->list_of_describes = cspec_vector_new();            \
                                                              \
    /* Print module information */                            \
    printf(                                                   \
      "\n%s%sModule `%s`%s\n",                                \
      cspec_string_get(cspec->BACK_PURPLE),                   \
      cspec_string_get(cspec->YELLOW),                        \
      cspec_string_get(cspec->name_of_module),                \
      cspec_string_get(cspec->RESET)                          \
    );                                                        \
    cspec->display_tab = cspec_string_new("");                \
                                                              \
    CSPEC_BLOCK(__VA_ARGS__); /* Run describes */             \
                                                              \
    /* Construct a module and save for exporting */           \
    mod = cspec_vector_new();                                 \
    cspec_vector_add(mod, cspec->list_of_describes);          \
    cspec_vector_add(mod, cspec->name_of_module);             \
    cspec_vector_add(cspec->list_of_modules, mod);            \
  }

/**
 * @brief Expands to a setup proc that gets executed before the tests
 * @param ... -> The proc to run
 **/
#define before(...) CSPEC_BLOCK(__VA_ARGS__)

/**
 * @marco: after
 * @brief Expands to a teardown proc that gets executed after the tests
 * @param ... -> The proc to run
 **/
#define after(...) CSPEC_BLOCK(__VA_ARGS__)

/**
 * @brief Sets the argument to a function to run before each it block
 * @param func -> The func to set
 **/
#define before_each(func) CSPEC_BLOCK({ cspec->before_func = func; })

/**
 * @marco: after_each
 * @brief Sets the argument to a function to run after each it block
 * @param func -> The func to set
 **/
#define after_each(func) CSPEC_BLOCK({ cspec->after_func = func; })

/**
 * @brief Defines a `describe` level block
 *    On every describe block we nest inwards
 * @param ...
 **/
#define cspec_describe(...)       \
  CSPEC_BLOCK({                   \
    if(cspec->inner_nest == -1) { \
      cspec->outer_nest = 0;      \
    }                             \
    cspec->inner_nest++;          \
                                  \
    __VA_ARGS__;                  \
                                  \
    /* Reset nests */             \
    cspec->outer_nest++;          \
    cspec->inner_nest--;          \
  })

/**
 * @marco: describe
 * @brief Expands to a block execution where tests are encompassed
 * @param object_name -> The name of the unit to describe
 * @param ... -> The proc to extend to
 **/
#define describe(object_name, ...)                                          \
  CSPEC_BLOCK({                                                             \
    cspec_vector *describe_block;                                           \
                                                                            \
    cspec_string_add_str(cspec->display_tab, "    ");                       \
    cspec->list_of_its      = cspec_vector_new();                           \
    cspec->list_of_contexts = cspec_vector_new();                           \
    cspec->name_of_describe = cspec_string_new(object_name);                \
                                                                            \
    /* Display describe data */                                             \
    printf(                                                                 \
      "%s%s`%s`%s\n",                                                       \
      cspec_string_get(cspec->display_tab),                                 \
      cspec_string_get(cspec->PURPLE),                                      \
      object_name,                                                          \
      cspec_string_get(cspec->RESET)                                        \
    );                                                                      \
                                                                            \
    /* Construct the describe block */                                      \
    describe_block = cspec_vector_new();                                    \
    cspec_vector_add(describe_block, cspec_vector_dup(cspec->list_of_its)); \
    cspec_vector_add(                                                       \
      describe_block, cspec_vector_dup(cspec->list_of_contexts)             \
    );                                                                      \
    cspec_vector_add(describe_block, cspec->name_of_describe);              \
    cspec_vector_add(cspec->list_of_describes, describe_block);             \
                                                                            \
    cspec_describe(__VA_ARGS__);                                            \
                                                                            \
    cspec_string_skip(cspec->display_tab, 4);                               \
  })

/**
 * @brief Basically aliasing for descibe
 * @param object_name -> The name of the unit to describe
 * @param ... -> The proc to extend to
 **/
#define context(object_name, ...)                                          \
  CSPEC_BLOCK({                                                            \
    cspec_vector *context_block;                                           \
    cspec_vector *desc_block;                                              \
    cspec_vector *list_of_cons;                                            \
                                                                           \
    cspec->in_context_block = true;                                        \
    cspec_string_add_str(cspec->display_tab, "    ");                      \
    cspec->list_of_its     = cspec_vector_new();                           \
    cspec->name_of_context = cspec_string_new(object_name);                \
                                                                           \
    /* Display context data */                                             \
    printf(                                                                \
      "%s%s`%s`%s\n",                                                      \
      cspec_string_get(cspec->display_tab),                                \
      cspec_string_get(cspec->YELLOW),                                     \
      object_name,                                                         \
      cspec_string_get(cspec->RESET)                                       \
    );                                                                     \
                                                                           \
    /* Construct data for export */                                        \
    context_block = cspec_vector_new();                                    \
    cspec_vector_add(context_block, cspec_vector_dup(cspec->list_of_its)); \
    cspec_vector_add(context_block, cspec->name_of_context);               \
                                                                           \
    /* Grab the specific describe block using `inner_nest` */              \
    desc_block = (cspec_vector *)cspec_vector_get(                         \
      cspec->list_of_describes, cspec->inner_nest                          \
    );                                                                     \
    list_of_cons = (cspec_vector *)cspec_vector_get(desc_block, 1);        \
    cspec_vector_add(list_of_cons, context_block);                         \
                                                                           \
    cspec_describe(__VA_ARGS__);                                           \
                                                                           \
    cspec_string_skip(cspec->display_tab, 4);                              \
    cspec->in_context_block = false;                                       \
  })

/**
 * @brief Temporarily disables a test
 * @param proc_name -> The name of test to run
 * @param ... -> The actual test code
 **/
#define xit(proc_name, ...)                                                 \
  CSPEC_BLOCK({                                                             \
    size_t start_test_timer;                                                \
    size_t end_test_timer;                                                  \
                                                                            \
    /* Execute a `before` function */                                       \
    if(cspec->before_func) {                                                \
      (*cspec->before_func)();                                              \
    }                                                                       \
    cspec_string_add_str(cspec->display_tab, "    ");                       \
                                                                            \
    /* Setup for a skipped it block */                                      \
    cspec->number_of_tests++;                                               \
    cspec->number_of_skipped_tests++;                                       \
    cspec->test_result_message = cspec_string_new("");                      \
    cspec->name_of_tested_proc = cspec_string_new(proc_name);               \
    cspec->list_of_asserts     = cspec_vector_new();                        \
    cspec->status_of_test      = CSPEC_SKIPPED;                             \
                                                                            \
    /* Dummy timers */                                                      \
    start_test_timer = cspec_timer();                                       \
    end_test_timer   = cspec_timer();                                       \
                                                                            \
    /* Check for valid test type */                                         \
    if(!strcmp(cspec_string_get(cspec->type_of_tests), "all") ||            \
       !strcmp(cspec_string_get(cspec->type_of_tests), "skipped")) {        \
      /* Display data about the skipped it */                               \
      printf(                                                               \
        "%s%s- %s%s\n",                                                     \
        cspec_string_get(cspec->display_tab),                               \
        cspec_string_get(cspec->GRAY),                                      \
        cspec_string_get(cspec->name_of_tested_proc),                       \
        cspec_string_get(cspec->RESET)                                      \
      );                                                                    \
    }                                                                       \
    cspec->total_time_taken_for_tests += end_test_timer - start_test_timer; \
    cspec_insert_it_block_in_list_of_its();                                 \
    /* Reset the display tab and execute the `after_func` */                \
    cspec_string_skip(cspec->display_tab, 4);                               \
                                                                            \
    if(cspec->after_func) {                                                 \
      (*cspec->after_func)();                                               \
    }                                                                       \
  })

/**
 * @brief Expands to a test run and saves all data gathered
 * @param proc_name -> The name of test to run
 * @param proc -> The actual test code
 **/
#define it(proc_name, ...)                                                  \
  CSPEC_BLOCK(                                                              \
    size_t start_test_timer; size_t end_test_timer;                         \
                                                                            \
    /* Execute a `before` function */                                       \
    if(cspec->before_func) { (*cspec->before_func)(); }                     \
                                                                            \
    /* Setup for an it block */                                             \
    cspec_string_add_str(cspec->display_tab, "    ");                       \
    cspec->number_of_tests++;                                               \
    cspec->test_result_message = cspec_string_new("");                      \
    cspec->name_of_tested_proc = cspec_string_new(proc_name);               \
    cspec->list_of_asserts     = cspec_vector_new();                        \
                                                                            \
    /* Assume its a passing test */                                         \
    cspec->status_of_test = CSPEC_PASSING;                                  \
    cspec->current_line   = __LINE__;                                       \
    cspec->current_file   = __FILE__;                                       \
                                                                            \
    start_test_timer = cspec_timer();                                       \
                                                                            \
    /* Execute asserts */                                                   \
    __VA_ARGS__;                                                            \
    /* jmp_buf cspec->escape; */ /* setjmp(&cspec->escape); */              \
    end_test_timer = cspec_timer();                                         \
                                                                            \
    if(cspec->status_of_test) {                                             \
      cspec->number_of_passing_tests++;                                     \
      if(!strcmp(cspec_string_get(cspec->type_of_tests), "all") ||          \
         !strcmp(cspec_string_get(cspec->type_of_tests), "passing")) {      \
        printf(                                                             \
          "%s%s✓%s it %s%s\n",                                              \
          cspec_string_get(cspec->display_tab),                             \
          cspec_string_get(cspec->GREEN),                                   \
          cspec_string_get(cspec->RESET),                                   \
          cspec_string_get(cspec->name_of_tested_proc),                     \
          cspec_string_get(cspec->RESET)                                    \
        );                                                                  \
      }                                                                     \
    } else {                                                                \
      /* Even if 1 of the asserts in the current it block fails, assume we  \
       * have a failing test */                                             \
      cspec->number_of_failing_tests++;                                     \
      if(!strcmp(cspec_string_get(cspec->type_of_tests), "all") ||          \
         !strcmp(cspec_string_get(cspec->type_of_tests), "failing")) {      \
        printf(                                                             \
          "%s%s✗%s it %s:\n%s%s\n",                                         \
          cspec_string_get(cspec->display_tab),                             \
          cspec_string_get(cspec->RED),                                     \
          cspec_string_get(cspec->RESET),                                   \
          cspec_string_get(cspec->name_of_tested_proc),                     \
          cspec_string_get(cspec->test_result_message),                     \
          cspec_string_get(cspec->RESET)                                    \
        );                                                                  \
      }                                                                     \
    } /* Calculate the total time */                                        \
    cspec->total_time_taken_for_tests += end_test_timer - start_test_timer; \
                                                                            \
    cspec_insert_it_block_in_list_of_its();                                 \
                                                                            \
    /* Reset the display tab and execute the `after_func` */                \
    cspec_string_skip(cspec->display_tab, 4);                               \
    if(cspec->after_func) { (*cspec->after_func)(); }                       \
  )

/**
 * @brief Exports the test results to some file type
 * @param vec -> Either passing|failing|skipped|all
 * @param type -> Export type either txt|xml|markdown
 **/
#define export_test_results(name, vec, type)                          \
  /* Check for valid type of test export */                           \
  CSPEC_BLOCK({                                                       \
    if(strcmp(vec, "passing") && strcmp(vec, "failing") &&            \
       strcmp(vec, "skipped") && strcmp(vec, "all")) {                \
      printf(                                                         \
        "\n%sInput a type of test to export "                         \
        "passing|failing|skipped|all%s\n\n",                          \
        cspec_string_get(cspec->RED),                                 \
        cspec_string_get(cspec->RESET)                                \
      );                                                              \
      return 0;                                                       \
    }                                                                 \
    cspec->type_of_export_tests = cspec_string_new(vec);              \
                                                                      \
    /* Reset the display tab */                                       \
    cspec_string_delete(cspec->display_tab);                          \
    cspec_string_add_str(cspec->display_tab, "    ");                 \
    if(!strcmp(type, "txt")) {                                        \
      cspec->fd = fopen(name, "w+");                                  \
      cspec_export_to_txt();                                          \
    } else if(!strcmp(type, "xml")) {                                 \
      cspec->fd = fopen(name, "w+");                                  \
      cspec_export_to_xml();                                          \
    } else if(!strcmp(type, "markdown")) {                            \
      cspec->fd = fopen(name, "w+");                                  \
      cspec_export_to_md();                                           \
    } else {                                                          \
      printf(                                                         \
        "\n%sSpecify the export type: `txt|xml|markdown|html`%s\n\n", \
        cspec_string_get(cspec->RED),                                 \
        cspec_string_get(cspec->RESET)                                \
      );                                                              \
      return 0; /* Exit the main function */                          \
    }                                                                 \
  })

/**
 * @brief Global variables grouped in container
 * @param number_of_tests -> The total number of tests performed
 * @param number_of_passing_tests -> Counts the passing tests
 * @param number_of_failing_tests -> Counts the failing tests
 * @param number_of_skipped_tests -> Counts the skipped tests
 * @param status_of_test -> Either _PASSING|_FAILING|_SKIPPED
 * @param fd -> A file descriptor used for saving test results
 * @param total_time_taken_for_tests
 * @param signals_vector -> A vector saving descriptions about signals
 * @param test_result_message -> The string builder we construct for assertions
 * @param name_of_tested_proc -> The current name of the it block being tested
 * @param name_of_describe -> The current name of the describe block
 * @param name_of_context -> The current name of the context block
 * @param name_of_module -> The current name of the module block
 * @param display_tab -> A 4 space overhead used for a nicer display of test
 *results
 * @param inner_nest -> Counts the times we enter a nested block
 * @param outer_nest -> Counts the times we exit a nested block
 * @param type_of_tests -> The type of tests we want to display
 * @param type_of_export_tests -> The type of tests we want to export
 * @param current_assert -> The current assert token (only used in
 *`assert_that`)
 * @param current_file -> Current value of the __FILE__ macro used for tracking
 *assert positions in the file
 * @param current_actual -> Current actual value token
 * @param current_expected -> Current expected value token
 * @param position_in_file -> A string builder containing __FILE__ and __LINE__
 *results
 * @param assert_result -> A string builder containing the result description of
 *the current assert
 * @param current_line -> Current value of the __LINE__ macro
 * @param in_context_block -> A boolean signaling if we are testing a context
 *block
 * @param before_func -> A function pointer to be executed before it blocks
 * @param after_func -> A function pointer to be executed after it blocks
 * @param COLORS -> Terminal string color codes
 * @param list_of_modules -> A vector containing data about modules
 * @param list_of_describes -> A vector containing data about describes, part of
 *list_of_modules
 * @param list_of_contexts -> A vector containing data about contexts, part of
 *list_of_describes
 * @param list_of_its -> A vector containing data about it block, part of
 *list_of_contexts
 * @param list_of_asserts -> A vector containing data about asserts, part of
 *list_of_its
 **/
typedef struct cspec_data_struct {
  uint64_t number_of_tests;
  uint64_t number_of_passing_tests;
  uint64_t number_of_failing_tests;
  uint64_t number_of_skipped_tests;
  int8_t status_of_test;
  FILE *fd;
  /* jmp_buf escape; */

  uint64_t total_time_taken_for_tests;
  cspec_vector *signals_vector;

  cspec_string *test_result_message;
  cspec_string *name_of_tested_proc;
  cspec_string *name_of_describe;
  cspec_string *name_of_context;
  cspec_string *name_of_module;
  cspec_string *display_tab;
  int64_t inner_nest;
  int64_t outer_nest;

  cspec_string *type_of_tests;
  cspec_string *type_of_export_tests;
  cspec_string *current_assert;
  const char *current_file;
  cspec_string *current_actual;
  cspec_string *current_expected;
  cspec_string *position_in_file;
  cspec_string *assert_result;
  uint64_t current_line;
  bool in_context_block;

  void (*before_func)(void);
  void (*after_func)(void);

  /** Colors **/
  cspec_string *GREEN;
  cspec_string *RED;
  cspec_string *YELLOW;
  cspec_string *PURPLE;
  cspec_string *CYAN;
  cspec_string *GRAY;
  cspec_string *WHITE;
  cspec_string *RESET;
  cspec_string *BACK_PURPLE;

  cspec_vector *list_of_modules;
  cspec_vector *list_of_describes;
  cspec_vector *list_of_contexts;
  cspec_vector *list_of_its;
  cspec_vector *list_of_asserts;
} cspec_data_struct;

static cspec_data_struct *cspec;

/**
 * @param CSPEC_PASSING -> Set for passing tests
 * @param CSPEC_FAILING -> Set for failing tests
 * @param CSPEC_SKIPPED -> Set for skipeed tests
 **/
#define CSPEC_PASSING 1
#define CSPEC_FAILING 0
#define CSPEC_SKIPPED -1

/**
 * @brief Expands to a do while loop that runs once
 *      Useful for executing naked blocks
 * @param ... -> The block of code to run
 **/
#define CSPEC_BLOCK(...) \
  do {                   \
    __VA_ARGS__          \
  } while(0)

#define is    ==
#define isnot !=

#ifndef __cplusplus
  #define not !
  #define and &&
  #define or  ||
#endif

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
 **/
#define define_assert(                                                 \
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

#define define_assert_array(                                                \
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
 **/
static void cspec_write_position_in_file(void) {
  cspec->position_in_file = cspec_string_new("");
  cspec_string_add_str(cspec->position_in_file, cspec->current_file);
  cspec_string_add_str(cspec->position_in_file, ":");
  cspec_string_add_int(cspec->position_in_file, cspec->current_line);
  cspec_string_add_str(cspec->position_in_file, ":");
}

/**
 * @brief Setup the assert description for printing and exporting
 **/
static void cspec_write_assert_actual_expected(void) {
  cspec_vector *list_of_strings;
  cspec->assert_result = cspec_string_new("");
  cspec_string_add_str(
    cspec->test_result_message, cspec_string_get(cspec->display_tab)
  );
  cspec_string_add_str(
    cspec->test_result_message, cspec_string_get(cspec->RESET)
  );
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
  cspec_string_add_str(
    cspec->test_result_message, cspec_string_get(cspec->RED)
  );
  cspec_string_add_str(cspec->test_result_message, "`");
  cspec_string_add_str(
    cspec->test_result_message, cspec_string_get(cspec->current_actual)
  );
  cspec_string_add_str(cspec->test_result_message, "`\n");
  cspec_string_add_str(
    cspec->test_result_message, cspec_string_get(cspec->RESET)
  );
  /****************************************************************************/
  cspec_string_add_str(cspec->assert_result, "|> `");
  cspec_string_add_str(
    cspec->assert_result, cspec_string_get(cspec->current_expected)
  );
  cspec_string_add_str(cspec->assert_result, "` expected but got ");
  cspec_string_add_str(cspec->assert_result, "`");
  cspec_string_add_str(
    cspec->assert_result, cspec_string_get(cspec->current_actual)
  );
  /****************************************************************************/
  list_of_strings = cspec_vector_new();
  cspec_vector_add(list_of_strings, cspec_string_get(cspec->position_in_file));
  cspec_vector_add(list_of_strings, cspec_string_get(cspec->assert_result));
  /****************************************************************************/
  cspec_vector_add(cspec->list_of_asserts, list_of_strings);
}

/**
 * @brief Inserts the it block in its designated vector index
 **/
static void cspec_insert_it_block_in_list_of_its(void) {
  /* Setup the it block vector */
  cspec_vector *it_block = cspec_vector_new();
  cspec_vector_add(it_block, cspec->list_of_asserts);
  cspec_vector_add(it_block, cspec->name_of_tested_proc);
  cspec_vector_add(it_block, (void *)(long)cspec->status_of_test);

  if(cspec->in_context_block) {
    /* Insert in a context block */
    if(cspec_vector_length(cspec->list_of_describes) > 1) {
      /* Stupid indexing for finding the correct nested blocks */
      cspec_vector *desc_block = (cspec_vector *)cspec_vector_get(
        cspec->list_of_describes,
        -(cspec_vector_length(cspec->list_of_describes) - cspec->inner_nest - 1)
      );
      cspec_vector *list_of_cons =
        (cspec_vector *)cspec_vector_get(desc_block, 1);
      cspec_vector *con_block = (cspec_vector *)cspec_vector_get(
        list_of_cons, cspec_vector_length(list_of_cons) - 1
      );
      cspec_vector *list_of_con_its =
        (cspec_vector *)cspec_vector_get(con_block, 0);
      cspec_vector_add(list_of_con_its, it_block);
    } else {
      cspec_vector *desc_block = (cspec_vector *)cspec_vector_get(
        cspec->list_of_describes,
        cspec_vector_length(cspec->list_of_describes) - 1
      );
      cspec_vector *list_of_cons =
        (cspec_vector *)cspec_vector_get(desc_block, 1);
      cspec_vector *con_block = (cspec_vector *)cspec_vector_get(
        list_of_cons, cspec_vector_length(list_of_cons) - 1
      );
      cspec_vector *list_of_con_its =
        (cspec_vector *)cspec_vector_get(con_block, 0);
      cspec_vector_add(list_of_con_its, it_block);
    }
  } else {
    /* Insert in a describe block */
    cspec_vector *desc_block = (cspec_vector *)cspec_vector_get(
      cspec->list_of_describes,
      cspec_vector_length(cspec->list_of_describes) - 1 - cspec->outer_nest
    );
    cspec_vector *list_of_desc_its =
      (cspec_vector *)cspec_vector_get(desc_block, 0);
    cspec_vector_add(list_of_desc_its, it_block);
  }
}

/**
 * @brief Report the number of tests and time taken while testing
 **/
static void cspec_report_time_taken_for_tests(void) {
  printf(
    "\n%s● %llu tests\n%s✓ %llu passing\n%s✗ %llu failing\n%s- %llu "
    "skipped%s\n",
    cspec_string_get(cspec->YELLOW),
    cspec->number_of_tests,
    cspec_string_get(cspec->GREEN),
    cspec->number_of_passing_tests,
    cspec_string_get(cspec->RED),
    cspec->number_of_failing_tests,
    cspec_string_get(cspec->GRAY),
    cspec->number_of_skipped_tests,
    cspec_string_get(cspec->RESET)
  );

  /* Print in seconds if the time is more than 100ms */
  if(cspec->total_time_taken_for_tests > 100000000) {
    printf(
      "%s★ Finished in %.5f seconds%s\n",
      cspec_string_get(cspec->CYAN),
      cspec->total_time_taken_for_tests / 1000000000.0,
      cspec_string_get(cspec->RESET)
    );
  }
  /* Else print in miliseconds */
  else {
    printf(
      "%s★ Finished in %.5f ms%s\n",
      cspec_string_get(cspec->CYAN),
      cspec->total_time_taken_for_tests / 1000000.0,
      cspec_string_get(cspec->RESET)
    );
  }
}

/**
 * @brief Writes asserts to an output file
 * @param assert -> The assert string to export
 **/
static void cspec_fprintf_asserts(cspec_vector *assert) {
  fprintf(
    cspec->fd,
    "%s            %s\n                %s\n\n",
    cspec_string_get(cspec->display_tab),
    (char *)cspec_vector_get(assert, 0),
    (char *)cspec_vector_get(assert, 1)
  );
}

/**
 * @brief Writes it blocks to an output file
 * @param it_block -> The it block to iterate against
 **/
static void cspec_fprintf_its(cspec_vector *it_block) {
  /* Check for type of test */
  if(((long)cspec_vector_get(it_block, 2) == CSPEC_FAILING)) {
    if(!strcmp(cspec_string_get(cspec->type_of_export_tests), "failing") ||
       !strcmp(cspec_string_get(cspec->type_of_export_tests), "all")) {
      /* Write failing tests */
      cspec_vector *asserts;
      fprintf(
        cspec->fd,
        "%s        ✗ it %s\n",
        cspec_string_get(cspec->display_tab),
        cspec_string_get((cspec_string *)cspec_vector_get(it_block, 1))
      );
      asserts = (cspec_vector *)cspec_vector_get(it_block, 0);

      /* Call to print the asserts iteratevely for the current it block */
      cspec_vector_map(asserts, (cspec_lambda)cspec_fprintf_asserts);
    }
  } else if(((long)cspec_vector_get(it_block, 2) == CSPEC_SKIPPED)) {
    if(!strcmp(cspec_string_get(cspec->type_of_export_tests), "skipped") ||
       !strcmp(cspec_string_get(cspec->type_of_export_tests), "all")) {
      /* Write skipped tests */
      fprintf(
        cspec->fd,
        "%s        - it %s\n",
        cspec_string_get(cspec->display_tab),
        cspec_string_get((cspec_string *)cspec_vector_get(it_block, 1))
      );
    }
  } else if(((long)cspec_vector_get(it_block, 2) == CSPEC_PASSING)) {
    /* Write passing tests */
    if(!strcmp(cspec_string_get(cspec->type_of_export_tests), "passing") ||
       !strcmp(cspec_string_get(cspec->type_of_export_tests), "all")) {
      fprintf(
        cspec->fd,
        "%s        ✓ it %s\n",
        cspec_string_get(cspec->display_tab),
        cspec_string_get((cspec_string *)cspec_vector_get(it_block, 1))
      );
    }
  }
}

/**
 * @brief Writes context blocks to an output file
 * @param con -> The context block to iterate against
 **/
static void cspec_fprintf_cons(cspec_vector *con) {
  cspec_vector *its;

  cspec_string_add_str(cspec->display_tab, "    ");

  fprintf(
    cspec->fd,
    "        `%s`\n",
    cspec_string_get((cspec_string *)cspec_vector_get(con, 1))
  );
  its = (cspec_vector *)cspec_vector_get(con, 0);

  /* Iterate over the it blocks under the context block */
  cspec_vector_map(its, (cspec_lambda)cspec_fprintf_its);

  cspec_string_skip(cspec->display_tab, 4);
}

/**
 * @brief Writes describe blocks to an output file
 * @param desc -> The describe block to iterate against
 **/
static void cspec_fprintf_describes(cspec_vector *desc) {
  cspec_vector *its;
  cspec_vector *cons;

  fprintf(
    cspec->fd,
    "    `%s`\n",
    cspec_string_get((cspec_string *)cspec_vector_get(desc, 2))
  );

  /* Iterate over the it blocks under the describe block */
  its = (cspec_vector *)cspec_vector_get(desc, 0);
  cspec_vector_map(its, (cspec_lambda)cspec_fprintf_its);

  /* Iterate over the context blocks under the describe block */
  cons = (cspec_vector *)cspec_vector_get(desc, 1);
  cspec_vector_map(cons, (cspec_lambda)cspec_fprintf_cons);
}

/**
 * @brief Writes module blocks to an output file
 * @param mod -> The module block to iterate against
 **/
static void cspec_fprintf_modules(cspec_vector *mod) {
  cspec_vector *descs;

  cspec_string_delete(cspec->display_tab);
  fprintf(
    cspec->fd,
    "\nModule `%s`\n",
    cspec_string_get((cspec_string *)cspec_vector_get(mod, 1))
  );
  descs = (cspec_vector *)cspec_vector_get(mod, 0);

  /* Iterate over the describe blocks under the module block */
  cspec_vector_map(descs, (cspec_lambda)cspec_fprintf_describes);
}

/**
 * @brief Export test results into a txt file
 **/
static void cspec_export_to_txt(void) {
  cspec_vector_map(cspec->list_of_modules, (cspec_lambda)cspec_fprintf_modules);
  fclose(cspec->fd);
}

/**
 * @brief Writes asserts to an xml file
 * @param assert -> The assert block containing strings to print
 **/
static void cspec_xml_write_asserts(cspec_vector *assert) {
  fprintf(
    cspec->fd,
    "%s                <failure>\n",
    cspec_string_get(cspec->display_tab)
  );

  fprintf(
    cspec->fd,
    "%s                    <position>%s</position>\n",
    cspec_string_get(cspec->display_tab),
    (char *)cspec_vector_get(assert, 0)
  );

  fprintf(
    cspec->fd,
    "%s                    <error>%s</error>\n",
    cspec_string_get(cspec->display_tab),
    (char *)cspec_vector_get(assert, 1)
  );

  fprintf(
    cspec->fd,
    "%s                </failure>\n",
    cspec_string_get(cspec->display_tab)
  );
}

/**
 * @brief Writes it blocks to an xml file
 * @param it_block -> The it block to iterate against
 **/
static void cspec_xml_write_its(cspec_vector *it_block) {
  /* Check for type of test */
  if(((long)cspec_vector_get(it_block, 2) == CSPEC_FAILING)) {
    if(!strcmp(cspec_string_get(cspec->type_of_export_tests), "failing") ||
       !strcmp(cspec_string_get(cspec->type_of_export_tests), "all")) {
      cspec_vector *asserts;
      /* Write failing tests */
      fprintf(
        cspec->fd,
        "            %s<it>\n                %s<name>%s</name>\n               "
        " %s<status>failing</status>\n",
        cspec_string_get(cspec->display_tab),
        cspec_string_get(cspec->display_tab),
        cspec_string_get((cspec_string *)cspec_vector_get(it_block, 1)),
        cspec_string_get(cspec->display_tab)
      );
      asserts = (cspec_vector *)cspec_vector_get(it_block, 0);

      /* Call to print the asserts iteratevely for the current it block */
      cspec_vector_map(asserts, (cspec_lambda)cspec_xml_write_asserts);
      fprintf(
        cspec->fd, "            %s</it>\n", cspec_string_get(cspec->display_tab)
      );
    }
  } else if(((long)cspec_vector_get(it_block, 2) == CSPEC_SKIPPED)) {
    if(!strcmp(cspec_string_get(cspec->type_of_export_tests), "skipped") ||
       !strcmp(cspec_string_get(cspec->type_of_export_tests), "all")) {
      /* Write skipped tests */
      fprintf(
        cspec->fd,
        "            %s<xit>\n                %s<name>%s</name>\n              "
        "  %s<status>skipped</status>\n",
        cspec_string_get(cspec->display_tab),
        cspec_string_get(cspec->display_tab),
        cspec_string_get((cspec_string *)cspec_vector_get(it_block, 1)),
        cspec_string_get(cspec->display_tab)
      );
      fprintf(
        cspec->fd,
        "            %s</xit>\n",
        cspec_string_get(cspec->display_tab)
      );
    }
  } else if(((long)cspec_vector_get(it_block, 2) == CSPEC_PASSING)) {
    /* Write passing tests */
    if(!strcmp(cspec_string_get(cspec->type_of_export_tests), "passing") ||
       !strcmp(cspec_string_get(cspec->type_of_export_tests), "all")) {
      fprintf(
        cspec->fd,
        "            %s<it>\n                %s<name>%s</name>\n               "
        " %s<status>passing</status>\n",
        cspec_string_get(cspec->display_tab),
        cspec_string_get(cspec->display_tab),
        cspec_string_get((cspec_string *)cspec_vector_get(it_block, 1)),
        cspec_string_get(cspec->display_tab)
      );
      fprintf(
        cspec->fd, "            %s</it>\n", cspec_string_get(cspec->display_tab)
      );
    }
  }
}

/**
 * @brief Writes context blocks to an xml file
 * @param con -> The context block to iterate against
 **/
static void cspec_xml_write_cons(cspec_vector *con) {
  cspec_vector *its;
  cspec_string_add_str(cspec->display_tab, "    ");
  fprintf(
    cspec->fd,
    "            <context>\n                <name>%s</name>\n",
    cspec_string_get((cspec_string *)cspec_vector_get(con, 1))
  );
  its = (cspec_vector *)cspec_vector_get(con, 0);

  /* Iterate over the it blocks under the context block */
  cspec_vector_map(its, (cspec_lambda)cspec_xml_write_its);
  fprintf(cspec->fd, "            </context>\n");
  cspec_string_skip(cspec->display_tab, 4);
}

/**
 * @brief Writes describe blocks to an xml file
 * @param desc -> The current describe block to iterate against
 **/
static void cspec_xml_write_describes(cspec_vector *desc) {
  cspec_vector *its;
  cspec_vector *cons;

  fprintf(
    cspec->fd,
    "        <describe>\n            <name>%s</name>\n",
    cspec_string_get((cspec_string *)cspec_vector_get(desc, 2))
  );

  /* Iterate over the it blocks under the describe block */
  its = (cspec_vector *)cspec_vector_get(desc, 0);
  cspec_vector_map(its, (cspec_lambda)cspec_xml_write_its);

  /* Iterate over the context blocks under the describe block */
  cons = (cspec_vector *)cspec_vector_get(desc, 1);
  cspec_vector_map(cons, (cspec_lambda)cspec_xml_write_cons);
  fprintf(cspec->fd, "        </describe>\n");
}

/**
 * @brief Writes module blocks to an xml file
 * @param mod -> The module block to iterate against
 **/
static void cspec_xml_write_modules(cspec_vector *mod) {
  cspec_vector *descs;

  time_t t = time(NULL);
  struct tm calc_time;
  localtime_r(&t, &calc_time);

  cspec_string_delete(cspec->display_tab);
  fprintf(
    cspec->fd,
    "    <module>\n        <name>%s</name>\n        "
    "<failures>%llu</failures>\n        <skipped>%llu</skipped>\n        "
    "<tests>%llu</tests>\n        <timestamp>%d-%02d-%02d "
    "%02d:%02d:%02d</timestamp>\n",
    cspec_string_get((cspec_string *)cspec_vector_get(mod, 1)),
    cspec->number_of_failing_tests,
    cspec->number_of_skipped_tests,
    cspec->number_of_tests,
    calc_time.tm_year + 1900,
    calc_time.tm_mon + 1,
    calc_time.tm_mday,
    calc_time.tm_hour,
    calc_time.tm_min,
    calc_time.tm_sec
  );
  descs = (cspec_vector *)cspec_vector_get(mod, 0);

  /* Iterate over the describe blocks under the module block */
  cspec_vector_map(descs, (cspec_lambda)cspec_xml_write_describes);
  fprintf(cspec->fd, "    </module>\n");
}

/**
 * @brief Export test results into an xml file
 **/
static void cspec_export_to_xml(void) {
  fprintf(cspec->fd, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");

  if(cspec->total_time_taken_for_tests > 100000000) {
    fprintf(
      cspec->fd,
      "<modules>\n    <duration>%.5f seconds</duration>\n",
      cspec->total_time_taken_for_tests / 1000000000.0
    );
  } else {
    fprintf(
      cspec->fd,
      "<modules>\n    <duration>%.5f ms</duration>\n",
      cspec->total_time_taken_for_tests / 1000000.0
    );
  }

  cspec_vector_map(
    cspec->list_of_modules, (cspec_lambda)cspec_xml_write_modules
  );
  fprintf(cspec->fd, "</modules>\n");
  fclose(cspec->fd);
}

/**
 * @brief Export test results into a markdown file
 **/
static void cspec_export_to_md(void) {}

/**
 * @brief Export test results into an html file
 **/
static void cspec_export_to_html(void) {}

/**
 * @brief Handles errors where undefined behaviour
 *         calls for segfauls or other singals
 * @param signal_id -> The number assign to the signal
 **/
static void cspec_signal_handler(const int signal_id) {
  char *sig_description;
  cspec_string *sig;

  cspec_string *id = cspec_string_new("");
  cspec_string_add_int(id, signal_id);

  /* Find the correct signal description */
  switch(signal_id) {
  /* SIGINT */ case 2:
    sig_description = (char *)cspec_vector_get(cspec->signals_vector, 0);
    break;
  /* SIGILL */ case 4:
    sig_description = (char *)cspec_vector_get(cspec->signals_vector, 1);
    break;
  /* SIGABRT */ case 6:
    sig_description = (char *)cspec_vector_get(cspec->signals_vector, 2);
    break;
  /* SIGFPE */ case 8:
    sig_description = (char *)cspec_vector_get(cspec->signals_vector, 3);
    break;
  /* SIGSEGV */ case 11:
    sig_description = (char *)cspec_vector_get(cspec->signals_vector, 4);
    break;
  /* SIGTERM */ case 15:
    sig_description = (char *)cspec_vector_get(cspec->signals_vector, 5);
    break;
  }

  /* Craft a response for signal errors */
  sig = cspec_string_new("");
  cspec_string_add_str(sig, cspec_string_get(cspec->display_tab));
  cspec_string_add_str(sig, cspec_string_get(cspec->RED));
  cspec_string_add_str(sig, "✗");
  cspec_string_add_str(sig, cspec_string_get(cspec->RESET));
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
  cspec_string_add_str(sig, cspec_string_get(cspec->RESET));
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
 * @brief Allocates memory for vectors to save test results in
 **/
static void cspec_setup_test_data(void) {
  /* Printf a neat intro */
  printf("\033[38;5;95m/######## ########/\n");
  printf("\033[38;5;95m/##### "
         "\033[38;5;89mc\033[38;5;90mS\033[38;5;91mp\033[38;5;92me\033[38;5;"
         "93mc\033[0m \033[38;5;95m#####/\n");
  printf("/######## ########/\033[0m\n");

  /* Craft the global variable struct */
  cspec = (cspec_data_struct *)malloc(sizeof(cspec_data_struct) + 1);

  cspec->number_of_tests            = 0;
  cspec->number_of_passing_tests    = 0;
  cspec->number_of_failing_tests    = 0;
  cspec->number_of_skipped_tests    = 0;
  cspec->total_time_taken_for_tests = 0;
  cspec->status_of_test             = CSPEC_PASSING;
  /* jmp_buf cspec->escape; */

  cspec->test_result_message = NULL;
  cspec->name_of_tested_proc = NULL;
  cspec->name_of_describe    = NULL;
  cspec->name_of_context     = NULL;

  cspec->current_assert       = NULL;
  cspec->current_file         = NULL;
  cspec->current_line         = 0;
  cspec->current_actual       = NULL;
  cspec->current_expected     = NULL;
  cspec->position_in_file     = NULL;
  cspec->assert_result        = NULL;
  cspec->type_of_tests        = NULL;
  cspec->type_of_export_tests = NULL;

  /** Before and after procs **/
  cspec->before_func = NULL;
  cspec->after_func  = NULL;

  cspec->display_tab      = NULL;
  cspec->signals_vector   = cspec_vector_new();
  cspec->inner_nest       = -1;
  cspec->outer_nest       = 0;
  cspec->in_context_block = false;

  /** Colors **/
  cspec->GREEN       = cspec_string_new("\033[38;5;78m");
  cspec->RED         = cspec_string_new("\033[1;31m");
  cspec->YELLOW      = cspec_string_new("\033[38;5;11m");
  cspec->PURPLE      = cspec_string_new("\033[38;5;207m");
  cspec->CYAN        = cspec_string_new("\033[1;36m");
  cspec->GRAY        = cspec_string_new("\033[38;5;244m");
  cspec->RESET       = cspec_string_new("\033[0m");
  cspec->BACK_PURPLE = cspec_string_new("\033[48;5;89m");

  cspec->list_of_modules   = cspec_vector_new();
  cspec->list_of_describes = cspec_vector_new();
  cspec->list_of_contexts  = cspec_vector_new();
  cspec->list_of_its       = cspec_vector_new();
  cspec->list_of_asserts   = cspec_vector_new();

  /* Signals that can be processed */
  cspec_vector_add(cspec->signals_vector, (void *)"`SIGINT | Interrupt`");
  cspec_vector_add(
    cspec->signals_vector, (void *)"`SIGILL | Illegal instruction`"
  );
  cspec_vector_add(cspec->signals_vector, (void *)"`SIGABRT | Abort`");
  cspec_vector_add(
    cspec->signals_vector,
    (void *)"`SIGFPE | Floating - Point arithmetic exception`"
  );
  cspec_vector_add(
    cspec->signals_vector, (void *)"`SIGSEGV | Segmentation Violation`"
  );
  cspec_vector_add(cspec->signals_vector, (void *)"`SIGTERM | Termination`");

  /* Setup the signals */
  signal(SIGINT, cspec_signal_handler);
  signal(SIGILL, cspec_signal_handler);
  signal(SIGABRT, cspec_signal_handler);
  signal(SIGFPE, cspec_signal_handler);
  signal(SIGSEGV, cspec_signal_handler);
  signal(SIGTERM, cspec_signal_handler);
}

/**
 * @brief Writes actual and expected values
 * @param actual -> The value passed by the user
 * @param expected -> The value `actual` is tested against
 * @param len -> The length of the arrays
 **/
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
 **/
static bool
cspec_charptr_array_comparison(char **actual, char **expected, size_t len) {
  /* Check for members */
  size_t i;
  for(i = 0; i < len; i++) {
    if(strcmp(actual[i], expected[i])) {
      return false;
    }
  }

  /* All elements are equal */
  return true;
}

/**
 * @brief Assertion of two char* arrays
 **/
define_assert_array(
  cspec_call_assert_that_charptr_array,
  char **,
  cspec_to_string_charptr_array_write,
  !cspec_charptr_array_comparison,
  len
)

  /**
   * @brief Negative assertion of two char* arrays
   **/
  define_assert_array(
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
 **/
#define assert_that_charptr_array(inner)         \
  CSPEC_BLOCK({                                  \
    cspec->current_file = __FILE__;              \
    cspec->current_line = __LINE__;              \
    cspec_call_assert_that_charptr_array(inner); \
  })

/**
 * @brief Assert that the expected charptr array differs from the result
 * @param actual -> The actual value
 * @param expected -> The expected value
 **/
#define nassert_that_charptr_array(inner)         \
  CSPEC_BLOCK({                                   \
    cspec->current_file = __FILE__;               \
    cspec->current_line = __LINE__;               \
    cspec_call_nassert_that_charptr_array(inner); \
  })

  /**
   * @brief Writes actual and expected values
   * @param actual -> The value passed by the user
   * @param expected -> The value `actual` is tested against
   **/
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
 **/
static bool cspec_charptr_comparison(const char *actual, const char *expected) {
  return !strcmp(expected, actual);
}

/**
 * @brief Assert that the expected string is equal to the result
 * @param actual -> The value passed by the user
 * @param expected -> The value `actual` is tested against
 **/
define_assert(
  cspec_call_assert_that_charptr,
  const char *,
  cspec_to_string_charptr_write,
  !cspec_charptr_comparison
)

  /**
   * @brief Assert that the expected string is different than the result
   * @param actual -> The value passed by the user
   * @param expected -> The value `actual` is tested against
   **/
  define_assert(
    cspec_call_nassert_that_charptr,
    const char *,
    cspec_to_string_charptr_write,
    cspec_charptr_comparison
  )

/**
 * @brief Assert that the expected string is equal to the result
 * @param actual -> The actual value
 * @param expected -> The expected string
 **/
#define assert_that_charptr(inner)         \
  CSPEC_BLOCK({                            \
    cspec->current_file = __FILE__;        \
    cspec->current_line = __LINE__;        \
    cspec_call_assert_that_charptr(inner); \
  })

/**
 * @brief Assert that the expected string is different than the result
 * @param actual -> The actual value
 * @param expected -> The expected string
 **/
#define nassert_that_charptr(inner)         \
  CSPEC_BLOCK({                             \
    cspec->current_file = __FILE__;         \
    cspec->current_line = __LINE__;         \
    cspec_call_nassert_that_charptr(inner); \
  })

  /**
   * @brief Writes the actual and expected values
   * @param actual -> The value passed by the user
   * @param expected -> The value `actual` is tested against
   * @param len -> The length of the arrays
   **/
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
 **/
static bool
cspec_double_array_comparison(double *actual, double *expected, size_t len) {
  /* Check for members */
  size_t i;
  for(i = 0; i < len; i++) {
    if(cspec_fabs(actual[i] - expected[i]) > 1E-12) {
      return false;
    }
  }

  /* All elements are equal */
  return true;
}

/**
 * @brief Assertion of two double arrays
 **/
define_assert_array(
  cspec_call_assert_that_double_array,
  double *,
  cspec_to_string_double_array_write,
  !cspec_double_array_comparison,
  len
)

  /**
   * @brief Negative assertion of two double arrays
   **/
  define_assert_array(
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
 **/
#define assert_that_double_array(inner)         \
  CSPEC_BLOCK({                                 \
    cspec->current_file = __FILE__;             \
    cspec->current_line = __LINE__;             \
    cspec_call_assert_that_double_array(inner); \
  })

/**
 * @brief Assert that the expected double array differs from the result
 * @param actual -> The actual value
 * @param expected -> The expected value
 **/
#define nassert_that_double_array(inner)         \
  CSPEC_BLOCK({                                  \
    cspec->current_file = __FILE__;              \
    cspec->current_line = __LINE__;              \
    cspec_call_nassert_that_double_array(inner); \
  })

  /**
   * @brief Writes actual and expected values
   * @param actual -> The value passed by the user
   * @param expected -> The value `actual` is tested against
   **/
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
 **/
static bool cspec_double_comparison(double actual, double expected) {
  /* Calculate the margin to which the difference
      is too big so the test fails */
  return cspec_fabs(actual - expected) < 1E-12;
}

/**
 * @brief Assert that the expected double is different than the result
 * @param actual -> The value passed by the user
 * @param expected -> The value `actual` is tested against
 **/
define_assert(
  cspec_call_assert_that_double,
  double,
  cspec_to_string_double_write,
  !cspec_double_comparison
)

  /**
   * @brief Assert that the expected double is different than the result
   * @param actual -> The value passed by the user
   * @param expected -> The value `actual` is tested against
   **/
  define_assert(
    cspec_call_nassert_that_double,
    double,
    cspec_to_string_double_write,
    cspec_double_comparison
  )

/**
 * @brief Assert that the expected double is different than the result
 * @param actual -> The actual value
 * @param expected -> The expected double
 **/
#define assert_that_double(inner)         \
  CSPEC_BLOCK({                           \
    cspec->current_file = __FILE__;       \
    cspec->current_line = __LINE__;       \
    cspec_call_assert_that_double(inner); \
  })

/**
 * @brief Assert that the expected double is different than the result
 * @param actual -> The actual value
 * @param expected -> The expected double
 **/
#define nassert_that_double(inner)         \
  CSPEC_BLOCK({                            \
    cspec->current_file = __FILE__;        \
    cspec->current_line = __LINE__;        \
    cspec_call_nassert_that_double(inner); \
  })

/**
 * @brief Asserts that a proc returns true
 * @param test -> The test proc to run
 **/
#define assert_that(test)                                                            \
  CSPEC_BLOCK({                                                                      \
    cspec->current_file     = __FILE__;                                              \
    cspec->current_line     = __LINE__;                                              \
    cspec->position_in_file = cspec_string_new("");                                  \
    cspec->assert_result    = cspec_string_new("");                                  \
                                                                                     \
    /* Save the test token */                                                        \
    cspec->current_assert = cspec_string_new(#test);                                 \
                                                                                     \
    /* Assert a custom block of code */                                              \
    if(!(test)) {                                                                    \
      cspec_vector *list_of_strings;                                                 \
                                                                                     \
      cspec->status_of_test = CSPEC_FAILING;                                         \
      cspec_write_position_in_file();                                                \
                                                                                     \
      /* Craft the string builder for terminal output */                             \
      cspec_string_add_str(                                                          \
        cspec->test_result_message, cspec_string_get(cspec->display_tab)             \
      );                                                                             \
      cspec_string_add_str(                                                          \
        cspec->test_result_message, cspec_string_get(cspec->RESET)                   \
      );                                                                             \
      cspec_string_add_str(cspec->test_result_message, "    ");                      \
      cspec_string_add_str(                                                          \
        cspec->test_result_message, cspec_string_get(cspec->position_in_file)        \
      );                                                                             \
      cspec_string_add_str(cspec->test_result_message, "\n");                        \
      cspec_string_add_str(                                                          \
        cspec->test_result_message, cspec_string_get(cspec->display_tab)             \
      );                                                                             \
      cspec_string_add_str(cspec->test_result_message, "        |> ");               \
      cspec_string_add_str(                                                          \
        cspec->test_result_message, cspec_string_get(cspec->RED)                     \
      );                                                                             \
      cspec_string_add_str(cspec->test_result_message, "`");                         \
      cspec_string_add_str(cspec->test_result_message, #test);                       \
      cspec_string_add_str(cspec->test_result_message, "`");                         \
      cspec_string_add_str(                                                          \
        cspec->test_result_message, cspec_string_get(cspec->RESET)                   \
      );                                                                             \
      cspec_string_add_str(cspec->test_result_message, " should be true\n");         \
      cspec_string_add_str(                                                          \
        cspec->test_result_message, cspec_string_get(cspec->RESET)                   \
      );                                                                             \
      /*************************************************************************/    \
      /* Craft the string builder for text output (miss the color codes) */          \
      cspec_string_add_str(cspec->assert_result, "|> `");                            \
      cspec_string_add_str(cspec->assert_result, #test);                             \
      cspec_string_add_str(cspec->assert_result, "`");                               \
      cspec_string_add_str(cspec->assert_result, " should be true");                 \
      /****************************************************************************/ \
      list_of_strings = cspec_vector_new();                                          \
      cspec_vector_add(                                                              \
        list_of_strings, cspec_string_get(cspec->position_in_file)                   \
      );                                                                             \
      cspec_vector_add(                                                              \
        list_of_strings, cspec_string_get(cspec->assert_result)                      \
      );                                                                             \
      /****************************************************************************/ \
      cspec_vector_add(cspec->list_of_asserts, list_of_strings);                     \
    }                                                                                \
  })

/**
 * @brief Asserts that a proc returns false
 * @param test -> The test proc to run
 **/
#define nassert_that(test)                                                           \
  CSPEC_BLOCK({                                                                      \
    cspec->current_file     = __FILE__;                                              \
    cspec->current_line     = __LINE__;                                              \
    cspec->position_in_file = cspec_string_new("");                                  \
    cspec->assert_result    = cspec_string_new("");                                  \
                                                                                     \
    /* Save the test token */                                                        \
    cspec->current_assert = cspec_string_new(#test);                                 \
                                                                                     \
    if((test)) {                                                                     \
      cspec_vector *list_of_strings;                                                 \
                                                                                     \
      cspec->status_of_test = CSPEC_FAILING;                                         \
      cspec_write_position_in_file();                                                \
                                                                                     \
      /* Craft the string builder for terminal output */                             \
      cspec_string_add_str(                                                          \
        cspec->test_result_message, cspec_string_get(cspec->display_tab)             \
      );                                                                             \
      cspec_string_add_str(                                                          \
        cspec->test_result_message, cspec_string_get(cspec->RESET)                   \
      );                                                                             \
      cspec_string_add_str(cspec->test_result_message, "    ");                      \
      cspec_string_add_str(                                                          \
        cspec->test_result_message, cspec_string_get(cspec->position_in_file)        \
      );                                                                             \
      cspec_string_add_str(cspec->test_result_message, "\n");                        \
      cspec_string_add_str(                                                          \
        cspec->test_result_message, cspec_string_get(cspec->display_tab)             \
      );                                                                             \
      cspec_string_add_str(cspec->test_result_message, "        |> ");               \
      cspec_string_add_str(                                                          \
        cspec->test_result_message, cspec_string_get(cspec->RED)                     \
      );                                                                             \
      cspec_string_add_str(cspec->test_result_message, "`");                         \
      cspec_string_add_str(cspec->test_result_message, #test);                       \
      cspec_string_add_str(cspec->test_result_message, "`");                         \
      cspec_string_add_str(                                                          \
        cspec->test_result_message, cspec_string_get(cspec->RESET)                   \
      );                                                                             \
      cspec_string_add_str(cspec->test_result_message, " should be false\n");        \
      cspec_string_add_str(                                                          \
        cspec->test_result_message, cspec_string_get(cspec->RESET)                   \
      );                                                                             \
      /*************************************************************************/    \
      /* Craft the string builder for text output (miss the color codes) */          \
      cspec_string_add_str(cspec->assert_result, "|> `");                            \
      cspec_string_add_str(cspec->assert_result, #test);                             \
      cspec_string_add_str(cspec->assert_result, "`");                               \
      cspec_string_add_str(cspec->assert_result, " should be false");                \
      /****************************************************************************/ \
      list_of_strings = cspec_vector_new();                                          \
      cspec_vector_add(                                                              \
        list_of_strings, cspec_string_get(cspec->position_in_file)                   \
      );                                                                             \
      cspec_vector_add(                                                              \
        list_of_strings, cspec_string_get(cspec->assert_result)                      \
      );                                                                             \
      /****************************************************************************/ \
      cspec_vector_add(cspec->list_of_asserts, list_of_strings);                     \
    }                                                                                \
  })

  /**
   * @brief Writes actual and expected values
   * @param actual -> The value passed by the user
   * @param expected -> The value `actual` is tested against
   * @param len -> The length of the arrays
   **/
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
 **/
static bool cspec_int_array_comparison(int *actual, int *expected, size_t len) {
  /* Check for members */
  size_t i;
  for(i = 0; i < len; i++) {
    if(actual[i] != expected[i]) {
      return false;
    }
  }

  /* All elements are equal */
  return true;
}

/**
 * @brief Assertion of two int arrays
 **/
define_assert_array(
  cspec_call_assert_that_int_array,
  int *,
  cspec_to_string_int_array_write,
  !cspec_int_array_comparison,
  len
)

  /**
   * @brief Negative assertion of two int arrays
   **/
  define_assert_array(
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
 **/
#define assert_that_int_array(inner)         \
  CSPEC_BLOCK({                              \
    cspec->current_file = __FILE__;          \
    cspec->current_line = __LINE__;          \
    cspec_call_assert_that_int_array(inner); \
  })

/**
 * @brief Assert that the expected int array differs from the result
 * @param actual -> The actual value
 * @param expected -> The expected value
 **/
#define nassert_that_int_array(inner)         \
  CSPEC_BLOCK({                               \
    cspec->current_file = __FILE__;           \
    cspec->current_line = __LINE__;           \
    cspec_call_nassert_that_int_array(inner); \
  })

  /**
   * @brief Writes actual and expected values
   * @param actual -> The value passed by the user
   * @param expected -> The value `actual` is tested against
   **/
  static void cspec_to_string_int_write(int actual, int expected) {
  cspec->current_actual   = cspec_string_new("");
  cspec->current_expected = cspec_string_new("");
  cspec_string_add_int(cspec->current_actual, actual);
  cspec_string_add_int(cspec->current_expected, expected);
}

/**
 * @brief A function that compares integers for assertions
 * @param actual -> The value passed by the user
 * @param expected -> The value `actual` is tested against
 * @return a boolean
 **/
static bool cspec_int_comparison(int actual, int expected) {
  return actual == expected;
}

/**
 * @brief Assert that the expected integer is equal to the result
 * @param actual -> The value passed by the user
 * @param expected -> The value `actual` is tested against
 **/
define_assert(
  cspec_call_assert_that_int,
  int,
  cspec_to_string_int_write,
  !cspec_int_comparison
)

  /**
   * @brief Assert that the expected integer is different than the result
   * @param actual -> The value passed by the user
   * @param expected -> The value `actual` is tested against
   **/
  define_assert(
    cspec_call_nassert_that_int,
    int,
    cspec_to_string_int_write,
    cspec_int_comparison
  )

/**
 * @brief Assert that the expected integer is equal to the result
 * @param actual -> The actual value
 * @param expected -> The expected int
 **/
#define assert_that_int(inner)         \
  CSPEC_BLOCK({                        \
    cspec->current_file = __FILE__;    \
    cspec->current_line = __LINE__;    \
    cspec_call_assert_that_int(inner); \
  })

/**
 * @brief Assert that the expected integer is different than the result
 * @param actual -> The actual value
 * @param expected -> The expected int
 **/
#define nassert_that_int(inner)         \
  CSPEC_BLOCK({                         \
    cspec->current_file = __FILE__;     \
    cspec->current_line = __LINE__;     \
    cspec_call_nassert_that_int(inner); \
  })

#endif
