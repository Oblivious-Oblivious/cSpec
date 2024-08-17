#include "../src/cSpec.h"
#include "prime_factors.module.h"
#include "second.module.spec.h"
#include "simple.module.spec.h"
#include "stack.module.spec.h"
#include "string_base.module.spec.h"
#include "vector.module.spec.h"
#include "xdecribexcontext.module.spec.h"
#include "xexample.module.spec.h"

int main(void) {
  cspec_run_suite("all", {
    primes();
    simple();
    second();
    stack_spec();
    string_spec();
    xdescribexcontext();
    xexample();
    vector_spec();
  });
}
