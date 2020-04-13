# cSpec - A Full Scale Testing Library
CSpec is a lightweight, compile time unit testing library for TDD and BDD models, heavily inspired from ruby's `rspec`.

![alt text][logo]

[logo]: https://github.com/Oblivious-Oblivious/cSpec/tree/master/images/simple_example.png

# Why
`C` as a language is notorious for allowing the programmer to make critical errors on their code. With software gradually becoming more and more complicated it is pivotal that proper unit tests are developed as well as code being test driven, to ensure that production code is easy to refactor and maintain. `cSpec` provides an interface for creating both low level tests in the form of simple assertions, as well as high level integration or even acceptance tests in the form of `describes` or `contexts`. These tools will allow the programmer to encapsulate test modules that will be maintainable throughout huge projects.



# Features
- Assertion macro for test blocks
    - `assert_that(test)`
- Assertion macros for different data types
    - `assert_that_int(actual, expected)`
    - `assert_that_double(actual, expected)`
    - `assert_that_string(actual, expected)`
- Generic assertion macros for C11
    - `assert_that_value(actual, expected)`
- Export options for txt, html, or markdown
    - `export_test_results(vec, type)`
- False assertions macro calls
    - `nassert_that_*`

# Basic Interface

------------------
- ### ***`spec`***
```C
#define spec(...) \
    int main(int argc, char **argv) { \
        _BLOCK(__VA_ARGS__); \
    }

spec({
    /* code */
});
```
Basically a `main` function wrapper making the testing commands and code more concise.

------------------------
- ### ***`spec_suite`***
```C
#define spec_suite(...) \
    static void run_spec_suite(char *type_of_tests) { \
        _setup_test_data(); \
        _BLOCK(__VA_ARGS__); \
    }

spec_suite({
    /*
        module_name1();
        module_name2();
        ...
    */
});
```
Defines a top-level function where test modules are run. Because the library's basic functionality relies on appending arbitrary macro calls, at least one top-level routine must be declared. The purpose of `spec_suite` is to run user selected  test modules but can be used to also setup function level variables or allocations.

------------------------
- ### ***`before`***
```C
#define before(...) _BLOCK( \
    __VA_ARGS__ \
)

before({
    /* code */
});
```
Inlines a code block to be executed at the ***top*** of describe blocks. Mainly used for setting up values values.

------------------------
- ### ***`after`***
```C
#define after(...) _BLOCK( \
    __VA_ARGS__ \
)

after({
    /* code */
});
```
Inlines a code block to be executed at the ***end*** of describe blocks. Mainly used for teardown purposes.

------------------------
- ### ***`before_each`***
```C
#define before_each(...) _BLOCK( \
    __VA_ARGS__ \
)

before_each({
    /* code */
});
```
Inlines a code block to be executed ***before each `it` block***. Mainly used to repeat some initiation of state.

------------------------
- ### ***`after_each`***
```C
#define after_each(...) _BLOCK( \
    __VA_ARGS__ \
)

after_each({
    /* code */
});
```
Inlines a code block to be executed ***after each `it` block***. Mainly used for teardown purposes.

------------------------
- ### ***`module`***
```C
#define module(suite_name, ...) \
    static void suite_name(void) { \
        _cspec->name_of_module = _new_string(#suite_name); \
        _cspec->list_of_describes = _new_vector(); \
        _cspec->display_tab = _new_string(""); \
        _BLOCK(__VA_ARGS__); \
        _vector *mod = _new_vector(); \
        _vector_add(mod, _cspec->list_of_describes); \
        _vector_add(mod, _cspec->name_of_module); \
        _vector_add(_cspec->list_of_modules, mod); \
        \
        /* mod([describes], name) */ \
    }

module(func_name, {
    describe("name 1", {});
    describe("name 2", {});
    /* ... */
});
```
Defines a test module, containing one or multiple `describe` blocks. This allows for modules to exist in separate files for better management and inclusion of tests.

