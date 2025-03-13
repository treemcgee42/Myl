
#include <cctype>

#ifdef MYL_TEST
#include <cmath>
#include <Test/Test.h>
#endif
#include <Tracing/Tracing.h>
#include <Unicode/Unicode.h>

#include "Lexer.h"

extern struct TraceContext TC;

std::string_view
Lexer::getStringView( SourceCodeLocation loc ) {
    return std::string_view( this->m_input ).substr(
        loc.byteOffset, loc.byteLength );
}

bool
Lexer::endOfInput() {
    return this->m_currentByteOffset >= this->m_input.size();
}

void
Lexer::readCodepoint() {
    this->m_codepointSize = tm42::utf8::readCodepoint(
        this->m_input.c_str() + ( this->m_currentByteOffset ),
        &this->m_codepoint );
}

void
Lexer::advanceReadCodepoint() {
    this->m_currentByteOffset += this->m_codepointSize;
}

void
Lexer::eatWhitespace() {
    if ( this->endOfInput() ) {
        return;
    }
    this->readCodepoint();
    while ( !this->endOfInput() && std::isspace( m_codepoint ) ) {
        this->advanceReadCodepoint();
        this->readCodepoint();
    }
}

static bool
isValidIdentStart( int codepoint ) {
    return std::isalpha( codepoint );
}

static bool
isValidIdentContinuation( int codepoint ) {
    return std::isalnum( codepoint );
}

Token
Lexer::eatIdent() {
    t1( &TC, "" );
    const int initialByteOffset = this->m_currentByteOffset;
    t9( &TC, "initialByteOffset: %d", initialByteOffset );
    this->readCodepoint();
    tassert( &TC, isValidIdentStart( this->m_codepoint ), "" );
    do {
        this->advanceReadCodepoint();
        this->readCodepoint();
    } while ( !this->endOfInput() &&
              isValidIdentContinuation( this->m_codepoint ) );

    return Token {
        TokenKind::IDENT,
        TokenData {},
        SourceCodeLocation{
            initialByteOffset,
            this->m_currentByteOffset - initialByteOffset
        }
    };
}

#ifdef MYL_TEST
void
testLexEatIdent( Tm42_TestContext * ctx ) {
    TM42_BEGIN_TEST( "Lex identifiers" );

    {
        auto lexer = Lexer( "foo" );
        const auto token = lexer.eatIdent();
        TM42_TEST_ASSERT( ctx, token.kind == TokenKind::IDENT );
        TM42_TEST_ASSERT( ctx, lexer.getStringView( token.loc ) == "foo" );
        TM42_TEST_ASSERT( ctx, token.loc.byteOffset == 0 );
        TM42_TEST_ASSERT( ctx, token.loc.byteLength == 3 );
    }

    TM42_END_TEST();
}
#endif

// Assumes `currentByteOffset` points the the first byte of the number.
// Leaves it pointing to right after the number
//
// INT32 := (-)?[0-9]+
// FLOAT64 := (-)?[0-9]+(.[0-9]*)?
Token
Lexer::eatNumber() {
    const int initialByteOffset = this->m_currentByteOffset;
    bool foundDigit = false;
    TokenKind tokenKind = TokenKind::INT32;

    // Parse non-decimal part.
    this->readCodepoint();
    if ( std::isdigit( this->m_codepoint ) ) {
        foundDigit = true;
    }
    tassert( &TC, foundDigit || ( this->m_codepoint == '-' ),
             "Expected a digit or '-', found codepoint '%c'",
             this->m_codepoint );
    bool justReadADigit;
    do {
        this->advanceReadCodepoint();
        this->readCodepoint();
        justReadADigit = std::isdigit( this->m_codepoint );
        foundDigit = foundDigit || justReadADigit;
    } while ( !this->endOfInput() && justReadADigit );
    tassert( &TC, foundDigit, "" );

    // Potentially parse decimal part.
    if ( !this->endOfInput() && this->m_codepoint == '.' ) {
        tokenKind = TokenKind::FLOAT64;
        do {
            this->advanceReadCodepoint();
            this->readCodepoint();
        } while ( !this->endOfInput() && std::isdigit( this->m_codepoint ) );
    }

    // Compute value, return token.
    TokenData tokenData;
    int stringLen = this->m_currentByteOffset - initialByteOffset;
    if ( tokenKind == TokenKind::FLOAT64 ) {
        t9( &TC, "Parsing '%s' to a float", m_input.substr( initialByteOffset, stringLen ).c_str() );
        tokenData = std::stof(
            this->m_input.substr( initialByteOffset, stringLen ) );
    } else {
        t9( &TC, "Parsing '%s' to an int", m_input.substr( initialByteOffset, stringLen ).c_str() );
        tokenData = std::stoi(
            this->m_input.substr( initialByteOffset, stringLen ) );
    }

    return Token {
        tokenKind,
        tokenData,
        SourceCodeLocation { initialByteOffset, stringLen }
    };
}

