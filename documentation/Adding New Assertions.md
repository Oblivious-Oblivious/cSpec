# Extending cSpec with custom assertions

`C` is an extensive language where someone could define multiple complicated structures. Testing custom structures requires previous esoteric knowledge or the tested framework, mostly on the low level asserts. Fortunately, cSpec allows for defining extensions files for any data type.

## The basics of an assertion
An assertion exists as an action checking the truthiness of some piece of code. This operation requires some user provided value: **actual**, some value to test upon: **expected** as well as a testing function to use for comparisons. In order to save results cSpec uses two custom data structures; a string builder `_string` and a dynamic list `_vector`.

## Describing `_string`

### ***_string*** - a dynamic string builder allocator
```C
typedef struct _string {
    char *str;
    size_t alloced;
    size_t length;
} _string;
```

#### Mehods:
- Allocating new space with ***`_new_string`***
```C
static _string *_new_string(char *initial_string) {
    _string *sb;
    sb = calloc(1, sizeof(*sb));
    sb->str = malloc(32);
    *sb->str = '\0';
    sb->alloced = 32;
    sb->length = 0;
    _string_add_str(sb, initial_string);
    return sb;
}
```
---
- Adding a `char*` to the builder with ***`_string_add_str`***
```C
static void _string_add_str(_string *sb, const char *str) {
    if(sb == NULL || str == NULL || *str == '\0') return;
    
    const char *ptr = str;
    while(*ptr) ++ptr;
    size_t len = ptr - str;
    
    _string_ensure_space(sb, len);
    memmove(sb->str+sb->length, str, len);

    /* Reset length and NULL terminate */
    sb->length += len;
    sb->str[sb->length] = '\0';
}
```
---
- Adding an `int` with ***`_string_add_int`***
```C
static void _string_add_int(_string *sb, int val) {
    char str[1024];

    if(sb == NULL) return;

    snprintf(str, sizeof(str), "%d", val);
    _string_add_str(sb, str);
}
```
---
- Adding a `double` with ***`_string_add_double_precision`***
```C
static void _string_add_double_precision(_string *sb, double val) {
    char str[1024];

    if(sb == NULL) return;

    /* Use %g for minimum precision on printing floats */
    snprintf(str, sizeof(str), "%g", val);
    _string_add_str(sb, str);
}
```
---
- Getting the string pointer with ***`_string_get`***
```C
static char *_string_get(_string *sb) {
    if(sb == NULL) return NULL;
    return sb->str;
}
```
---
- Removing characters from the end with ***`_string_shorten`***
```C
static void _string_shorten(_string *sb, size_t len) {
    if(sb == NULL || len >= sb->length) return;

    /* Reset the length and NULL terminate, ingoring
        all values right to the NULL from memory */
    sb->length = len;
    sb->str[sb->length] = '\0';
}
```
---
- Removing characters from the beginning with ***`_string_skip`***
```C
static void _string_skip(_string *sb, size_t len) {
    if(sb == NULL || len == 0) return;

    if(len >= sb->length) {
        /* If we choose to drop more bytes than the
            string has simply clear the string */
        _string_delete(sb);
        return;
    }

    sb->length -= len;
    
    /* +1 to move the NULL. */
    memmove(sb->str, sb->str + len, sb->length + 1);
}
```
---
- Resetting the string to `""` with ***`_string_delete`***
```C
static void _string_delete(_string *sb) {
    if(sb == NULL) return;
    _string_shorten(sb, 0);
}
```
---
- Getting a substring of a current builder with `_string_substring`
```C
static _string *_string_substring(_string *str, size_t from, size_t _to_) {
    _string *strdup = _string_dup(str);
    _string_skip(strdup, from);
    _string_shorten(strdup, _to_ - from + 1);
    return strdup;
}
```
---
- Getting a memory duplicate of a current string with `_string_dup`
```C
static _string *_string_dup(_string *sb) {
    if(sb == NULL) return NULL;

    _string *dup = _new_string("");
    _string_add_str(dup, _string_get(sb));
    return dup;
}
```

The lirary uses the `_string` data structure to construct string builders and save them to a hierarchy of vectors.

