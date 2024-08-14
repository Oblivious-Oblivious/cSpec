#include "../../src/cSpec.h"
#include "second.module.spec.h"
#include "simple.module.spec.h"

int main(void) {
  cspec_run_suite("all", {
    simple();
    second();
  });
}
