// Copyright (C) 2025 by Varun Malladi

#include <assert.h>
#include <iostream>

#include "Error.h"

bool
SourceCodeLocation::isValid( const std::string & src ) {
    return ( ( this->byteOffset > 0 ) &&
             ( this->byteOffset + this->byteLength < src.size() ) );
}

void
emitSourceError( const std::string & src, SourceCodeLocation loc,
                 const std::string & msg ) {
    assert( loc.isValid( src ) );

    // Count line number
    int lineNumber = 1;
    size_t lineStart = 0;
    for ( size_t i = 0; i < (size_t)loc.byteOffset; ++i ) {
        if ( src[ i ] == '\n' ) {
            ++lineNumber;
            lineStart = i + 1;
        }
    }

    // Find end of the line.
    size_t lineEnd = src.find( '\n', loc.byteOffset );
    if ( lineEnd == std::string::npos ) {
        lineEnd = src.size();
    }

    std::string line = src.substr( lineStart, lineEnd - lineStart );

    // Compute column (number of characters from lineStart to byteOffset)
    int column = loc.byteOffset - lineStart;

    std::cerr << "error <FILE>:" << lineNumber << ":" << column
              << ": " << msg << "\n";
    std::cerr << line << "\n";
    std::cerr << std::string( column, ' ' )
              << std::string( loc.byteLength > 0 ? loc.byteLength : 1, '^' )
              << "\n";
}
