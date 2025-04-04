// Copyright (C) 2025 by Varun Malladi

#include <assert.h>

#include "Bytecode.h"

std::ostream &
operator<<( std::ostream & os, Opcode op ) {
    switch ( op ) {
    case Opcode::ADD:
        return os << "ADD";
    case Opcode::ADD_IMM:
        return os << "ADD_IMM";
    case Opcode::LOAD:
        return os << "LOAD";
    case Opcode::STORE:
        return os << "STORE";
    case Opcode::ZERO_ACC:
        return os << "ZERO_ACC";
    case Opcode::ARG:
        return os << "ARG";
    case Opcode::ARG_IMM:
        return os << "ARG_IMM";
    case Opcode::CALL:
        return os << "CALL";
    case Opcode::RET:
        return os << "RET";
    default:
        assert( false );
    };
}

std::ostream &
operator<<( std::ostream & os, Bytecode bytecode ) {
    return os << bytecode.op << " " << int( bytecode.arg );
}
