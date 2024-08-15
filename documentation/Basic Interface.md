- ### **_`before`_**

```C
before({
  /* code */
});
```

Inlines a code block to be executed at the **_top_** of describe blocks.
Mainly used for setting up values values.

---

- ### **_`after`_**

```C
after({
  /* code */
});
```

Inlines a code block to be executed at the **_end_** of describe blocks.
Mainly used for teardown purposes.

---

- ### **_`before_each`_**

```C
before_each({
  /* code */
});
```

Inlines a code block to be executed **_before each `it` block_**.
Mainly used to repeat some initiation of state.

---

- ### **_`after_each`_**

```C
after_each({
  /* code */
});
```

Inlines a code block to be executed **_after each `it` block_**.
Mainly used for teardown purposes.

---

- ### **_`module`_**

```C
module(func_name, {
  describe("name 1", {});
  describe("name 2", {});
  /* ... */
})
```

Defines a test module, containing one or multiple `describe` blocks.
This allows for modules to exist in separate files for better management and inclusion of tests.

---

- ### **_`describe`_**

```C
describe("describe name", {
  it("does something", {});
  it("does something else", {});
  /* ... */
});
```

Defines a code block for grouping tests. Best use of the `describe` block
would be testing for 'things' like objects or variables or anything that has semantic meaning.

---

- ### **_`context`_**

```C
context("context name", {
  it("tests some state", {});
  it("tests some more state", {});
  /* ... */
});
```

Defines a code block for grouping tests. Best use of the `context` block
would be testing for 'states' of data and their interaction.

---

- ### **_`it`_**

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
considered _passing_ only if all asserts are true. Finding even one
failing assert will make the test failing as well,
and the test result will contain information about the error.

---

- ### **_`xit`_**

```C
xit("name of skipped it block", {
  /* ... */
});
```

The `xit` blocks simply declares that all following asserts inside the block
will be skipped. Immediately the test becomes a skipped one and its asserts are not executed.

---
