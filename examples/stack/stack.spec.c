#include "../../src/cSpec.h"
#include "stack.module.spec.h"

int main(void) {
  cspec_run_suite("all", { stack_spec(); });
}
