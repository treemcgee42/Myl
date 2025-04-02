// Copyright (C) 2025 by Varun Malladi

#include <assert.h>

#ifdef MYL_TEST
#include <Test/Test.h>
#endif // MYL_TEST
#include <Tracing/Tracing.h>

#include "Parser.h"

extern TraceContext TC;

namespace SExpr {

void
Base::print( std::ostream & os ) const {
    os << "???";
}

void
Nil::print( std::ostream & os ) const {
    os << "NIL";
}

void
Int32::print( std::ostream & os ) const {
    os << "I32<" << this->value << ">";
}

void
Float64::print( std::ostream & os ) const {
    os << "F64<" << this->value << ">";
}

void
Symbol::print( std::ostream & os ) const {
    os << "SYM<" << this->value << ">";
}

void
Label::print( std::ostream & os ) const {
    os << "LABEL<" << this->value << ">";
}

void
Cons::print( std::ostream & os ) const {
    os << "(";
    if ( this->car ) {
        os << *this->car;
    } else {
        os << "NIL";
    }
    os << " ";
    if ( this->cdr ) {
        os << *this->cdr;
    } else {
        os << "NIL";
    }
    os << ")";
}

void
Proc::print( std::ostream & os ) const {
    os << "(" << this->procSymbol;
    for ( const auto & parameter : this->parameters ) {
        os << " ";
        if ( parameter.label ) {
            os << "@" << *parameter.label << " ";
        }
        os << *parameter.value;
    }
    os << ")";
}

std::ostream &
operator<<( std::ostream & os, const Base & obj ) {
    obj.print( os );
    return os;
}

} // namespace SExpr

void
Parser::expectToken( TokenKind e ) {
    tassert( &TC, this->m_currentToken.kind == e,
             "Expected token %s, got %s",
             tokenKindStr( e ), tokenKindStr( this->m_currentToken.kind ) );
}

Token *
Parser::eatToken() {
    if ( this->m_nextTokenIdx >= this->m_tokens.size() ) {
        return nullptr;
    }
    this->m_currentToken = this->m_tokens[ this->m_nextTokenIdx ];
    this->m_nextTokenIdx += 1;
    return &this->m_currentToken;
}

SExpr::Cons
Parser::parseCons() {
    this->expectToken( TokenKind::LPAREN );
    this->eatToken();

    SExpr::Cons cons;
    if ( this->m_currentToken.kind == TokenKind::RPAREN ) {
        return cons;
    }
    auto sExpr = this->parseSExpr();
    if ( this->error ) {
        return cons;
    }
    cons.car = std::move( sExpr );
    cons.cdr = std::make_unique< SExpr::Nil >();
    if ( this->m_currentToken.kind == TokenKind::RPAREN ) {
        this->eatToken();
        return std::move( cons );
    }

    if ( this->m_currentToken.kind == TokenKind::RPAREN ) {
        return cons;
    }
    sExpr = this->parseSExpr();
    if ( this->error ) {
        return cons;
    }
    cons.cdr = std::move( sExpr );

    // I could use a reference, but the pointer makes it clearer to me since move
    // semantics are heavily in play.
    std::unique_ptr< SExpr::Base > *  workingCdr = &cons.cdr;
    while ( true ) {
        if ( this->m_currentToken.kind == TokenKind::RPAREN ) {
            this->eatToken();
            break;
        }

        sExpr = this->parseSExpr();
        if ( this->error ) {
            return SExpr::Cons();
        }

        auto newCons = std::make_unique< SExpr::Cons >(
            std::move( *workingCdr ),
            std::move( sExpr ) );

        *workingCdr = std::move( newCons );
        workingCdr = &( dynamic_cast< SExpr::Cons * >( workingCdr->get() )->cdr );
    }

    return cons;
}

