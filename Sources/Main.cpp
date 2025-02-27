#include "Tracing/Tracing.h"

struct TraceContext TC;

int
main() {
  init_tracing(&TC, stdout, "Main");

  t0(&TC, "Hello, %s!", "world");

  deinit_tracing(&TC);
  return 0;
}
