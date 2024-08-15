#include "../../src/cSpec.h"
#include "string_base.module.spec.h"

int main(void) {
  cspec_run_suite("all", { string_spec(); });
}
