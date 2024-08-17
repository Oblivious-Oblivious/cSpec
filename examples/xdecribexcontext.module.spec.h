#ifndef __XDESCRIBEXCONTEXT_MODULE_SPEC_H_
#define __XDESCRIBEXCONTEXT_MODULE_SPEC_H_

#include "../src/cSpec.h"

module(xdescribexcontext, {
  xdescribe("ignore this describe", {
    it("does on 1", { assert_that(1 is 1); });
    it("does on 2", { assert_that(2 is 2); });
    it("does on 3", { assert_that(3 is 3); });
    it("does on 4", { assert_that(4 is 4); });
  });

  xcontext("ignore this context", {
    it("does on 1", { assert_that(1 is 1); });
    it("does on 2", { assert_that(2 is 2); });
    it("does on 3", { assert_that(3 is 3); });
    it("does on 4", { assert_that(4 is 4); });
  });
})

#endif
