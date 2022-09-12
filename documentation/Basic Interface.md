------------------
- ### ***`spec`***
```C
spec({
    /* code */
});
```
A `main` function wrapper making the testing commands and code more concise.

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
Defines a top-level function where test modules are run. Because the library's
basic functionality relies on appending arbitrary macro calls, at least one
top-level routine must be declared. The purpose of `spec_suite` is to run
user selected test modules but can be used to also setup function level
variables or allocations.

------------------------
- ### ***`before`***
```C
before({
    /* code */
});
```
Inlines a code block to be executed at the ***top*** of describe blocks.
Mainly used for setting up values values.

------------------------
- ### ***`after`***
```C
after({
    /* code */
});
```
Inlines a code block to be executed at the ***end*** of describe blocks.
Mainly used for teardown purposes.

------------------------
- ### ***`before_each`***
```C
before_each({
    /* code */
});
```
Inlines a code block to be executed ***before each `it` block***.
Mainly used to repeat some initiation of state.

------------------------
- ### ***`after_each`***
```C
after_each({
    /* code */
});
```
Inlines a code block to be executed ***after each `it` block***.
Mainly used for teardown purposes.

------------------------
- ### ***`module`***
```C
module(func_name, {
    describe("name 1", {});
    describe("name 2", {});
    /* ... */
});
```
Defines a test module, containing one or multiple `describe` blocks.
This allows for modules to exist in separate files for better management and inclusion of tests.

------------------------
- ### ***`describe`***
```C
describe("describe name", {
    it("does something", {});
    it("does something else", {});
    /* ... */
});
```
Defines a code block for grouping tests. Best use of the `describe` block
would be testing for 'things' like objects or variables or anything that has semantic meaning.

------------------------
- ### ***`context`***
```C
context("context name", {
    it("tests some state", {});
    it("tests some more state", {});
    /* ... */
});
```
Defines a code block for grouping tests. Best use of the `context` block
would be testing for 'states' of data and their interaction.

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

The `it` block defines the basic declaration of a test procedure. You can
call an `it` with one or multiple `assert` blocks. The test will be
considered *passing* only if all asserts are true. Finding even one
failing assert will make the test failing as well,
and the test result will contain information about the error.

------------------------
- ### ***`xit`***
```C
xit("name of skipped it block", {
    /* ... */
});
```

The `xit` blocks simply declares that all following asserts inside the block
will be skipped. Immediately the test becomes a skipped one and its asserts are not executed.

------------------------
- ### ***`export_test_results`***
```C
#define export_test_results(name, vec, type) CSPEC_BLOCK( \
    /* Check for valid type of test export */ \
    if(!cspec_streql(vec, "passing") \
    && !cspec_streql(vec, "failing") \
    && !cspec_streql(vec, "skipped") \
    && !cspec_streql(vec, "all")) { \
        printf("\n%sInput a type of test to export passing|failing|skipped|all%s\n\n", \
        cspec_string_get(cspec->RED), \
        cspec_string_get(cspec->RESET)); \
        return 0; \
    } \
    cspec->type_of_export_tests = cspec_string_new(vec); \
    \
    /* Reset the display tab */ \
    cspec_string_delete(cspec->display_tab); \
    cspec_string_add_str(cspec->display_tab, "    "); \
    if(type == "txt") { \
        cspec->fd = fopen(name, "w+"); \
        cspec_export_to_txt(); \
    } \
    else if(type == "xml") { \
        cspec->fd = fopen(name, "w+"); \
        cspec_export_to_xml(); \
    } \
    else if(type == "markdown") { \
        cspec->fd = fopen(name, "w+"); \
        cspec_export_to_md(); \
    } \
    else { \
        printf("\n%sSpecify the export type: `txt|xml|markdown`%s\n\n", \
        cspec_string_get(cspec->RED), \
        cspec_string_get(cspec->RESET)); \
        return 0; /* Exit the main function */ \
    } \
)
```

Export test results gathered through running assertions and formats it to a user specified format

-------------

## Assertions
- ### ***`assert_that`***
```C
#define assert_that(test) CSPEC_BLOCK( \
    /* Assert a custom block of code */ \
	if(!(test)) { \
        cspec->status_of_test = CSPEC_FAILING; \
        cspec_write_position_in_file(); \
        cspec_write_assert(); \
    } \
    \
    /* assert(string_builder) */ \
)
```
Basic assert declaration. Asserts a test block for truthiness.

--------------------------------------------------------------
