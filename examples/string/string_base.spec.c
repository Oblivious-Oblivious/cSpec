#include "../../src/cSpec.h"
#include "string_base.module.spec.h"

spec_suite({ string_spec(); });

int main(void) {
  run_spec_suite("all");
  export_test_results("output.txt", "all", "txt");
}
