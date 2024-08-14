#include "../../src/cSpec.h"
#include "prime_factors.module.h"

int main(void) {
  cspec_run_suite("all", { primes(); });
}
