#ifndef __STRING_H_
#define __STRING_H_

#include <stdio.h>  /* printf, snprintf */
#include <stdlib.h> /* malloc, calloc, realloc, free */
#include <string.h> /* memmove, strlen */

#ifndef bool
  #define bool  unsigned char
  #define true  1
  #define false 0
#endif

static const size_t string_init_capacity = 32;

typedef struct string {
  char *str;
  size_t alloced;
  size_t len;
} string;

static void string_ensure_space(string *sb, size_t add_len);

string *string_create(char *initial_string);
#define new_string(initial_string) string_create(initial_string)

void string_add_str(string *sb, const char *str);
void string_add_char(string *sb, char c);
void string_add_int(string *sb, int val);
void string_add_double_precision(string *sb, double val);
char *string_get(string *sb);
char string_get_char_at_index(string *sb, size_t index);
void string_shorten(string *sb, size_t len);
void string_delete(string *sb);
void string_skip(string *sb, size_t len);
size_t string_length(string *sb);

#endif
