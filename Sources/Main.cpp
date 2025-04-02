
#include <Tracing/Tracing.h>

#include "Lexer.h"
#include "Parser.h"
#include "Repl.h"

struct TraceContext TC;

#ifdef MYL_TEST

#include <Test/Test.h>

extern void testSymbolInterner( Tm42_TestContext * ctx );

extern void testLexEatIdent( Tm42_TestContext * ctx );
extern void testLexEatNumber( Tm42_TestContext * ctx );
extern void testLexLabel( Tm42_TestContext * ctx );
extern void testLexLex( Tm42_TestContext * ctx );

extern void testParseCons( Tm42_TestContext * ctx );
extern void testParseProc( Tm42_TestContext * ctx );

int
main() {
    init_tracing( &TC, stdout, "Main" );
    t0( &TC, "Tracing initialized." );

    Tm42_TestContext ctx;

    testSymbolInterner( &ctx );

    testLexEatIdent( &ctx );
    testLexEatNumber( &ctx );
    testLexLabel( &ctx );
    testLexLex( &ctx );

    testParseCons( &ctx );
    testParseProc( &ctx );
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

      auto lexer = Lexer( *input );
      auto lexResult = lexer.lex();
      std::cout << "--- LEX ---\n";
      std::cout << "Lexed " << lexResult.tokens.size() << " tokens\n";
      if ( lexResult.error ) {
          continue;
      }

      auto parser = Parser( *input, lexResult.tokens );
      auto ast = parser.parse();
      std::cout << "--- INITIAL PARSE ---\n";
      std::cout << "Parsed " << ast.sexprs.size() << " nodes\n";
      for ( const auto & sexpr : ast.sexprs ) {
          std::cout << *sexpr << "\n";
      }
  }

  deinit_tracing(&TC);
  return 0;
}

#endif // MYL_TEST
