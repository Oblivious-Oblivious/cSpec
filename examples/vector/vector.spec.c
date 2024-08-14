#include "../../src/cSpec.h"
#include "vector.module.spec.h"

spec_suite({ vector_spec(); })

  int main(void) {
  run_spec_suite("all");
  export_test_results("output.txt", "all", "txt");
}
