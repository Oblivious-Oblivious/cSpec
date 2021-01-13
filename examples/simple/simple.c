#include "simple.h"

data dt;

void debug_msg(void) {
    printf("This is called before all tests\n");
}

void teardown_msg(void) {
    printf("This is called after all tests\n");
}

void initializer(void) {
    dt.i = 1;
    dt.d = 2.0;
    dt.s = "str";
}

void destructor(void) {
    dt.i = -1;
    dt.d = 0;
    dt.s = (void*)0;
}

