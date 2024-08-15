#include "simple.h"

#include <stdio.h> /* printf */

void debug_msg(void) { printf("This is called before all tests\n"); }

void teardown_msg(void) { printf("This is called after all tests\n"); }

void initializer(void) {}

void destructor(void) {}
