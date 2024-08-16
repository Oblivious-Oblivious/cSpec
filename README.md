# cSpec - A Full Scale Testing Library

cSpec is a lightweight, compile time unit testing library
for TDD and BDD models, heavily inspired by ruby's `RSpec`.

[![GPLv3 License](https://img.shields.io/badge/License-GPL%20v3-yellow.svg)](./COPYING)

![](https://github.com/Oblivious-Oblivious/cSpec/blob/master/images/simple_example.png)

## Why

`cSpec` provides an interface for creating both low level tests
in the form of simple assertions, as well as high level integration or even
acceptance tests in the form of `describes` or `contexts`. These tools will
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
  - `assert_that_int_array(actual, expected, length)`
  - `assert_that_double_array(actual, expected, length)`
  - `assert_that_charptr_array(actual, expected, length)`
- False assertions macro calls for defined data types
  - `nassert_that_*`

## Usage

- Include the `cSpec.h` file your project (compiled into the `export` directory)
- All directives are defined in this one header

## Contributing

1. Fork it (<https://github.com/Oblivious-Oblivious/cSpec/fork>)
2. Create your feature branch (`git checkout -b my-new-feature`)
3. Commit your changes (`git commit -am 'Add some feature'`)
4. Push to the branch (`git push origin my-new-feature`)
5. Create a new Pull Request

## Contributors

- [oblivious](https://github.com/Oblivious-Oblivious) - creator and maintainer

## Basic usage

Good practice for defining cSpec tests goes as follows:

- Define a spec file named: `mySuite.spec.c`
- Include the header files of the code to be tested.
- If you have multiple modules you can define them in different files named: `myModule.module.h`
- Define a spec suite of modules using `spec_suite`

##### Setting up the testing environment

Suppose we want to test a C project with a header and implementation file.

**_`meaning_of_life.h`_**

```C
#ifndef __MEANING_OF_LIFE_H_
#define __MEANING_OF_LIFE_H_

/**
 * @func: find_meaning_of_life
 * @brief: Finds the ultimate answer to the great
 *      question of life the universe and everything
 * @return Returns the answer
 */
int find_meaning_of_life(void);

#endif
```

**_`meaning_of_life.c`_**

```C
#include "meaning_of_life.h"

int find_meaning_of_life(void) {
  if(1 == 1) {
    return 42;
  }
}
```

**We define a spec file as such:**

**_`meaning_of_life.spec.c`_**

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
})

int main(void) {
  /* We only want to print failing tests */
  cspec_run_suite("failing", {
    lifeMod();
  });
}
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
})

#endif
```

**and then include it in the spec file:**

`life.spec.c`

```C
#include "meaning_of_life.h"
#include "cSpec.h"

/* Get modules */
#include "meaning_of_life.module.h"

int main(void) {
  cspec_run_suite("failing", {
    lifeMod();
  });
}
```

**Result:**
![](https://github.com/Oblivious-Oblivious/cSpec/blob/master/images/meaning_of_life_example.png)

It is a preety satisfying DSL, very intuitive and close to high level designs.

## Basic Interface

- [Interface](https://github.com/Oblivious-Oblivious/cSpec/blob/master/documentation/Basic%20Interface.md)

## Creating asserts

When calling asserts, the main purpose is to validate some expression,
to check for a true of false condition. cSpec uses a few
abstractions for making assert statements look more natural.

```C
#define is ==
#define isnot !=
#define equals ,
#define to
#define with
#define array_size ,
```

Using those constructs to call asserts we use the power of the C preprocessor to expand the macro like this:

- assert_that_int(actual equals to expected) --> assert_that_int(actual, expected)
- assert_that(value isnot 2) --> assert_that(value != 2)

---
