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
    os << "(PROC<" << this->procSymbol << ">";
    for ( const auto & parameter : this->parameters ) {
        os << " ";
        if ( parameter.label ) {
            os << "@SYM<" << *parameter.label << "> ";
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
        // Empty list.
        return cons;
    }

    auto sExpr = this->parseSExpr();
    if ( this->error ) {
        return cons;
    }
    cons.car = std::move( sExpr );

    if ( this->m_currentToken.kind == TokenKind::RPAREN ) {
        // Nil CDR.
        this->eatToken();
        return std::move( cons );
    }

    sExpr = this->parseSExpr();
    if ( this->error ) {
        return cons;
    }
    cons.cdr = std::make_unique< SExpr::Cons >( std::move( sExpr ) );

    auto * workingCdr = dynamic_cast< SExpr::Cons * >( cons.cdr.get() );
    while ( true ) {
        if ( this->m_currentToken.kind == TokenKind::RPAREN ) {
            this->eatToken();
            break;
        }

        sExpr = this->parseSExpr();
        if ( this->error ) {
            return cons;
        }

        auto newCons = std::make_unique< SExpr::Cons >( std::move( sExpr ) );
        workingCdr->cdr = std::move( newCons );
        workingCdr = dynamic_cast< SExpr::Cons * >( workingCdr->cdr.get() );
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
        TM42_TEST_ASSERT( ctx, cons.cdr == nullptr );
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

        const auto cdr = dynamic_cast< SExpr::Cons * >( cons.cdr.get() );
        TM42_TEST_ASSERT( ctx, cdr );
        const auto cdrCar = dynamic_cast< SExpr::Float64 * >( cdr->car.get() );
        TM42_TEST_ASSERT( ctx, cdrCar );
        TM42_TEST_ASSERT( ctx, cdr->cdr == nullptr );
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

        const auto cdrCdr = dynamic_cast< SExpr::Cons * >( cdr->cdr.get() );
        TM42_TEST_ASSERT( ctx, cdrCdr );
        const auto cdrCdrCar = dynamic_cast< SExpr::Int32 * >( cdrCdr->car.get() );
        TM42_TEST_ASSERT( ctx, cdrCdrCar );
        TM42_TEST_ASSERT( ctx, cdrCdrCar->value == 3 );
        TM42_TEST_ASSERT( ctx, cdrCdr->cdr == nullptr );
    }

    TM42_END_TEST();
}
#endif // MYL_TEST

SExpr::Proc
Parser::parseProc( SExpr::Cons cons ) {
    const auto symbolPtr = dynamic_cast< SExpr::Symbol * >( cons.car.get() );
    assert( symbolPtr );

    std::vector< SExpr::Proc::Parameter > parameters;
    // Keep the both unique_ptr and the downcast ptr so the downcast ptr doesn't
    // point to something that gets cleaned up.
    auto rest = std::move( cons.cdr );
    while ( rest ) {
        SExpr::Proc::Parameter param;
        auto restConsPtr = dynamic_cast< SExpr::Cons * >( rest.get() );
        auto current = std::move( restConsPtr->car );
        auto nextCell = std::move( restConsPtr->cdr );

        if ( auto labelPtr = dynamic_cast< SExpr::Label * >( current.get() );
             labelPtr ) {
            auto nextCellConsPtr = dynamic_cast< SExpr::Cons * >( nextCell.get() );
            assert( nextCellConsPtr );

            param.label = labelPtr->value;
            param.value = std::move( nextCellConsPtr->car );
            rest = std::move( nextCellConsPtr->cdr );
        } else {
            param.value = std::move( current );
            rest = std::move( nextCell );
        }

        parameters.push_back( std::move( param ) );
    }

    return SExpr::Proc( *symbolPtr, std::move( parameters ) );
}

#ifdef MYL_TEST

struct ParseProcTestInput {
    std::shared_ptr< SymbolInterner > symbolInterner;
    Parser parser;
    SExpr::Cons cons;

    InternedSymbol
    intern( const std::string & st ) {
        return this->symbolInterner->intern( st );
    }
};

ParseProcTestInput
parseProcTestHelper( const char * input ) {
    const auto src = std::string( input );
    auto lexer = Lexer( src );
    const auto lexResult = lexer.lex();
    assert( !lexResult.error );

    auto parser = Parser( input, lexResult.tokens );
    parser.eatToken();
    auto cons = parser.parseCons();
    return { lexer.symbolInterner, std::move( parser ), std::move( cons ) };
}

void
testParseProc( Tm42_TestContext * ctx ) {
    TM42_BEGIN_TEST( "Parse procedures" );

    { // no parameters
        auto input = parseProcTestHelper( "(foo)" );
        const auto proc = input.parser.parseProc( std::move( input.cons ) );
        TM42_TEST_ASSERT( ctx, proc.parameters.size() == 0 );
    }
    { // unlabeled parameters
        auto input = parseProcTestHelper( "(foo 1 2)" );
        const auto proc = input.parser.parseProc( std::move( input.cons ) );
        TM42_TEST_ASSERT( ctx, proc.parameters.size() == 2 );
        TM42_TEST_ASSERT( ctx, !proc.parameters[ 0 ].label );
        TM42_TEST_ASSERT( ctx, !proc.parameters[ 1 ].label );
    }
    { // labeled parameters
        auto input = parseProcTestHelper( "(foo @p1 1 @p2 2)" );
        const auto proc = input.parser.parseProc( std::move( input.cons ) );
        TM42_TEST_ASSERT( ctx, proc.parameters.size() == 2 );
        TM42_TEST_ASSERT(
            ctx, *proc.parameters[ 0 ].label == input.intern( "p1" ) );
        TM42_TEST_ASSERT(
            ctx, *proc.parameters[ 1 ].label == input.intern( "p2" ) );
    }
    { // mix labeled and unlabeled parameters
        auto input = parseProcTestHelper( "(foo 1 @p2 3 4 @p5 6 7)" );
        const auto proc = input.parser.parseProc( std::move( input.cons ) );
        TM42_TEST_ASSERT( ctx, proc.parameters.size() == 5 );
        TM42_TEST_ASSERT( ctx, !proc.parameters[ 0 ].label );
        TM42_TEST_ASSERT(
            ctx, *proc.parameters[ 1 ].label == input.intern( "p2" ) );
        TM42_TEST_ASSERT( ctx, !proc.parameters[ 2 ].label );
        TM42_TEST_ASSERT(
            ctx, *proc.parameters[ 3 ].label == input.intern( "p5" ) );
        TM42_TEST_ASSERT( ctx, !proc.parameters[ 4 ].label );
    }

    TM42_END_TEST();
}

#endif // MYL_TEST

std::unique_ptr< SExpr::Base >
Parser::parseSExpr() {
    switch ( this->m_currentToken.kind ) {
    case TokenKind::LPAREN:
        return std::make_unique< SExpr::Cons >( this->parseCons() );
    case TokenKind::INT32: {
        const auto data = std::get< I32 >( this->m_currentToken.data );
        this->eatToken();
        return std::make_unique< SExpr::Int32 >( data );
    }
    case TokenKind::FLOAT64: {
        const auto data = std::get< F64 >( this->m_currentToken.data );
        this->eatToken();
        return std::make_unique< SExpr::Float64 >( data );
    }
    case TokenKind::IDENT: {
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
