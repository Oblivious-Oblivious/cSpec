#include "stack.h"

stack *new_stack(void) {
  stack *st = (stack*)malloc(sizeof(stack) + 1);
  st->size = 0;

  return st;
}

bool stack_is_empty(stack *st) {
  return st->size == 0;
}

void stack_push(stack *st, int element) {
  st->elements[st->size++] = element;
}

int stack_pop(stack *st) {
  if(stack_is_empty(st)) return 0;
  return st->elements[--st->size];
}
