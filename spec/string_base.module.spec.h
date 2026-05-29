#ifndef __STRING_BASE_MODULE_SPEC_H_
#define __STRING_BASE_MODULE_SPEC_H_

#include "../src/cSpec.h"

typedef struct string {
  char *str;
  size_t alloced;
  size_t len;
} string;

static const size_t string_init_capacity = 32;

static void string_ensure_space(string *sb, size_t add_len) {
  if(sb == NULL || add_len == 0) {
    return;
  }

  if(sb->alloced >= sb->len + add_len + 1) {
    return;
  }

  while(sb->alloced < sb->len + add_len + 1) {
    sb->alloced <<= 1;
    if(sb->alloced == 0) {
      sb->alloced--;
    }
  }
  sb->str = (char *)realloc(sb->str, sb->alloced);
}

static void string_add_str(string *sb, const char *str) {
  size_t len;

  if(sb == NULL || str == NULL || *str == '\0') {
    return;
  }

  len = strlen(str);
  string_ensure_space(sb, len);
  memmove(sb->str + sb->len, str, len);
  sb->len += len;
  sb->str[sb->len] = '\0';
}

static string *string_create(char *initial_string) {
  string *sb = (string *)malloc(sizeof(*sb));
  sb->str    = (char *)malloc(sizeof(char) * string_init_capacity);

  *sb->str    = '\0';
  sb->alloced = string_init_capacity;
  sb->len     = 0;

  string_add_str(sb, initial_string);
  return sb;
}

#define new_string(initial_string) string_create(initial_string)

static char *string_get(string *sb) {
  if(sb == NULL) {
    return NULL;
  }
  return sb->str;
}

static void string_free(string *sb) {
  free(sb->str);
  free(sb);
}

module(T_string_base, {
  describe("string", {
    string *str;
    char *initial_value;

    before({
      str           = NULL;
      initial_value = (char *)"initial";
    });

    it("creates a string with an initial char* using `nassert_that`", {
      str = new_string(initial_value);
      nassert_that(str is NULL);
    });

    context("when a string pointer is not NULL", {
      it("returns the char* when calling `string_get`", {
        assert_that(sizeof(string_get(str)) is sizeof(char *));
      });
    });

    context("when the initial value exitsts", {
      it("returns the correct char* when calling `string_get`", {
        assert_that_charptr(string_get(str) equals to initial_value);
      });
    });

    after({ string_free(str); });
  });
})

#endif
