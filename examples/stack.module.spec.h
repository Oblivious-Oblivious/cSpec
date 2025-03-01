#ifndef __STACK_MODULE_SPEC_H_
#define __STACK_MODULE_SPEC_H_

#include "../src/cSpec.h"
#include "stack.h"

static stack *st;

static void setup(void) { st = new_stack(); }
static void defer(void) { stack_free(st); }

module(stack_spec, {
  before_each(&setup);
  after_each(&defer);

  it("asserts that a new stack is empty", { assert_that(stack_is_empty(st)); });

  it("pushes an element into the stack", {
    stack_push(st, 0);
    nassert_that(stack_is_empty(st));
  });

  it("pops an empty stack and returns NULL", { stack_pop(st); });

  it("is empty after 1 push and 1 pop", {
    stack_push(st, 0);
    stack_pop(st);
    assert_that(stack_is_empty(st));
  });

  it("is not empty after 2 pushes and 1 pop", {
    stack_push(st, 0);
    stack_push(st, 0);
    stack_pop(st);
    nassert_that(stack_is_empty(st));
  });

  it("pops `x` after pushing `x`", {
    stack_push(st, 99);
    assert_that_int(stack_pop(st) equals to 99);
    stack_push(st, 88);
    assert_that_int(stack_pop(st) equals to 88);
  });

  it("pops `y` then `x` after pushing `x` then `y`", {
    stack_push(st, 99);
    stack_push(st, 88);
    assert_that_int(stack_pop(st) equals to 88);
    assert_that_int(stack_pop(st) equals to 99);
  });
})

#endif
