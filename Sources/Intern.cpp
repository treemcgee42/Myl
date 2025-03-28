// Copyright (C) 2025 by Varun Malladi

#include "Intern.h"

#ifdef MYL_TEST
#include <Test/Test.h>
#endif

InternedSymbol
SymbolInterner::intern( const std::string & str ) {
    auto it = stringToId.find( str );
    if ( it != stringToId.end() ) {
        return it->second;
    }

    idToString.push_back(str);
    const InternedSymbol id = static_cast<InternedSymbol>( idToString.size() );
    stringToId[ str ] = id;
    return id;
}

#ifdef MYL_TEST
void
testSymbolInterner( Tm42_TestContext * ctx ) {
    TM42_BEGIN_TEST( "Lex identifiers" );

    {
        auto interner = SymbolInterner();
        TM42_TEST_ASSERT(
            ctx,
            interner.intern( "foo" ) == interner.intern( "foo" ) );
        TM42_TEST_ASSERT(
            ctx,
            interner.intern( "foo" ) != interner.intern( "bar" ) );
    }

    TM42_END_TEST();
}
#endif // MYL_TEST
