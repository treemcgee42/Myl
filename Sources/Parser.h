/* Copyright (C) 2025 by Varun Malladi */

#pragma once

#include <cstdint>
#include <iostream>
#include <variant>
#include <vector>

#include "Lexer.h"

using I32 = std::int32_t;
using F64 = double;

namespace SExpr {

class Base {
public:
    virtual ~Base() {}  // Enables RTTI and ensures proper cleanup.
    virtual void print( std::ostream & os ) const;
};

class Nil: public Base {
public:
    Nil() = default;
    virtual void print( std::ostream & os ) const override;
};

class Int32: public Base {
public:
    I32 value;
    Int32( I32 value ): value( value ) {}

    virtual void print( std::ostream & os ) const override;
};

class Float64: public Base {
public:
    F64 value;
    Float64( F64 value ): value( value ) {}

    virtual void print( std::ostream & os ) const override;
};

class Symbol: public Base {
public:
    InternedSymbol value;
    Symbol( InternedSymbol value ): value( value ) {}

    virtual void print( std::ostream & os ) const override;
};

class Label: public Base {
public:
    InternedSymbol value;
    Label( InternedSymbol value ): value( value ) {}

    virtual void print( std::ostream & os ) const override;
};

class Cons: public Base {
public:
    std::unique_ptr< Base > car;
    std::unique_ptr< Base > cdr;

    Cons() = default;
    Cons( std::unique_ptr< Base > car ): car( std::move( car ) ) {}
    Cons( std::unique_ptr< Base > car, std::unique_ptr< Base > cdr )
        : car( std::move( car ) ), cdr( std::move( cdr ) ) {}

    virtual void print( std::ostream & os ) const override;
};

std::ostream & operator<<( std::ostream & os, const Base & obj );

class Proc: public Base {
public:
    struct Parameter {
        std::optional< InternedSymbol > label;
        std::unique_ptr< Base > value;
    };

    Symbol procSymbol;
    std::vector< Parameter > parameters;

    Proc( Symbol procSymbol, std::vector< Parameter > parameters )
        : procSymbol( procSymbol ), parameters( std::move( parameters ) ) {}

    virtual void print( std::ostream & os ) const override;
};

} // namespace SExpr

class Parser {
public:
    struct Result {
        std::vector< std::unique_ptr< SExpr::Base > > sexprs;
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
    SExpr::Cons parseCons();
    // Proc := '(' Symbol Parameters ')'
    // Parameters := Cons< Parameter, Parameters > | Nil
    // Parameter := Label SExpr
    // Label := '@' Symbol | Nil
    SExpr::Proc parseProc( SExpr::Cons cons );
    // We have to return a pointer or else we'll lose RTTI... ):
    std::unique_ptr< SExpr::Base > parseSExpr();

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
