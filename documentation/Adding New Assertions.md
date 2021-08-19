# Extending cSpec with custom assertions

`C` is an extensive language where someone could define multiple complicated
structures. Testing custom structures requires previous esoteric knowledge
or the tested framework, mostly on the low level asserts. Fortunately,
cSpec allows for defining extensions files for any data type.

## The basics of an assertion
An assertion exists as an action checking the truthiness of some piece of
code. This operation requires some user provided value: **actual**, some
value to test upon: **expected** as well as a testing function to use for
comparisons. In order to save results cSpec uses two custom data
structures; a string builder `cspec_string` and a dynamic list `cspec_vector`.

## Describing `cspec_string`

### ***cspec_string*** - a dynamic string builder allocator
```C
typedef struct cspec_string {
    char *str;
    size_t alloced;
    size_t length;
} cspec_string;
```

#### Mehods:
- Allocating new space with ***`cspec_string_new`***
```C
static cspec_string *cspec_string_new(const char *initial_string) {
    cspec_string *sb;
    sb = (cspec_string*)calloc(1, sizeof(*sb));
    sb->str = (char*)malloc(32);
    *sb->str = '\0';
    sb->alloced = 32;
    sb->len = 0;
    cspec_string_add_str(sb, initial_string);
    return sb;
}
```
---
- Adding a `char*` to the builder with ***`_string_add_str`***
```C
static void cspec_string_add_str(cspec_string *sb, const char *str) {
    const char *ptr;
    size_t len;
    
    if(sb == NULL || str == NULL || *str == '\0') return;
    
    ptr = str;
    while(*ptr) ++ptr;
    len = ptr - str;
    
    cspec_string_ensure_space(sb, len);
    cspec_memmove(sb->str+sb->len, str, len);

    /* Reset len and NULL terminate */
    sb->len += len;
    sb->str[sb->len] = '\0';
}
```
---
- Adding an `int` with ***`_string_add_int`***
```C
static void cspec_string_add_int(cspec_string *sb, int val) {
    char str[1024];

    if(sb == NULL) return;

    snprintf(str, sizeof(str), "%d", val);
    cspec_string_add_str(sb, str);
}
```
---
- Adding a `double` with ***`_string_add_double_precision`***
```C
static void cspec_string_add_double_precision(cspec_string *sb, double val) {
    char str[1024];

    if(sb == NULL) return;

    /* Use %g for minimum precision on printing floats */
    snprintf(str, sizeof(str), "%g", val);
    cspec_string_add_str(sb, str);
}
```
---
- Getting the string pointer with ***`_string_get`***
```C
static char *cspec_string_get(cspec_string *sb) {
    if(sb == NULL) return NULL;
    return sb->str;
}
```
---
- Removing characters from the end with ***`_string_shorten`***
```C
static void cspec_string_shorten(cspec_string *sb, size_t len) {
    if(sb == NULL || len >= sb->len) return;

    /* Reset the len and NULL terminate, ingoring
        all values right to the NULL from memory */
    sb->len = len;
    sb->str[sb->len] = '\0';
}
```
---
- Removing characters from the beginning with ***`_string_skip`***
```C
static void cspec_string_skip(cspec_string *sb, size_t len) {
    if(sb == NULL || len == 0) return;

    if(len >= sb->len) {
        /* If we choose to drop more bytes than the
            string has simply clear the string */
        cspec_string_delete(sb);
        return;
    }

    sb->len -= len;

    /* +1 to move the NULL. */
    cspec_memmove(sb->str, sb->str + len, sb->len + 1);
}
```
---
- Resetting the string to `""` with ***`_string_delete`***
```C
static void cspec_string_delete(cspec_string *sb) {
    if(sb == NULL) return;
    cspec_string_shorten(sb, 0);
}
```
---
- Getting a substring of a current builder with `_string_substring`
```C
static cspec_string *cspec_string_substring(cspec_string *str, size_t str_from, size_t str_to) {
    cspec_string *strdup = cspec_string_dup(str);
    cspec_string_skip(strdup, str_from);
    cspec_string_shorten(strdup, str_to - str_from + 1);
    return strdup;
}
```
---
- Getting a memory duplicate of a current string with `_string_dup`
```C
static cspec_string *cspec_string_dup(cspec_string *sb) {
    cspec_string *dup;
    if(sb == NULL) return NULL;

    dup = cspec_string_new("");
    cspec_string_add_str(dup, cspec_string_get(sb));
    return dup;
}
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
When writing to the _cspec string variables we add whatever our values
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
    for(i = 0; i < len; i++)
        if(actual[i] != expected[i]) return false;

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
assert). After all this, we are done. Maybe complicated, breaks some
programming practices, and duplicates code, but remember that without a second
preprocessor, or writing to some external device such extension functionality
is very difficult to simplify and abstract more.
