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
})
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

--------------------------------------------------------------
