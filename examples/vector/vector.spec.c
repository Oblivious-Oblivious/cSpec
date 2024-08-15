#include "../../src/cSpec.h"
#include "vector.module.spec.h"

int main(void) {
  cspec_run_suite("all", { vector_spec(); });
}