#ifdef MYL_TEST
#define EPSILON 0.00001
void
testLexEatNumber( Tm42_TestContext * ctx ) {
    TM42_BEGIN_TEST( "Lex numbers" );

    { // Positive integer.
        auto lexer = Lexer( "123" );
        const auto token = lexer.eatNumber();
        TM42_TEST_ASSERT( ctx, token.kind == TokenKind::INT32 );
        TM42_TEST_ASSERT( ctx, std::get< I32 >( token.data ) == 123 );
        TM42_TEST_ASSERT( ctx, token.loc.byteOffset == 0 );
        TM42_TEST_ASSERT( ctx, token.loc.byteLength == 3 );
    }

    { // Negative integer.
        auto lexer = Lexer( "-123" );
        const auto token = lexer.eatNumber();
        TM42_TEST_ASSERT( ctx, token.kind == TokenKind::INT32 );
        TM42_TEST_ASSERT( ctx, std::get< I32 >( token.data ) == -123 );
        TM42_TEST_ASSERT( ctx, token.loc.byteOffset == 0 );
        TM42_TEST_ASSERT( ctx, token.loc.byteLength == 4 );
    }

    { // Positive float, just decimal.
        auto lexer = Lexer( "123." );
        const auto token = lexer.eatNumber();
        TM42_TEST_ASSERT( ctx, token.kind == TokenKind::FLOAT64 );
        TM42_TEST_ASSERT(
            ctx,
            std::fabs( std::get< F64 >( token.data ) - 123.0 ) < EPSILON );
        TM42_TEST_ASSERT( ctx, token.loc.byteOffset == 0 );
        TM42_TEST_ASSERT( ctx, token.loc.byteLength == 4 );
    }

    { // Negative float, just decimal.
        auto lexer = Lexer( "-123." );
        const auto token = lexer.eatNumber();
        TM42_TEST_ASSERT( ctx, token.kind == TokenKind::FLOAT64 );
        TM42_TEST_ASSERT(
            ctx,
            std::fabs( -123.0 - std::get< F64 >( token.data ) ) < EPSILON );
        TM42_TEST_ASSERT( ctx, token.loc.byteOffset == 0 );
        TM42_TEST_ASSERT( ctx, token.loc.byteLength == 5 );
    }

    { // Positive float, with digits after decimal.
        auto lexer = Lexer( "123.56" );
        const auto token = lexer.eatNumber();
        TM42_TEST_ASSERT( ctx, token.kind == TokenKind::FLOAT64 );
        TM42_TEST_ASSERT(
            ctx,
            std::fabs( std::get< F64 >( token.data ) - 123.56 ) < EPSILON );
        TM42_TEST_ASSERT( ctx, token.loc.byteOffset == 0 );
        TM42_TEST_ASSERT( ctx, token.loc.byteLength == 6 );
    }

    { // Negative float, with digits after decimal.
        auto lexer = Lexer( "-123.56" );
        const auto token = lexer.eatNumber();
        TM42_TEST_ASSERT( ctx, token.kind == TokenKind::FLOAT64 );
        TM42_TEST_ASSERT(
            ctx,
            std::fabs( -123.56 - std::get< F64 >( token.data ) ) < EPSILON );
        TM42_TEST_ASSERT( ctx, token.loc.byteOffset == 0 );
        TM42_TEST_ASSERT( ctx, token.loc.byteLength == 7 );
    }

    TM42_END_TEST();
}
#undef EPSILON
#endif

