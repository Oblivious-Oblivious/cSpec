# cSpec - A Full Scale Testing Library

CSpec is a lightweight, compile time unit testing library
for TDD and BDD models, heavily inspired from ruby's `rspec`.

[![GPLv3 License](https://img.shields.io/badge/License-GPL%20v3-yellow.svg)](./COPYING)

![](https://github.com/Oblivious-Oblivious/cSpec/blob/master/images/simple_example.png)

## Why
`cSpec` provides an interface for creating both low level tests
in the form of simple assertions, as well as high level integration or even
acceptance tests in the form of `describes` or `contexts`.  These tools will
allow the programmer to encapsulate test modules that will be maintainable
throughout huge projects.

## Features
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

## Usage

* Include the `cSpec.h` file your project (lies inside the `export` directory)
* All directives are defined in this one header

## Contributing

1. Fork it (<https://github.com/Oblivious-Oblivious/cSpec/fork>)
2. Create your feature branch (`git checkout -b my-new-feature`)
3. Commit your changes (`git commit -am 'Add some feature'`)
4. Push to the branch (`git push origin my-new-feature`)
5. Create a new Pull Request

## Contributors

- [Oblivious](https://github.com/Oblivious-Oblivious) - creator and maintainer

## Basic usage
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
 * @brief: Finds the ultimate answer to the great
 *      question of life the universe and everything
 * @return Returns the answer
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

## Basic Interface

- [Interface](https://github.com/Oblivious-Oblivious/cSpec/blob/master/documentation/Basic%20Interface.md)

## Typed, truth assertions
- ##### ***`assert_that(code_block)`***
- ##### ***`assert_that_int(actual, expected)`***
- ##### ***`assert_that_double(actual, expected)`***
- ##### ***`assert_that_charptr(actual, expected)`***
- ##### ***`assert_that_int_array(actual, expected)`***
- ##### ***`assert_that_double_array(actual, expected)`***
- ##### ***`assert_that_charptr_array(actual, expected)`***

## Typed, false assertions
- ##### ***`nassert_that(code_block)`***
- ##### ***`nassert_that_int(actual, expected)`***
- ##### ***`nassert_that_double(actual, expected)`***
- ##### ***`nassert_that_charptr(actual, expected)`***
- ##### ***`nassert_that_int_array(actual, expected)`***
- ##### ***`nassert_that_double_array(actual, expected)`***
- ##### ***`nassert_that_charptr_array(actual, expected)`***

All assertions except `assert_that` and `nassert_that` take 2
arguments as parameters, the actual and expected values.
`actual` is the value that the user provides to the test.
`expected` is the value that the user expects `actual` to be.

------------------------------------

## Creating asserts
When calling asserts, the main purpose is to validate some expression,
to check for a true of false condition. cSpec uses a few
abstractions for making assert statements look more natural.

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

## Adding New Assertions

- [New Assertions](https://github.com/Oblivious-Oblivious/cSpec/blob/master/documentation/Adding%20New%20Assertions.md)

## Implementation
All functionality is included in a single header file `cSpec.h`. You can
include the header into your test files. Production code and test code
are completely separate. The library modifies a global struct of variables
that are visible thoughout all tests. Variables include file names,
lines of asserts, assert result strings, status of tests and more.

The use of global values is driven by the design of the library.
Each interface block (`describe`, `context`, `it`, ... ) is
a `do {} while(0)` block of code that gets inlined inside a `module`.
This choice of design allows us to embed multiple blocks inside each other.
This way we can define complicated interactions between objects and functions
as well as be able to define readable test structures. This approach however
creates a different namespace (scope) for each block, meaning that moving
data from inner blocks to outer blocks poses significant problems in scoping.
The most basic solution is to move everything to a global scope, since the
compile time definition of tests is linear anyway,
so there is no need for sophisticated code constructs.

------------------------------
