#ifndef __SECOND_MODULE_SPEC_H_
#define __SECOND_MODULE_SPEC_H_

#include "../src/cSpec.h"

module(second, {
  describe("DESC 1", {
    it("before on desc 1", { assert_that(42 isnot 69); });

    describe("DESC 2", {
      describe("DESC 3", { it("does on 3", { assert_that(3 is 3); }); });
      it("does on 2", { assert_that(2 is 2); });
    });
    it("does on 1", { assert_that(1 is 1); });
  });

  describe("Array Assertions", {
    int a[5] = {1, 2, 3, 4, 5};
    int b[5] = {7, 7, 7, 7, 7};

    double aa[5] = {1.1, 2.2, 3.3, 4.4, 5.5};
    double bb[5] = {7.7, 7.7, 7.7, 7.7, 7.7};

    char *aaa[5] = {
      (char *)"a", (char *)"b", (char *)"c", (char *)"d", (char *)"e"
    };
    char *bbb[5] = {
      (char *)"g", (char *)"g", (char *)"g", (char *)"g", (char *)"g"
    };

    it("succeeds `assert_that_int_array`", {
      int my_arr[5] = {1, 2, 3, 4, 5};
      assert_that_int_array(my_arr equals to a with array_size 5);
    });
    it("fails `assert_that_int_array`", {
      assert_that_int_array(a equals to b with array_size 5);
    });

    it("succeeds `nassert_that_int_array`", {
      int my_arr2[5] = {1, 2, 3, 4, 5};
      nassert_that_int_array(my_arr2 equals to b with array_size 5);
    });
    it("fails `nassert_that_int_array`", {
      int my_arr2[5] = {7, 7, 7, 7, 7};
      nassert_that_int_array(my_arr2 equals to b with array_size 5);
    });

    it("succeeds `assert_that_double_array`", {
      double my_arr[5] = {1.1, 2.2, 3.3, 4.4, 5.5};
      assert_that_double_array(my_arr equals to aa with array_size 5);
    });
    it("fails `assert_that_double_array`", {
      assert_that_double_array(aa equals to bb with array_size 5);
    });

    it("succeeds `nassert_that_double_array`", {
      double my_arr2[5] = {1.1, 2.2, 3.3, 4.4, 5.5};
      nassert_that_double_array(my_arr2 equals to bb with array_size 5);
    });
    it("fails `nassert_that_double_array`", {
      double my_arr2[5] = {7.7, 7.7, 7.7, 7.7, 7.7};
      nassert_that_double_array(my_arr2 equals to bb with array_size 5);
    });

    it("succeeds `assert_that_charptr_array`", {
      char *my_arr[5] = {
        (char *)"a", (char *)"b", (char *)"c", (char *)"d", (char *)"e"
      };
      assert_that_charptr_array(my_arr equals to aaa with array_size 5);
    });
    it("fails `assert_that_charptr_array`", {
      assert_that_charptr_array(aaa equals to bbb with array_size 5);
    });

    it("succeeds `nassert_that_charptr_array`", {
      char *my_arr2[5] = {
        (char *)"g", (char *)"g", (char *)"g", (char *)"g", (char *)"g"
      };
      nassert_that_charptr_array(aaa equals to my_arr2 with array_size 5);
    });
    it("fails `nassert_that_charptr_array`", {
      char *my_arr3[5] = {
        (char *)"g", (char *)"g", (char *)"g", (char *)"g", (char *)"g"
      };
      nassert_that_charptr_array(my_arr3 equals to bbb with array_size 5);
    });
  });
})

#endif