------------------------
- ### ***`describe`***
```C
#define describe(object_name, ...) _BLOCK( \
    _string_add_str(_cspec->display_tab, "    "); \
    _cspec->list_of_its = _new_vector(); \
    _cspec->list_of_contexts = _new_vector(); \
    _cspec->name_of_describe = _new_string(object_name); \
    _vector *describe_block = _new_vector(); \
    _vector_add(describe_block, _vector_dup(_cspec->list_of_its)); \
    _vector_add(describe_block, _vector_dup(_cspec->list_of_contexts)); \
    _vector_add(describe_block, _cspec->name_of_describe); \
    _vector_add(_cspec->list_of_describes, describe_block); \
    \
    _describe(__VA_ARGS__); \
    \
    _string_skip(_cspec->display_tab, 4); \
    /* desc([its], [contexts], name)*/ \
)

describe("describe name", {
    it("does something", {});
    it("does something else", {});
    /* ... */
});
```
Defines a code block for grouping tests. Best use of the `describe` block would be testing for 'things' like objects or variables or anything that has semantic meaning.

------------------------
- ### ***`context`***
```C
#define context(object_name, ...) _BLOCK( \
    _cspec->in_context_block = true; \
    _string_add_str(_cspec->display_tab, "    "); \
    _cspec->list_of_its = _new_vector(); \
    _cspec->name_of_context = _new_string(object_name); \
    _vector *context_block = _new_vector(); \
    _vector_add(context_block, _vector_dup(_cspec->list_of_its)); \
    _vector_add(context_block, _cspec->name_of_context); \
    _vector *desc_block = _vector_get(_cspec->list_of_describes, _cspec->inner_nest); \
    _vector *list_of_cons = _vector_get(desc_block, 1); \
    _vector_add(list_of_cons, context_block); \
    _describe(__VA_ARGS__); \
    _string_skip(_cspec->display_tab, 4); \
    _cspec->in_context_block = false; \
    \
    /* con([its], name) */ \
)

context("context name", {
    it("tests some state", {});
    it("tests some more state", {});
    /* ... */
});
```
Defines a code block for grouping tests. Best use of the `context` block would be testing for 'states' of data and their interaction.

------------------------
- ### ***`it`***
```C
#define it(proc_name, ...) _BLOCK( \
    /* Execute a `before` function */ \
    if(_cspec->before_func) (*_cspec->before_func)(); \
    \
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
    __VA_ARGS__ \
    size_t end_test_timer = _get_timer(); \
    \
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

it("tries to test stuff using asserts", {
    meaning_of_life = 43;
    assert_that(1 is 1);
    assert_that_int(meaning_of_life equals to 42);
    /* ... */
});
```

The `it` block defines the basic declaration of a test procedure. You can call an `it` with one or multiple `assert` blocks. The test will be considered *passing* only if all asserts are true. Finding even one failing assert will make the test failing as well, and the test result will contain information about the error.

------------------------
- ### ***`xit`***
```C
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

xit("name of skipped it block", {
    /* ... */
});
```

The `xit` blocks simply declares that all following asserts inside the block will be skipped. Immediately the test becomes a skipped one and its asserts are not executed.

------------------------
- ### ***`export_test_results`***
```C
#define export_test_results(vec, type) _BLOCK( \
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
        _cspec->fd = fopen("output.txt", "w+"); \
        _export_to_txt(); \
    } \
    else if(type == "html") { \
        _cspec->fd = fopen("output.html", "w+"); \
        _export_to_html(); \
    } \
    else if(type == "markdown") { \
        _cspec->fd = fopen("output.md", "w+"); \
        _export_to_md(); \
    } \
    else { \
        printf("\n%sSpecify the export type: `txt|html|markdown`%s\n\n", \
        _string_get(_cspec->RED), \
        _string_get(_cspec->RESET)); \
        return 0; /* Exit the main function */ \
    } \
)
```

Export test results gathered through running assertions and formats it to txt, html or markdown. Writes to a file named `output.txt`

-------------

## Assertions
- ### ***`assert_that`***
```C
#define assert_that(test) _BLOCK( \
    _cspec->current_file = __FILE__; \
    _cspec->current_line = __LINE__; \
    _cspec->position_in_file = _new_string(""); \
    _cspec->assert_result = _new_string(""); \
    _cspec->current_assert = #test; \
    \
    /* Assert a custom block of code */ \
	if(!(test)) { \
        _cspec->status_of_test = _FAILING; \
        _write_position_in_file(); \
        _write_assert(); \
    } \
    \
    /* assert(string_builder) */ \
)
```
Basic assert declaration. Asserts a test block for truthiness.

--------------------------------------------------------------

## Typed, truth assertions
- ### ***`assert_that(code_block)`***
- ### ***`assert_that_int(actual, expected)`***
- ### ***`assert_that_double(actual, expected)`***
- ### ***`assert_that_string(actual, expected)`***

