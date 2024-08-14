#include "../../src/cSpec.h"
#include "vector.module.spec.h"

int main(void) {
  cspec_run_suite("all", { vector_spec(); });
  cspec_export_test_results("output.txt", "all", "txt");
}