# Crafting a new assert

The library provides a built in macro for defining new asserts in a modular way. The macro `define_assert` makes it easier to have a streamlined way of defining assertions of any data type. It works as such:

- `define_assert(name_of_assert, <type>, write_to_string_method, datatype_comparison)`

Example:
```C
/* Define our custom `assert_int_func` assert */
define_assert(assert_int_func, int, write_ints, !compare_two_ints)

static void write_ints(int actual, int expected) {
    char actual_buf[1024];
    char expected_buf[1024];
    sprintf(actual_buf, "%d", actual);
    sprintf(expected_buf, "%d", expected);

    /* These lines below should be used in some shape or form */
    _cspec->current_actual = _new_string(actual_buf);
    _cspec->current_expected = _new_string(expected_buf);
}

static bool compare_two_ints(int actual, int expected) {
    return actual == expected;
}
```

Here we define a custom way to assert equality between two `int` values. When writing to the _cspec string variables we add whatever our values are to the equivalent string builders. The reason we use the negation of the comparison method, is because assertions check if the comparison is failing so that error descriptions are triggered.
***This is the way all provided assertions are defined, but you can hardcode your own if you study the code.***

After defining the base functionality we need an *interface* for us to use in the spec files. An interface can be defined as such:

```C
#define assert_int(expression) do { \
    _cspec->current_file = __FILE__; \
    _cspec->current_line = __LINE__; \
    \
    /* The function assert we defined */ \
    assert_int_func(expression); \
} while(0);
```

When we craft asserts we need to provide the user with the file and line constants for displaying the position of the error in the file. We have to define this inside a macro call so that we make sure it gets inlined to the spec file code  properly. Setting file and line values inside a function will provide information about the function itself not the user provided spec code. Now we can successfully call the assert like this:
```C
assert_int(1, 2);
/* or */
assert_int(1 equals to 2);
/* ...and so on... */
```

# Something more complicated

We are going to use the method above to define an assert for arrays of integers. Idealy we would want to call the function as such:

```C
int list_a[3] = {1, 2, 3};
int list_b[3] = {4, 4, 4};
assert_that_int_array(list_a equals to list_b with length 3);
```

### Functionality
```C
static void _to_string_int_array_write(int *actual, int *expected, size_t len) {
    _cspec->current_actual = _new_string("[");
    _cspec->current_expected = _new_string("[");

    for(int i = 0; i < len - 1; i++) {
        _string_add_int(_cspec->current_actual, actual[i]);
        _string_add_str(_cspec->current_actual, ", ");

        _string_add_int(_cspec->current_expected, expected[i]);
        _string_add_str(_cspec->current_expected, ", ");
    }
    _string_add_int(_cspec->current_actual, actual[len - 1]);
    _string_add_str(_cspec->current_actual, "]");

    _string_add_int(_cspec->current_expected, expected[len - 1]);
    _string_add_str(_cspec->current_expected, "]");
}

static bool _int_array_comparison(int *actual, int *expected, size_t len) {
    /* Check for members */
    for(int i = 0; i < len; i++) {
        if(actual[i] != expected[i]) return false;
    }

    /* All elements are equal */
    return true;
}
```
We define a method that displays the array in a readable way with brackets. We also use the build in _string_add_int to write integers to the string builder.\
So an the array: `int arr[5] = {1, 2, 3, 4, 5}` would be displayed as `[1, 2, 3, 4, 5]`.

### Interface
```C
define_assert_array(_call_assert_that_int_array, int*, _to_string_int_array_write, !_int_array_comparison, len)

#define assert_that_int_array(expression) _BLOCK( \
    _cspec->current_file = __FILE__; \
    _cspec->current_line = __LINE__; \
    _call_assert_that_int_array(expression); \
)
```
Here we call the built in `define_assert_array` that also receives the length of the arrays we compare. Obviously if the actual has a different length that the expected array they are not equal (length can be checked with a different assert). After all this, we are done. Maybe complicated, breaks some programming practices, and duplicates code, but remember that without a second preprocessor, or writing to some external device such extension functionality is very difficult to simplify and abstract more.