Token
Lexer::eatToken() {
    int initialByteOffset = this->m_currentByteOffset;
    Token token;

    this->readCodepoint();
    tassert( &TC, this->m_codepointSize > 0, "" );

    if ( this->m_codepoint == '(' ) {
        token.kind = TokenKind::LPAREN;
        token.loc = SourceCodeLocation { initialByteOffset, 1 };
        this->advanceReadCodepoint();
    } else if ( this->m_codepoint == ')' ) {
        token.kind = TokenKind::RPAREN;
        token.loc = SourceCodeLocation { initialByteOffset, 1 };
        this->advanceReadCodepoint();
    } else if ( isValidIdentStart( this->m_codepoint ) ) {
        token = this->eatIdent();
    } else if ( std::isdigit( this->m_codepoint ) ||
                ( this->m_codepoint == '-' ) ) {
        token = this->eatNumber();
    } else {
        tassert( &TC, false, "" );
    }

    return token;
}

std::vector< Token >
Lexer::lex() {
    std::vector< Token > tokens;
    while ( !this->endOfInput() ) {
        this->eatWhitespace();
        if ( this->endOfInput() ) {
            break;
        }
        tokens.push_back( this->eatToken() );
    }
    return std::move( tokens );
}

#ifdef MYL_TEST
void
testLexLex( Tm42_TestContext * ctx ) {
    TM42_BEGIN_TEST( "Test overall lexer" );

    {
        auto lexer = Lexer( "(defun foo () 2)" );
        const auto tokens = lexer.lex();

        TM42_TEST_ASSERT( ctx, tokens[ 0 ].kind == TokenKind::LPAREN );
        TM42_TEST_ASSERT( ctx, tokens[ 0 ].loc.byteOffset == 0 );
        TM42_TEST_ASSERT( ctx, tokens[ 0 ].loc.byteLength == 1 );

        TM42_TEST_ASSERT( ctx, tokens[ 1 ].kind == TokenKind::IDENT );
        TM42_TEST_ASSERT( ctx, lexer.getStringView( tokens[ 1 ].loc ) == "defun" );
        TM42_TEST_ASSERT( ctx, tokens[ 1 ].loc.byteOffset == 1 );
        TM42_TEST_ASSERT( ctx, tokens[ 1 ].loc.byteLength == 5 );

        TM42_TEST_ASSERT( ctx, tokens[ 2 ].kind == TokenKind::IDENT );
        TM42_TEST_ASSERT( ctx, lexer.getStringView( tokens[ 2 ].loc ) == "foo" );
        TM42_TEST_ASSERT( ctx, tokens[ 2 ].loc.byteOffset == 7 );
        TM42_TEST_ASSERT( ctx, tokens[ 2 ].loc.byteLength == 3 );

        TM42_TEST_ASSERT( ctx, tokens[ 3 ].kind == TokenKind::LPAREN );
        TM42_TEST_ASSERT( ctx, tokens[ 3 ].loc.byteOffset == 11 );
        TM42_TEST_ASSERT( ctx, tokens[ 3 ].loc.byteLength == 1 );

        TM42_TEST_ASSERT( ctx, tokens[ 4 ].kind == TokenKind::RPAREN );
        TM42_TEST_ASSERT( ctx, tokens[ 4 ].loc.byteOffset == 12 );
        TM42_TEST_ASSERT( ctx, tokens[ 4 ].loc.byteLength == 1 );

        TM42_TEST_ASSERT( ctx, tokens[ 5 ].kind == TokenKind::INT32 );
        TM42_TEST_ASSERT( ctx, std::get< I32 >( tokens[ 5 ].data ) == 2 );
        TM42_TEST_ASSERT( ctx, tokens[ 5 ].loc.byteOffset == 14 );
        TM42_TEST_ASSERT( ctx, tokens[ 5 ].loc.byteLength == 1 );

        TM42_TEST_ASSERT( ctx, tokens[ 6 ].kind == TokenKind::RPAREN );
        TM42_TEST_ASSERT( ctx, tokens[ 6 ].loc.byteOffset == 15 );
        TM42_TEST_ASSERT( ctx, tokens[ 6 ].loc.byteLength == 1 );
    }

    TM42_END_TEST();
}
#endif // MYL_TEST
