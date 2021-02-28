#include "../../export/cSpec.h"
#include "prime_factors.h"

#include "prime_factors.module.h"

spec_suite({
    primes();
});

spec({
    run_spec_suite("all");
});
