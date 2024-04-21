# Extending cSpec with custom assertions

`C` is an extensive language where someone could define multiple complicated
structures. Testing custom structures requires previous esoteric knowledge
of the tested framework, mostly on the low level asserts.  cSpec allows for defining extensions files for any data type.

## The basics of an assertion
An assertion checks for the truthiness of some piece of code. This operation
requires some user provided value: **actual**, some value to test
upon: **expected** as well as a testing function to use for comparisons.
In order to save results cSpec uses two custom data structures; a
string builder `cspec_string` and a dynamic list `cspec_vector`.

## Describing `cspec_string`

### ***cspec_string*** - a dynamic string builder allocator
```C
typedef struct cspec_string {
  char *str;
  size_t alloced;
  size_t length;
} cspec_string;
```

The lirary uses the `cspec_string` data structure to construct
string builders and save them to a hierarchy of vectors.

# Crafting a new assert

The library provides a built in macro for defining new asserts
in a modular way. The macro `define_assert` makes it easier to
have a streamlined way of defining assertions of any data type. It works as such:

- `define_assert(name_of_assert, <type>, write_to_string_method, datatype_comparison)`

Example:
```C
/* Define our custom `assert_int_func` assert */
define_assert(
  cspec_call_assert_that_int,
  int,
  cspec_to_string_int_write,
  !cspec_int_comparison
)

#define cspec_to_string_int_write(actual, expected) CSPEC_BLOCK( \
  cspec->current_actual = cspec_string_new(""); \
  cspec->current_expected = cspec_string_new(""); \
  cspec_string_add_int(cspec->current_actual, actual); \
  cspec_string_add_int(cspec->current_expected, expected); \
)

static bool cspec_int_comparison(int actual, int expected) {
  return actual == expected;
}
```

Here we define a custom way to assert equality between two `int` values.
When writing to the `_cspec` string variables we add whatever our values
are to the equivalent string builders. The reason we use the negation
of the comparison method, is because assertions check if the
comparison is failing so that error descriptions are triggered.
***This is the way all provided assertions are defined, but you can hardcode your own if you study the code.***

After defining the base functionality we need an *interface* for
us to use in the spec files. An interface can be defined as such:

```C
#define assert_that_int(inner) CSPEC_BLOCK( \
  cspec->current_file = __FILE__; \
  cspec->current_line = __LINE__; \
  cspec_call_assert_that_int(inner); \
)
```

When we craft asserts we need to provide the user with the file and line
constants for displaying the position of the error in the file. We have
to define this inside a macro call so that we make sure it gets inlined
to the spec file code properly. Setting file and line values inside a
function will provide information about the function itself not the user
provided spec code. Now we can successfully call the assert like this:
```C
assert_that_int(1, 2);
/* or */
assert_that_int(1 equals to 2);
/* ...and so on... */
```

# Something more complicated

We are going to use the method above to define an assert for arrays
of integers. Idealy we would want to call the function as such:

```C
int list_a[3] = {1, 2, 3};
int list_b[3] = {4, 4, 4};
assert_that_int_array(list_a equals to list_b with length 3);
```

### Functionality
```C
static void cspec_to_string_int_array_write(int *actual, int *expected, size_t len) {
  size_t i;

  cspec->current_actual = cspec_string_new("[");
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
```
We define a method that displays the array in a readable way with brackets.
We also use the build in _string_add_int to write integers to the string builder.
So an the array: `int arr[5] = {1, 2, 3, 4, 5}` would be displayed as `[1, 2, 3, 4, 5]`.

### Interface
```C
define_assert_array(
  cspec_call_assert_that_int_array,
  int*,
  cspec_to_string_int_array_write,
  !cspec_int_array_comparison,
  len
)

#define assert_that_int_array(inner) CSPEC_BLOCK( \
  cspec->current_file = __FILE__; \
  cspec->current_line = __LINE__; \
  cspec_call_assert_that_int_array(inner); \
)
```
Here we call the built in `define_assert_array` that also receives the length
of the arrays we compare. Obviously if the actual has a different length that
the expected array they are not equal (length can be checked with a different
assert). After all this, we are done. Maybe complicated, breaks some programming
practices, but without a second preprocessor, or writing to some external device
such extension functionality is very difficult to simplify and abstract more.
