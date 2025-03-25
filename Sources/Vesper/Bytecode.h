/* Copyright (C) 2025 by Varun Malladi */

#pragma once

#include <cstdint>
#include <iostream>
#include <vector>

using U8 = std::uint8_t;

enum class Opcode : U8 {
    ADD,
    LOAD,
};

std::ostream & operator<<( std::ostream & os, Opcode op );

struct Bytecode {
    Opcode op;
    U8 arg;
};

std::ostream & operator<<( std::ostream & os, Bytecode bytecode );
