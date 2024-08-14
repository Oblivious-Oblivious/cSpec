#include "../../src/cSpec.h"
#include "second.module.spec.h"
#include "simple.module.spec.h"

spec_suite({
  simple();
  second();
});

int main(void) { run_spec_suite("all"); }
