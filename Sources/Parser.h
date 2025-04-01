/* Copyright (C) 2025 by Varun Malladi */

#pragma once

#include <cstdint>
#include <iostream>
#include <variant>
#include <vector>

#include "Lexer.h"

using I32 = std::int32_t;
using F64 = double;

enum class SExprKind {
    NIL,
    INT32,
    FLOAT64,
    CONS,
    SYMBOL,
    // TODO: symbols
};

struct SExpr;

struct ConsNode {
    std::unique_ptr< SExpr > car;
    std::unique_ptr< SExpr > cdr;
};

typedef std::variant< ConsNode, I32, F64, InternedSymbol > SExprData;

struct SExpr {
    SExprKind kind;
    SExprData data;

    SExpr() : kind( SExprKind::NIL ), data( 0 ) {}
    SExpr( I32 i ) : kind( SExprKind::INT32 ), data( i ) {}
    SExpr( F64 f ) : kind( SExprKind::FLOAT64 ), data( f ) {}
    SExpr( ConsNode consNode )
        : kind( SExprKind::CONS ), data( std::move( consNode ) ) {}
    SExpr( InternedSymbol internedSymbol )
        : kind( SExprKind::SYMBOL ), data( internedSymbol ) {}

    friend std::ostream & operator<<( std::ostream & os, const SExpr & obj );
};

class Parser {
public:
    struct Result {
        std::vector< SExpr > sexprs;
        bool error;
    };

    Parser( const std::string & source, const std::vector< Token > & tokens )
        : source( source ), m_tokens{ tokens } {}
    Result parse();

    // --- begin parse functions ----------------------------------------------------
    // These functions generally assume that the first token of the thing they are
    // parsing is stored in `m_currentToken`. The caller may assume that, right after
    // returning from a parse function, `eatToken` will yield the token after the
    // thing we just parsed, if any.

    // cons := '(' SExpr SExpr? ')'
    SExpr parseCons();
    SExpr parseSExpr();

    // --- end parse functions ------------------------------------------------------

    // Returns a pointer to the token just eaten if it just ate one, null otherwise.
    Token * eatToken();

private:
    void expectToken( TokenKind e );

    const std::string & source;
    const std::vector< Token > & m_tokens;
    int m_nextTokenIdx = 0;
    Token m_currentToken;
    bool error = false;
};
