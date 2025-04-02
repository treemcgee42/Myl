/* Copyright (C) 2025 by Varun Malladi */

#pragma once

#include <string>

struct SourceCodeLocation {
    // Region is str[ byteOffset:byteOffset+byteLength ].
    int byteOffset;
    int byteLength;

    // Check if this references a valid location in the provided string.
    bool isValid( const std::string & src );
};

std::ostream & operator<<( std::ostream & os, const SourceCodeLocation & loc );

void emitSourceError( const std::string & src, SourceCodeLocation loc,
                      const std::string & msg );
