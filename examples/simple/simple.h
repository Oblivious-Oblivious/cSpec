#ifndef __SIMPLE_H_
#define __SIMPLE_H_

#include <stdio.h>

typedef struct data {
    int i;
    double d;
    char *s;
} data;

void debug_msg(void);
void teardown_msg(void);
void initializer(void);
void destructor(void);

#endif