#ifdef MYL_TEST
void
testParseCons( Tm42_TestContext * ctx ) {
    TM42_BEGIN_TEST( "Parse cons." );

    { // Case: empty
        const auto src = "()";
        auto lexer = Lexer( src );
        const auto lexResult = lexer.lex();
        auto parser = Parser( src, lexResult.tokens );
        parser.eatToken();
        const auto cons = parser.parseCons();
        TM42_TEST_ASSERT( ctx, cons.car == nullptr );
        TM42_TEST_ASSERT( ctx, cons.cdr == nullptr );
    }
    { // Case: single item list
        const auto src = "(1)";
        auto lexer = Lexer( src );
        const auto lexResult = lexer.lex();
        auto parser = Parser( src, lexResult.tokens );
        parser.eatToken();
        const SExpr::Cons cons = parser.parseCons();
        std::cout << cons << "\n";
        const auto carPtr = dynamic_cast< SExpr::Int32 * >( cons.car.get() );
        TM42_TEST_ASSERT( ctx, carPtr );
        TM42_TEST_ASSERT( ctx, carPtr->value == 1 );
        TM42_TEST_ASSERT( ctx, dynamic_cast< SExpr::Nil * >( cons.cdr.get() ) );
    }
    { // Case: two item list
        const auto src = "(1 2.0)";
        auto lexer = Lexer( src );
        const auto lexResult = lexer.lex();
        auto parser = Parser( src, lexResult.tokens );
        parser.eatToken();
        const auto cons = parser.parseCons();

        const auto car = dynamic_cast< SExpr::Int32 * >( cons.car.get() );
        TM42_TEST_ASSERT( ctx, car );
        TM42_TEST_ASSERT( ctx, car->value == 1 );

        const auto cdr = dynamic_cast< SExpr::Float64 * >( cons.cdr.get() );
        TM42_TEST_ASSERT( ctx, cdr );
    }
    { // Case: three item list
        const auto src = "(1 2 3)";
        auto lexer = Lexer( src );
        const auto lexResult = lexer.lex();
        auto parser = Parser( src, lexResult.tokens );
        parser.eatToken();
        const auto cons = parser.parseCons();

        const auto car = dynamic_cast< SExpr::Int32 * >( cons.car.get() );
        TM42_TEST_ASSERT( ctx, car );
        TM42_TEST_ASSERT( ctx, car->value == 1 );

        const auto cdr = dynamic_cast< SExpr::Cons * >( cons.cdr.get() );
        TM42_TEST_ASSERT( ctx, cdr );
        const auto cdrCar = dynamic_cast< SExpr::Int32 * >( cdr->car.get() );
        TM42_TEST_ASSERT( ctx, cdrCar );
        TM42_TEST_ASSERT( ctx, cdrCar->value == 2 );

        const auto cdrCdr = dynamic_cast< SExpr::Int32 * >( cdr->cdr.get() );
        TM42_TEST_ASSERT( ctx, cdrCdr );
        TM42_TEST_ASSERT( ctx, cdrCdr->value == 3 );
    }

    TM42_END_TEST();
}
#endif // MYL_TEST

SExpr::Proc
Parser::parseProc( SExpr::Cons cons ) {
    const auto symbolPtr = dynamic_cast< SExpr::Symbol * >( cons.car.get() );
    assert( symbolPtr );

    std::vector< SExpr::Proc::Parameter > parameters;
    if ( dynamic_cast< SExpr::Nil * >( cons.cdr.get() ) ) {
        return SExpr::Proc( *symbolPtr, std::move( parameters ) );
    }
}

#ifdef MYL_TEST

struct ParseProcTestInput {
    Parser parser;
    SExpr::Cons cons;
};

ParseProcTestInput
parseProcTestHelper( const char * input ) {
    auto lexer = Lexer( input );
    const auto lexResult = lexer.lex();
    auto parser = Parser( input, lexResult.tokens );
    parser.eatToken();
    auto cons = parser.parseCons();
    return { std::move( parser ), std::move( cons ) };
}

void
testParseProc( Tm42_TestContext * ctx ) {
    TM42_BEGIN_TEST( "Parse procedures" );

    {
        auto input = parseProcTestHelper( "(foo)" );
        const auto proc = input.parser.parseProc( std::move( input.cons ) );
        std::cout << proc << "\n";
    }

    TM42_END_TEST();
}

#endif // MYL_TEST

std::unique_ptr< SExpr::Base >
Parser::parseSExpr() {
    switch ( this->m_currentToken.kind ) {
    case TokenKind::LPAREN:
        std::cout << "parsing cons" << "\n";
        return std::make_unique< SExpr::Cons >( this->parseCons() );
    case TokenKind::INT32: {
        std::cout << "parsing int32" << "\n";
        const auto data = std::get< I32 >( this->m_currentToken.data );
        this->eatToken();
        return std::make_unique< SExpr::Int32 >( data );
    }
    case TokenKind::FLOAT64: {
        std::cout << "parsing float64" << "\n";
        const auto data = std::get< F64 >( this->m_currentToken.data );
        this->eatToken();
        return std::make_unique< SExpr::Float64 >( data );
    }
    case TokenKind::IDENT: {
        std::cout << "parsing ident" << "\n";
        const auto data = std::get< InternedSymbol >( this->m_currentToken.data );
        this->eatToken();
        return std::make_unique< SExpr::Symbol >( data );
    }
    case TokenKind::LABEL: {
        const auto data = std::get< InternedSymbol >( this->m_currentToken.data );
        this->eatToken();
        return std::make_unique< SExpr::Label >( data );
    }
    default: {
        std::cout << "parsing error" << "\n";
        emitSourceError( this->source, this->m_currentToken.loc,
                         "Could not parse SExpr starting here." );
        this->error = true;
        return std::make_unique< SExpr::Base >();
    }
    };
}

Parser::Result
Parser::parse() {
    std::vector< std::unique_ptr< SExpr::Base > > toReturn;
    while ( this->eatToken() ) {
        toReturn.push_back( this->parseSExpr() );
        if ( this->error ) {
            break;
        }
    }
    return { std::move( toReturn ), this->error };
}
