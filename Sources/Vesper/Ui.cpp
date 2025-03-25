// Copyright (C) 2025 by Varun Malladi

#include <iomanip>

#include "Ui.h"

void
printDataAsHexSpaced( const void * data, size_t size, std::ostream & os ) {
    auto * bytes = reinterpret_cast< const unsigned char * >( data );
    for ( size_t i = 0; i < size; ++i ) {
        os << std::setw( 2 ) << std::setfill( '0' ) << std::hex
           << static_cast<int>( bytes[ i ] ) << " ";
    }
    os << std::dec;
}

void
hexdump( const void* data, size_t size, size_t bytesPerRow, std::ostream & os ) {
    auto * bytes = reinterpret_cast< const unsigned char * >( data );

    for ( size_t i = 0; i < size; i += bytesPerRow ) {
        // Indicate which byte offset the current row starts from.
        os << std::setw( 4 ) << std::setfill( '0' ) << std::hex << i << ": ";

        // Hex version!
        for ( size_t j = 0; j < bytesPerRow; ++j ) {
            if ( i + j < size ) {
                os << std::setw( 2 ) << static_cast<int>( bytes[ i + j ] ) << " ";
            } else {
                os << "   ";
            }
        }

        os << " ";

        // ASCII version!
        for ( size_t j = 0; j < bytesPerRow; ++j ) {
            if ( i + j < size ) {
                unsigned char c = bytes[ i + j ];
                os << ( std::isprint( c ) ? static_cast< char >( c ) : '.' );
            }
        }

        os << "\n";
    }

    // Don't forget to reset formatting.
    os << std::dec;
}
