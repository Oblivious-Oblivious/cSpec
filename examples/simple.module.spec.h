#ifndef __SIMPLE_MODULE_SPEC_H_
#define __SIMPLE_MODULE_SPEC_H_

#include "../src/cSpec.h"
#include "simple.h"

module(simple, {
  describe("cSpec functions", {
    before({ debug_msg(); });
    before_each(&initializer);

    it("succeeds `assert_that`", { assert_that(1 is 1); });
    it("fails `assert_that`", { assert_that(1 isnot 1); });

    it("succeeds `nassert_that`", { nassert_that(2 isnot 2); });
    it("fails `nassert_that`", { nassert_that(2 is 2); });

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

    after_each(&destructor);
    after({ teardown_msg(); });
  });
})

#endif
