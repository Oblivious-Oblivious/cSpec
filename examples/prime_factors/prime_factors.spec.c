#include "../../src/cSpec.h"
#include "prime_factors.h"

#include "primes.module.h"

spec_suite({
    primes();
});

spec({
    run_spec_suite("all");
});