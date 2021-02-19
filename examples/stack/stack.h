#ifndef __STACK_H_
#define __STACK_H_

#include <stdlib.h>

typedef unsigned char bool;
#define true 1
#define false 0

typedef struct stack {
    size_t size;
    int elements[2];
} stack;

stack *new_stack(void);
bool stack_is_empty(stack *st);
void stack_push(stack *st, int element);
int stack_pop(stack *st);

#endif

