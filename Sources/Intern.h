/* Copyright (C) 2025 by Varun Malladi */

#pragma once

#include <string>
#include <unordered_map>
#include <vector>

using InternedSymbol = uint32_t;

class SymbolInterner {
public:
    InternedSymbol intern( const std::string & str );

private:
    std::unordered_map< std::string, InternedSymbol > stringToId;
    std::vector<std::string> idToString;
};