## Typed, false assertions
- ### ***`nassert_that(code_block)`***
- ### ***`nassert_that_int(actual, expected)`***
- ### ***`nassert_that_double(actual, expected)`***
- ### ***`nassert_that_string(actual, expected)`***

## Generic assertions
- ### ***`assert_that_value(actual, expected)`***
- ### ***`nassert_that_value(actual, expected)`***

All assertions except `assert_that` and `nassert_that` take 2 arguments as parameters, the actual and expected values.
`actual` is the value that the user provides to the test.
`expected` is the value that the user expects `actual` to be.

Generic assertions are supported for C11 making use of the `_Generic` keyword, essentially abstracting out the worry of a type system when we are writting high level tests. Although handy, it is **highly recommended** that most tests specify the type of its contents so that we can avoid accidentally comparing 'apples with oranges'.
One useful use case of `assert_that_value` could be comparing floating point values with integers, because of the ability to easily cast `int` values to `float` or `double`.

------------------------------------

# Implementation
All functionality is included in a single header file `cSpec.h`. You can include the header into your test files. Production code and test code are completely separate. The library modifies a global struct of variables that are visible thoughout all tests. Variables include file names, lines of asserts, assert result strings, status of tests and more.

The use of global values is driven by the design of the library. Each interface block (`describe`, `context`, `it`, ... ) is a `do {} while(0)` block of code that gets inlined inside a `module`. This choice of design allows us to embed multiple blocks inside each other. This way we can define complicated interactions between objects and functions as well as be able to define readable test structures. This approach however creates a different namespace (scope) for each block, meaning that moving data from inner blocks to outer blocks poses significant problems in scoping. The most basic solution is to move everything to a global scope, since the compile time definition of tests is linear anyway, so there is no need for sophisticated code constructs.

------------------------------

# Basic usage
Good practice for defining cSpec tests goes as follows:
- Define a spec file named: `mySuite.spec.c`
- Include the header files of the code to be tested.
- If you have multiple modules you can define them in different files named: `myModule.module.h`
- Define a spec suite of modules using `spec_suite`
- Run the `spec` function and optionally export the results using `export_test_results`

##### Setting up the testing environment
Suppose we want to test a C project with a header and implementation file.

***`meaning_of_life.h`***

```C
#ifndef __MEANING_OF_LIFE_H_
#define __MEANING_OF_LIFE_H_

/**
 * @func: find_meaning_of_life
 * @desc: Returns the ultimate answer to the great
 *      question of life the universe and everything
 * @return The number
 **/
int find_meaning_of_life();

#endif
```

***`meaning_of_life.c`***
```C
#include "meaning_of_life.h"

int find_meaning_of_life() {
    if(1 == 1) {
        return 42;
    }
}
```

**We would define a spec file as such:**

***`meaning_of_life.spec.c`***
```C
#include "meaning_of_life.h"
#include "cSpec.h"

module(lifeMod, {
    describe("what is life", {
        it("seeks for the meaning of life", {
            assert_that(find_meaning_of_life() is 42);
            assert_that_int(find_meaning_of_life() equals to 42);
        });
    });
});

spec_suite({
    lifeMod();
});

spec({
    run_spec_suite("failing"); /* We only want to print failing tests */
    export_test_results("all", "txt"); /* We want a txt file of all test results */
});
```

**Alternatively we could define a module as such:**

`meaning_of_life.module.h`

```C
#ifndef __MEANING_OF_LIFE_MODULE_H_
#define __MEANING_OF_LIFE_MODULE_H_

module(lifeMod, {
    describe("what is life", {
        it("seeks for the meaning of life", {
            assert_that(find_meaning_of_life() is 42);
            assert_that_int(find_meaning_of_life() equals to 42);
        });
    });
});

#endif
```

**and then include it in the spec file:**

`life.spec.c`
```C
#include "meaning_of_life.h"
#include "cSpec.h"

/* Get modules */
#include "meaning_of_life.module.h"

spec_suite({
    lifeMod();
});

spec({
    run_spec_suite("failing"); /* We only want to print failing tests */
    export_test_results("all", "txt"); /* We want a txt file of all test results */
});
```

**You can compile the file as such:**
```BASH
gcc meaning_of_life.c life.spec.c  -O3 -Wno-int-conversion -Wno-incompatible-pointer-types -Wno-pointer-to-int-cast
```

It is a preety satisfying DSL, very intuitive and close to high level designs.