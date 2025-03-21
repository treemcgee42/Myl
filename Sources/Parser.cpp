// Copyright (C) 2025 by Varun Malladi

#include <assert.h>

#ifdef MYL_TEST
#include <Test/Test.h>
#endif // MYL_TEST
#include <Tracing/Tracing.h>

#include "Parser.h"

extern TraceContext TC;

std::ostream &
operator<<( std::ostream& os, const SExpr & obj ) {
    switch ( obj.kind ) {
    case SExprKind::NIL:
        return os << "NIL";
    case SExprKind::INT32:
        return os << "I32<" << std::get< I32 >( obj.data ) << ">";
    case SExprKind::FLOAT64:
        return os << "F64<" << std::get< F64 >( obj.data ) << ">";
    case SExprKind::CONS: {
        const auto & cons = std::get< ConsNode >( obj.data );
        return os << "(" << ( *cons.car ) << " " << ( *cons.cdr ) << ")";
    }
    default:
        return os << "???";
    }
}

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

SExpr
Parser::parseCons() {
    this->expectToken( TokenKind::LPAREN );
    this->eatToken();

    ConsNode cons;
    auto sExpr = this->parseSExpr();
    cons.car = std::make_unique< SExpr >( std::move( sExpr ) );
    cons.cdr = std::make_unique< SExpr >();
    if ( this->m_currentToken.kind == TokenKind::RPAREN ) {
        this->eatToken();
        return SExpr( std::move( cons ) );
    }

    sExpr = this->parseSExpr();
    cons.cdr = std::make_unique< SExpr >( std::move( sExpr ) );

    // I could use a reference, but the pointer makes it clearer to me since move
    // semantics are heavily in play.
    std::unique_ptr< SExpr > *  workingCdr = &cons.cdr;
    while ( true ) {
        if ( this->m_currentToken.kind == TokenKind::RPAREN ) {
            this->eatToken();
            break;
        }

        sExpr = this->parseSExpr();
        assert( sExpr.kind != SExprKind::NIL );

        ConsNode newCons;
        newCons.car = std::move( std::move( *workingCdr ) );
        newCons.cdr = std::make_unique< SExpr >( std::move( sExpr ) );
        auto newConsSExpr = std::make_unique< SExpr >( std::move( newCons ) );

        *workingCdr = std::move( newConsSExpr );
        workingCdr = &std::get< ConsNode >( ( *workingCdr )->data ).cdr;
    }

    return SExpr( std::move( cons ) );
}

#ifdef MYL_TEST
void
testParseCons( Tm42_TestContext * ctx ) {
    TM42_BEGIN_TEST( "Parse cons." );

    { // Case: empty
        auto lexer = Lexer( "()" );
        const auto tokens = lexer.lex();
        auto parser = Parser( tokens );
        parser.eatToken();
        const auto consSExpr = parser.parseCons();
        TM42_TEST_ASSERT( ctx, consSExpr.kind == SExprKind::CONS );
        const auto & consNode = std::get< ConsNode >( consSExpr.data );

        TM42_TEST_ASSERT( ctx, consNode.car->kind == SExprKind::NIL );

        TM42_TEST_ASSERT( ctx, consNode.cdr->kind == SExprKind::NIL );
    }
    { // Case: single item list
        auto lexer = Lexer( "(1)" );
        const auto tokens = lexer.lex();
        auto parser = Parser( tokens );
        parser.eatToken();
        const auto consSExpr = parser.parseCons();
        TM42_TEST_ASSERT( ctx, consSExpr.kind == SExprKind::CONS );
        const auto & consNode = std::get< ConsNode >( consSExpr.data );

        TM42_TEST_ASSERT( ctx, consNode.car->kind == SExprKind::INT32 );
        const auto carData = std::get< I32 >( consNode.car->data );
        TM42_TEST_ASSERT( ctx, carData == 1 );

        TM42_TEST_ASSERT( ctx, consNode.cdr->kind == SExprKind::NIL );
    }
    { // Case: two item list
        auto lexer = Lexer( "(1 2.0)" );
        const auto tokens = lexer.lex();
        auto parser = Parser( tokens );
        parser.eatToken();
        const auto consSExpr = parser.parseCons();
        TM42_TEST_ASSERT( ctx, consSExpr.kind == SExprKind::CONS );
        const auto & consNode = std::get< ConsNode >( consSExpr.data );

        TM42_TEST_ASSERT( ctx, consNode.car->kind == SExprKind::INT32 );
        const auto carData = std::get< I32 >( consNode.car->data );
        TM42_TEST_ASSERT( ctx, carData == 1 );

        TM42_TEST_ASSERT( ctx, consNode.cdr->kind == SExprKind::FLOAT64 );
    }
    { // Case: three item list
        auto lexer = Lexer( "(1 2 3)" );
        const auto tokens = lexer.lex();
        auto parser = Parser( tokens );
        parser.eatToken();
        const auto consSExpr = parser.parseCons();
        TM42_TEST_ASSERT( ctx, consSExpr.kind == SExprKind::CONS );
        const auto & consNode1 = std::get< ConsNode >( consSExpr.data );

        TM42_TEST_ASSERT( ctx, consNode1.car->kind == SExprKind::INT32 );
        auto data = std::get< I32 >( consNode1.car->data );
        TM42_TEST_ASSERT( ctx, data == 1 );

        TM42_TEST_ASSERT( ctx, consNode1.cdr->kind == SExprKind::CONS );
        const auto & consNode2 = std::get< ConsNode >( consNode1.cdr->data );
        TM42_TEST_ASSERT( ctx, consNode2.car->kind == SExprKind::INT32 );
        data = std::get< I32 >( consNode2.car->data );
        TM42_TEST_ASSERT( ctx, data == 2 );

        TM42_TEST_ASSERT( ctx, consNode2.cdr->kind == SExprKind::INT32 );
        data = std::get< I32 >( consNode2.cdr->data );
        TM42_TEST_ASSERT( ctx, data == 3 );
    }

    TM42_END_TEST();
}
#endif // MYL_TEST

SExpr
Parser::parseSExpr() {
    if ( this->m_currentToken.kind == TokenKind::LPAREN ) {
        return SExpr( this->parseCons() );
    } else if ( this->m_currentToken.kind == TokenKind::INT32 ) {
        const auto data = std::get< I32 >( this->m_currentToken.data );
        this->eatToken();
        return SExpr( data );
    } else if ( this->m_currentToken.kind == TokenKind::FLOAT64 ) {
        const auto data = std::get< F64 >( this->m_currentToken.data );
        this->eatToken();
        return SExpr( data );
    } else {
        return SExpr();
    }
}

std::vector< SExpr >
Parser::parse() {
    std::vector< SExpr > toReturn;
    while ( this->eatToken() ) {
        toReturn.push_back( this->parseSExpr() );
    }
    return std::move( toReturn );
}
