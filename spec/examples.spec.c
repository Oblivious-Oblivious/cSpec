#include "../src/cSpec.h"
#include "./prime_factors.module.spec.h"
#include "./second.module.spec.h"
#include "./simple.module.spec.h"
#include "./stack.module.spec.h"
#include "./string_base.module.spec.h"
#include "./vector.module.spec.h"
#include "./xdecribexcontext.module.spec.h"
#include "./xexample.module.spec.h"

int main(void) {
  cspec_run_suite("all", {
    T_primes();
    T_simple();
    T_second();
    T_stack();
    T_string_base();
    T_xdescribexcontext();
    T_xexample();
    T_vector();
  });
}
