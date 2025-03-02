
#include <Tracing/Tracing.h>

#include "Repl.h"

static struct TraceContext TC;

int
main() {
  init_tracing( &TC, stdout, "Main" );
  t0( &TC, "Tracing initialized." );

  while ( true ) {
      auto input = getInputBasicRepl();
      if ( !input ) {
          t0( &TC, "Closing REPL..." );
          break;
      }
      t0( &TC, "Read input: %s", input->c_str() );
  }

  deinit_tracing(&TC);
  return 0;
}
