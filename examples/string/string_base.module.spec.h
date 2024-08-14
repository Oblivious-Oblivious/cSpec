#ifndef __STRING_BASE_MODULE_SPEC_H_
#define __STRING_BASE_MODULE_SPEC_H_

#include "../../src/cSpec.h"
#include "string_base.h"

module(string_spec, {
  describe("string", {
    string *str;
    char *initial_value;

    before({
      str           = NULL;
      initial_value = (char *)"initial";
    });

    it("creates a string with an initial char* using `nassert_that`", {
      str = new_string(initial_value);
      assert_that(str isnot NULL);
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

    after({});
  });
})

#endif
