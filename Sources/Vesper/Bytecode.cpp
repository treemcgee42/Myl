// Copyright (C) 2025 by Varun Malladi

#include "Bytecode.h"

std::ostream &
operator<<( std::ostream & os, Opcode op ) {
    switch ( op ) {
    case Opcode::ADD:
        return os << "ADD";
    default:
        return os << "???";
    };
}

std::ostream &
operator<<( std::ostream & os, Bytecode bytecode ) {
    return os << bytecode.op << " " << int( bytecode.arg );
}
