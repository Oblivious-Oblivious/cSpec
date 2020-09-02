# cSpec - A Full Scale Testing Library
CSpec is a lightweight, compile time unit testing library for TDD and BDD models, heavily inspired from ruby's `rspec`.

![](https://github.com/Oblivious-Oblivious/cSpec/blob/master/images/simple_example.png)

# Why
`C` as a language is notorious for allowing the programmer to make critical errors on their code. With software gradually becoming more and more complicated it is pivotal that proper unit tests are developed as well as code being test driven, to ensure that production code is easy to refactor and maintain. `cSpec` provides an interface for creating both low level tests in the form of simple assertions, as well as high level integration or even acceptance tests in the form of `describes` or `contexts`. These tools will allow the programmer to encapsulate test modules that will be maintainable throughout huge projects.

# Features
- Assertion macro for test blocks
    - `assert_that(test)`
- Assertion macros for different data types
    - `assert_that_int(actual, expected)`
    - `assert_that_double(actual, expected)`
    - `assert_that_charptr(actual, expected)`
- Assertions for arrays
    - `assert_that_int_array(actual, expected)`
    - `assert_that_double_array(actual, expected)`
    - `assert_that_charptr_array(actual, expected)`
- Export options for txt, html, or markdown
    - `export_test_results(vec, type)`
- False assertions macro calls for defined data types
    - `nassert_that_*`

# Basic Interface

------------------
- ### ***`spec`***
```C
spec({
    /* code */
});
```
Basically a `main` function wrapper making the testing commands and code more concise.

------------------------
- ### ***`spec_suite`***
```C
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
before({
    /* code */
});
```
Inlines a code block to be executed at the ***top*** of describe blocks. Mainly used for setting up values values.

------------------------
- ### ***`after`***
```C
after({
    /* code */
});
```
Inlines a code block to be executed at the ***end*** of describe blocks. Mainly used for teardown purposes.

------------------------
- ### ***`before_each`***
```C
before_each({
    /* code */
});
```
Inlines a code block to be executed ***before each `it` block***. Mainly used to repeat some initiation of state.

------------------------
- ### ***`after_each`***
```C
after_each({
    /* code */
});
```
Inlines a code block to be executed ***after each `it` block***. Mainly used for teardown purposes.

------------------------
- ### ***`module`***
```C
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
xit("name of skipped it block", {
    /* ... */
});
```

The `xit` blocks simply declares that all following asserts inside the block will be skipped. Immediately the test becomes a skipped one and its asserts are not executed.

------------------------
- ### ***`export_test_results`***
```C
#define export_test_results(name vec, type) _BLOCK( \
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
    else if(type == "html") { \
        _cspec->fd = fopen(name, "w+"); \
        _export_to_html(); \
    } \
    else if(type == "markdown") { \
        _cspec->fd = fopen(name, "w+"); \
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

Export test results gathered through running assertions and formats it to a user specified format

-------------

## Assertions
- ### ***`assert_that`***
```C
#define assert_that(test) _BLOCK( \
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
- ### ***`assert_that_charptr(actual, expected)`***
- ### ***`assert_that_int_array(actual, expected)`***
- ### ***`assert_that_double_array(actual, expected)`***
- ### ***`assert_that_charptr_array(actual, expected)`***

## Typed, false assertions
- ### ***`nassert_that(code_block)`***
- ### ***`nassert_that_int(actual, expected)`***
- ### ***`nassert_that_double(actual, expected)`***
- ### ***`nassert_that_charptr(actual, expected)`***
- ### ***`nassert_that_int_array(actual, expected)`***
- ### ***`nassert_that_double_array(actual, expected)`***
- ### ***`nassert_that_charptr_array(actual, expected)`***

## Generic assertions
- ### ***`assert_that_value(actual, expected)`***
- ### ***`nassert_that_value(actual, expected)`***

All assertions except `assert_that` and `nassert_that` take 2 arguments as parameters, the actual and expected values.
`actual` is the value that the user provides to the test.
`expected` is the value that the user expects `actual` to be.

Generic assertions are supported for C11 making use of the `_Generic` keyword, essentially abstracting out the worry of a type system when we are writting high level tests. Although handy, it is **highly recommended** that most tests specify the type of its contents so that we can avoid accidentally comparing 'apples with oranges'.
One useful use case of `assert_that_value` could be comparing floating point values with integers, because of the ability to easily cast `int` values to `float` or `double`.

------------------------------------

# Creating asserts
When calling asserts, the main purpose is to validate some expression, to check for a true of false condition. cSpec uses a few abstractions for making assert statements look more natural.

```C
#define is ==
#define isnot !=
#define not !
#define and &&
#define or ||
#define equals ,
#define to
#define with
#define length ,
```

Using those constructs to call asserts we use the power of the C preprocessor to expand the macro like this:
- assert_that_int(actual equals to expected) --> assert_that_int(actual, expected)
- assert_that(value isnot 2) --> assert_that(value != 2)
- ...

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
int find_meaning_of_life(void);

#endif
```

***`meaning_of_life.c`***
```C
#include "meaning_of_life.h"

int find_meaning_of_life(void) {
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

**Result:**
![](https://github.com/Oblivious-Oblivious/cSpec/blob/master/images/meaning_of_life_example.png)

It is a preety satisfying DSL, very intuitive and close to high level designs.