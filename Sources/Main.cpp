
#include <Tracing/Tracing.h>

#include "Repl.h"

struct TraceContext TC;

#ifdef MYL_TEST

#include <Test/Test.h>

extern void testLexEatIdent( Tm42_TestContext * ctx );
extern void testLexEatNumber( Tm42_TestContext * ctx );
extern void testLexLex( Tm42_TestContext * ctx );

int
main() {
    init_tracing( &TC, stdout, "Main" );
    t0( &TC, "Tracing initialized." );

    Tm42_TestContext ctx;
    testLexEatIdent( &ctx );
    testLexEatNumber( &ctx );
    testLexLex( &ctx );
}

#else // MYL_TEST

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

#endif // MYL_TEST
