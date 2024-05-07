#include "../../export/cSpec.h"
#include "prime_factors.module.h"

spec_suite({ primes(); });

int main(void) { run_spec_suite("all"); }
