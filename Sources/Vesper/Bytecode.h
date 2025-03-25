/* Copyright (C) 2025 by Varun Malladi */

#pragma once

#include <cstdint>
#include <iostream>
#include <vector>

using U8 = std::uint8_t;

enum class Opcode : U8 {
    ADD,
    ADD_IMM,
    LOAD,
    STORE,
    ZERO_ACC,
    // --- begin control flow -------------------------------------------------------
    CALL
    // --- end control flow ---------------------------------------------------------
};

std::ostream & operator<<( std::ostream & os, Opcode op );

struct Bytecode {
    Opcode op;
    U8 arg;
};

std::ostream & operator<<( std::ostream & os, Bytecode bytecode );
