#ifndef __SIMPLE_MODULE_SPEC_H_
#define __SIMPLE_MODULE_SPEC_H_

#include "../src/cSpec.h"
#include "simple.h"

module(simple, {
  describe("cSpec functions", {
    before({ debug_msg(); });
    before_each(&initializer);

    it("just fails", { fail("This is a failure"); });

    it("succeeds `assert_that`", { assert_that(1 is 1); });
    it("fails `assert_that`", { assert_that(1 isnot 1); });

    it("fails again", { fail("This is a second failure"); });

    it("succeeds `nassert_that`", { nassert_that(2 isnot 2); });
    it("fails `nassert_that`", { nassert_that(2 is 2); });

    it("fails a third time", { fail("This is a third failure"); });

    it("succeeds `assert_that_char`", { assert_that_char('a' equals to 'a'); });
    it("fails `assert_that_char`", { assert_that_char('a' equals to 'b'); });

    it("succeeds `nassert_that_char`", {
      nassert_that_char('a' equals to 'b');
    });
    it("fails `nassert_that_char`", { nassert_that_char('a' equals to 'a'); });

    it("succeeds `assert_that_char_array`", {
      char actual[3]   = {'a', 'b', 'c'};
      char expected[3] = {'a', 'b', 'c'};
      assert_that_char_array(actual equals to expected with array_size 3);
    });
    it("fails `assert_that_char_array`", {
      char actual[3]   = {'a', 'b', 'c'};
      char expected[3] = {'a', 'b', 'd'};
      assert_that_char_array(actual equals to expected with array_size 3);
    });

    it("succeeds `nassert_that_char_array`", {
      char actual[3]   = {'a', 'b', 'c'};
      char expected[3] = {'a', 'b', 'd'};
      nassert_that_char_array(actual equals to expected with array_size 3);
    });
    it("fails `nassert_that_char_array`", {
      char actual[3]   = {'a', 'b', 'c'};
      char expected[3] = {'a', 'b', 'c'};
      nassert_that_char_array(actual equals to expected with array_size 3);
    });

    it("succeeds `assert_that_unsigned_char`", {
      assert_that_unsigned_char(255 equals to 255);
    });
    it("fails `assert_that_unsigned_char`", {
      assert_that_unsigned_char(255 equals to 254);
    });

    it("succeeds `nassert_that_unsigned_char`", {
      nassert_that_unsigned_char(255 equals to 254);
    });
    it("fails `nassert_that_unsigned_char`", {
      nassert_that_unsigned_char(255 equals to 255);
    });

    it("succeeds `assert_that_unsigned_char_array`", {
      unsigned char actual[3]   = {255, 254, 253};
      unsigned char expected[3] = {255, 254, 253};
      assert_that_unsigned_char_array(
        actual equals to expected with array_size 3
      );
    });
    it("fails `assert_that_unsigned_char_array`", {
      unsigned char actual[3]   = {255, 254, 253};
      unsigned char expected[3] = {255, 254, 252};
      assert_that_unsigned_char_array(
        actual equals to expected with array_size 3
      );
    });

    it("succeeds `nassert_that_unsigned_char_array`", {
      unsigned char actual[3]   = {255, 254, 253};
      unsigned char expected[3] = {255, 254, 252};
      nassert_that_unsigned_char_array(
        actual equals to expected with array_size 3
      );
    });
    it("fails `nassert_that_unsigned_char_array`", {
      unsigned char actual[3]   = {255, 254, 253};
      unsigned char expected[3] = {255, 254, 253};
      nassert_that_unsigned_char_array(
        actual equals to expected with array_size 3
      );
    });

    it("succeeds `assert_that_short`", {
      assert_that_short(255 equals to 255);
    });
    it("fails `assert_that_short`", { assert_that_short(255 equals to 254); });

    it("succeeds `nassert_that_short`", {
      nassert_that_short(255 equals to 254);
    });
    it("fails `nassert_that_short`", {
      nassert_that_short(255 equals to 255);
    });

    it("succeeds `assert_that_short_array`", {
      short actual[3]   = {255, 254, 253};
      short expected[3] = {255, 254, 253};
      assert_that_short_array(actual equals to expected with array_size 3);
    });
    it("fails `assert_that_short_array`", {
      short actual[3]   = {255, 254, 253};
      short expected[3] = {255, 254, 252};
      assert_that_short_array(actual equals to expected with array_size 3);
    });

    it("succeeds `nassert_that_short_array`", {
      short actual[3]   = {255, 254, 253};
      short expected[3] = {255, 254, 252};
      nassert_that_short_array(actual equals to expected with array_size 3);
    });
    it("fails `nassert_that_short_array`", {
      short actual[3]   = {255, 254, 253};
      short expected[3] = {255, 254, 253};
      nassert_that_short_array(actual equals to expected with array_size 3);
    });

    it("succeeds `assert_that_unsigned_short`", {
      assert_that_unsigned_short(65535 equals to 65535);
    });
    it("fails `assert_that_unsigned_short`", {
      assert_that_unsigned_short(65535 equals to 65534);
    });

    it("succeeds `nassert_that_unsigned_short`", {
      nassert_that_unsigned_short(65535 equals to 65534);
    });
    it("fails `nassert_that_unsigned_short`", {
      nassert_that_unsigned_short(65535 equals to 65535);
    });

    it("succeeds `assert_that_unsigned_short_array`", {
      unsigned short actual[3]   = {65535, 65534, 65533};
      unsigned short expected[3] = {65535, 65534, 65533};
      assert_that_unsigned_short_array(
        actual equals to expected with array_size 3
      );
    });
    it("fails `assert_that_unsigned_short_array`", {
      unsigned short actual[3]   = {65535, 65534, 65533};
      unsigned short expected[3] = {65535, 65534, 65532};
      assert_that_unsigned_short_array(
        actual equals to expected with array_size 3
      );
    });

    it("succeeds `nassert_that_unsigned_short_array`", {
      unsigned short actual[3]   = {65535, 65534, 65533};
      unsigned short expected[3] = {65535, 65534, 65532};
      nassert_that_unsigned_short_array(
        actual equals to expected with array_size 3
      );
    });
    it("fails `nassert_that_unsigned_short_array`", {
      unsigned short actual[3]   = {65535, 65534, 65533};
      unsigned short expected[3] = {65535, 65534, 65533};
      nassert_that_unsigned_short_array(
        actual equals to expected with array_size 3
      );
    });

    it("succeeds `assert_that_int`", { assert_that_int(1 equals to 1); });
    it("fails `assert_that_int`", { assert_that_int(2 equals to 3); });

    it("succeeds `nassert_that_int`", { nassert_that_int(1 equals to 2); });
    it("fails `nassert_that_int`", { nassert_that_int(4 equals to 4); });

    it("succeeds `assert_that_int_array`", {
      int myactual[3]   = {1, 2, 3};
      int myexpected[3] = {1, 2, 3};
      assert_that_int_array(myactual equals to myexpected with array_size 3);
    });
    it("fails `assert_that_int_array`", {
      int myactual[3]   = {1, 2, 3};
      int myexpected[3] = {1, 2, 4};
      assert_that_int_array(myactual equals to myexpected with array_size 3);
    });

    it("succeeds `nassert_that_int_array`", {
      int myactual[3]   = {1, 2, 3};
      int myexpected[3] = {1, 2, 4};
      nassert_that_int_array(myactual equals to myexpected with array_size 3);
    });
    it("fails `nassert_that_int_array`", {
      int myactual[3]   = {1, 2, 3};
      int myexpected[3] = {1, 2, 3};
      nassert_that_int_array(myactual equals to myexpected with array_size 3);
    });

    it("succeeds `assert_that_long`", {
      assert_that_long(2147483647 equals to 2147483647);
    });
    it("fails `assert_that_long`", {
      assert_that_long(2147483647 equals to 2147483646);
    });

    it("succeeds `nassert_that_long`", {
      nassert_that_long(2147483647 equals to 2147483646);
    });
    it("fails `nassert_that_long`", {
      nassert_that_long(2147483647 equals to 2147483647);
    });

    it("succeeds `assert_that_long_array`", {
      long actual[3]   = {2147483647, 2147483646, 2147483645};
      long expected[3] = {2147483647, 2147483646, 2147483645};
      assert_that_long_array(actual equals to expected with array_size 3);
    });
    it("fails `assert_that_long_array`", {
      long actual[3]   = {2147483647, 2147483646, 2147483645};
      long expected[3] = {2147483647, 2147483646, 2147483644};
      assert_that_long_array(actual equals to expected with array_size 3);
    });

    it("succeeds `nassert_that_long_array`", {
      long actual[3]   = {2147483647, 2147483646, 2147483645};
      long expected[3] = {2147483647, 2147483646, 2147483644};
      nassert_that_long_array(actual equals to expected with array_size 3);
    });
    it("fails `nassert_that_long_array`", {
      long actual[3]   = {2147483647, 2147483646, 2147483645};
      long expected[3] = {2147483647, 2147483646, 2147483645};
      nassert_that_long_array(actual equals to expected with array_size 3);
    });

    it("succeeds `assert_that_unsigned_long`", {
      assert_that_unsigned_long(4294967295 equals to 4294967295);
    });
    it("fails `assert_that_unsigned_long`", {
      assert_that_unsigned_long(4294967295 equals to 4294967294);
    });

    it("succeeds `nassert_that_unsigned_long`", {
      nassert_that_unsigned_long(4294967295 equals to 4294967294);
    });
    it("fails `nassert_that_unsigned_long`", {
      nassert_that_unsigned_long(4294967295 equals to 4294967295);
    });

    it("succeeds `assert_that_unsigned_long_array`", {
      unsigned long actual[3]   = {4294967295, 4294967294, 4294967293};
      unsigned long expected[3] = {4294967295, 4294967294, 4294967293};
      assert_that_unsigned_long_array(
        actual equals to expected with array_size 3
      );
    });
    it("fails `assert_that_unsigned_long_array`", {
      unsigned long actual[3]   = {4294967295, 4294967294, 4294967293};
      unsigned long expected[3] = {4294967295, 4294967294, 4294967292};
      assert_that_unsigned_long_array(
        actual equals to expected with array_size 3
      );
    });

    it("succeeds `nassert_that_unsigned_long_array`", {
      unsigned long actual[3]   = {4294967295, 4294967294, 4294967293};
      unsigned long expected[3] = {4294967295, 4294967294, 4294967292};
      nassert_that_unsigned_long_array(
        actual equals to expected with array_size 3
      );
    });
    it("fails `nassert_that_unsigned_long_array`", {
      unsigned long actual[3]   = {4294967295, 4294967294, 4294967293};
      unsigned long expected[3] = {4294967295, 4294967294, 4294967293};
      nassert_that_unsigned_long_array(
        actual equals to expected with array_size 3
      );
    });

    it("succeeds `assert_that_long_long`", {
      assert_that_long_long(9223372036854775807 equals to 9223372036854775807);
    });
    it("fails `assert_that_long_long`", {
      assert_that_long_long(9223372036854775807 equals to 9223372036854775806);
    });

    it("succeeds `nassert_that_long_long`", {
      nassert_that_long_long(9223372036854775807 equals to 9223372036854775806);
    });
    it("fails `nassert_that_long_long`", {
      nassert_that_long_long(9223372036854775807 equals to 9223372036854775807);
    });

    it("succeeds `assert_that_long_long_array`", {
      long long actual[3] = {
        9223372036854775807, 9223372036854775806, 9223372036854775805
      };
      long long expected[3] = {
        9223372036854775807, 9223372036854775806, 9223372036854775805
      };
      assert_that_long_long_array(actual equals to expected with array_size 3);
    });
    it("fails `assert_that_long_long_array`", {
      long long actual[3] = {
        9223372036854775807, 9223372036854775806, 9223372036854775805
      };
      long long expected[3] = {
        9223372036854775807, 9223372036854775806, 9223372036854775804
      };
      assert_that_long_long_array(actual equals to expected with array_size 3);
    });

    it("succeeds `nassert_that_long_long_array`", {
      long long actual[3] = {
        9223372036854775807, 9223372036854775806, 9223372036854775805
      };
      long long expected[3] = {
        9223372036854775807, 9223372036854775806, 9223372036854775804
      };
      nassert_that_long_long_array(actual equals to expected with array_size 3);
    });
    it("fails `nassert_that_long_long_array`", {
      long long actual[3] = {
        9223372036854775807, 9223372036854775806, 9223372036854775805
      };
      long long expected[3] = {
        9223372036854775807, 9223372036854775806, 9223372036854775805
      };
      nassert_that_long_long_array(actual equals to expected with array_size 3);
    });

    it("succeeds `assert_that_float`", {
      assert_that_float(3.2 equals to 3.2);
    });
    it("fails `assert_that_float`", { assert_that_float(3.2 equals to 4.2); });

    it("succeeds `nassert_that_float`", {
      nassert_that_float(6.6 equals to 7.7);
    });
    it("fails `nassert_that_float`", {
      nassert_that_float(6.6 equals to 6.6);
    });

    it("succeeds `assert_that_float_array`", {
      float actual[3]   = {3.2, 3.3, 3.4};
      float expected[3] = {3.2, 3.3, 3.4};
      assert_that_float_array(actual equals to expected with array_size 3);
    });
    it("fails `assert_that_float_array`", {
      float actual[3]   = {3.2, 3.3, 3.4};
      float expected[3] = {3.2, 3.3, 3.5};
      assert_that_float_array(actual equals to expected with array_size 3);
    });

    it("succeeds `nassert_that_float_array`", {
      float actual[3]   = {3.2, 3.3, 3.4};
      float expected[3] = {3.2, 3.3, 3.5};
      nassert_that_float_array(actual equals to expected with array_size 3);
    });
    it("fails `nassert_that_float_array`", {
      float actual[3]   = {3.2, 3.3, 3.4};
      float expected[3] = {3.2, 3.3, 3.4};
      nassert_that_float_array(actual equals to expected with array_size 3);
    });

    it("succeeds `assert_that_double`", {
      assert_that_double(3.2 equals to 3.2);
    });
    it("fails `assert_that_double`", {
      assert_that_double(3.2 equals to 4.2);
    });

    it("succeeds `nassert_that_double`", {
      nassert_that_double(6.6 equals to 7.7);
    });
    it("fails `nassert_that_double`", {
      nassert_that_double(6.6 equals to 6.6);
    });

    it("succeeds `assert_that_double_array`", {
      double actual[3]   = {3.2, 3.3, 3.4};
      double expected[3] = {3.2, 3.3, 3.4};
      assert_that_double_array(actual equals to expected with array_size 3);
    });
    it("fails `assert_that_double_array`", {
      double actual[3]   = {3.2, 3.3, 3.4};
      double expected[3] = {3.2, 3.3, 3.5};
      assert_that_double_array(actual equals to expected with array_size 3);
    });

    it("succeeds `nassert_that_double_array`", {
      double actual[3]   = {3.2, 3.3, 3.4};
      double expected[3] = {3.2, 3.3, 3.5};
      nassert_that_double_array(actual equals to expected with array_size 3);
    });
    it("fails `nassert_that_double_array`", {
      double actual[3]   = {3.2, 3.3, 3.4};
      double expected[3] = {3.2, 3.3, 3.4};
      nassert_that_double_array(actual equals to expected with array_size 3);
    });

    it("succeeds `assert_that_int`", { assert_that_int(1 equals to 1); });
    it("fails `assert_that_int`", { assert_that_int(2 equals to 3); });

    it("succeeds `nassert_that_int`", { nassert_that_int(1 equals to 2); });
    it("fails `nassert_that_int`", { nassert_that_int(4 equals to 4); });

    it("succeeds `assert_that_double`", {
      assert_that_double(3.2 equals to 3.2);
    });
    it("fails `assert_that_double`", {
      assert_that_double(3.2 equals to 4.2);
    });

    xit("SKIP THAT TEST", { assert_that_value(1 is 1); });

    it("succeeds `nassert_that_double`", {
      nassert_that_double(6.6 equals to 7.7);
    });
    it("fails `nassert_that_double`", {
      nassert_that_double(6.6 equals to 6.6);
    });

    it("succeeds `assert_that_charptr`", {
      assert_that_charptr("testme" equals to "testme");
    });
    it("fails `assert_that_charptr`", {
      assert_that_charptr("testme" equals to "nottestme");
    });

    it("succeeds `nassert_that_charptr`", {
      nassert_that_charptr("testme" equals to "notthepreviousvariable");
    });
    it("fails `nassert_that_charptr`", {
      nassert_that_charptr("ok str" equals to "ok str");
    });

    it("succeeds `assert_that_charptr_array`", {
      char *actual[3]   = {(char *)"str1", (char *)"str2", (char *)"str3"};
      char *expected[3] = {(char *)"str1", (char *)"str2", (char *)"str3"};
      assert_that_charptr_array(actual equals to expected with array_size 3);
    });
    it("fails `assert_that_charptr_array`", {
      char *actual[3]   = {(char *)"str1", (char *)"str2", (char *)"str3"};
      char *expected[3] = {(char *)"str1", (char *)"str2", (char *)"str4"};
      assert_that_charptr_array(actual equals to expected with array_size 3);
    });

    it("succeeds `nassert_that_charptr_array`", {
      char *actual[3]   = {(char *)"str1", (char *)"str2", (char *)"str3"};
      char *expected[3] = {(char *)"str1", (char *)"str2", (char *)"str4"};
      nassert_that_charptr_array(actual equals to expected with array_size 3);
    });
    it("fails `nassert_that_charptr_array`", {
      char *actual[3]   = {(char *)"str1", (char *)"str2", (char *)"str3"};
      char *expected[3] = {(char *)"str1", (char *)"str2", (char *)"str3"};
      nassert_that_charptr_array(actual equals to expected with array_size 3);
    });

    after_each(&destructor);
    after({ teardown_msg(); });
  });
})

#endif
