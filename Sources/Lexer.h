#pragma once

#include <cstdint>
#include <string>
#include <variant>
#include <vector>

#include "Error.h"
#include "Intern.h"

using I32 = std::int32_t;
using F64 = double;

enum class TokenKind {
    LPAREN,
    RPAREN,
    IDENT,
    INT32,
    FLOAT64,
};

const char * tokenKindStr( TokenKind tk );

typedef std::variant< InternedSymbol, I32, F64 > TokenData;

struct Token {
    TokenKind kind;
    TokenData data;
    SourceCodeLocation loc;
};

class Lexer {
 public:
    struct Result {
        std::vector< Token > tokens;
        bool error;
    };

    Lexer( const std::string & input );
    Result lex();

  std::string_view getStringView( SourceCodeLocation loc );

  // Detect if we are at the end of the input.
  bool endOfInput();
  // Sets codepoint and codepointSize. Does not advance byte offset.
  void readCodepoint();
  // Advance input to the next codepoint, based on the previous codepoint size
  // computed by `readCodepoint()`.
  void advanceReadCodepoint();
  // Advances byte offset.
  void eatWhitespace();
  // Assumes currentByteOffset points to the beginning of the identifier.
  // Afterwards, currentByteOffset will point to just after the end of the
  // identifier.
  //
  // TOKEN := [:al][:alnum]*
  Token eatIdent();
  // Assumes `currentByteOffset` points the the first byte of the number.
  // Leaves it pointing to right after the number
  //
  // INT32 := (-)?[0-9]+
  // FLOAT64 := (-)?[0-9]+(.[0-9]*)?
  Token eatNumber();
  Token eatToken();

  const std::string & m_input;
  int m_currentByteOffset = 0;
  int m_codepoint = 0;
  int m_codepointSize = 0;
    bool error = false;
    std::shared_ptr< SymbolInterner > symbolInterner;
};
