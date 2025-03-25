/* Copyright (C) 2025 by Varun Malladi */

#pragma once

#include <iostream>

void printDataAsHexSpaced( const void * data, size_t size,
                           std::ostream & os = std::cout );
void hexdump( const void * data, size_t size, size_t bytesPerRow,
              std::ostream & os = std::cout );
