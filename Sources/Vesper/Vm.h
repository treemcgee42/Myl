/* Copyright (C) 2025 by Varun Malladi */

#pragma once

#include <cstdint>
#include <vector>

#include "Bytecode.h"

using I32 = std::int32_t;
using I64 = std::int64_t;
using U64 = std::uint64_t;
using F64 = double;

union Register {
    I32 i32;

    Register( I32 i32 ): i32( i32 ) {}
};

std::ostream & operator<<( std::ostream & os, Register reg );

class DataStack {
public:
    DataStack();
    Register get( size_t offsetFromBase );
    void push( Register value );

    std::vector< Register > m_stack;
    // Points to the base item in the current stack frame.
    size_t m_baseIdx;
    // Points to the next available slot in the current stack frame, i.e. the one right
    // after the top populated slot.
    size_t m_topIdx;
};

class Vm {
public:
    Vm();
    Vm( std::vector< Bytecode > && code );

    Register accumulatorValue();

    void pushDataOntoStack( Register value );
    void setAccumulator( Register value );

    void executeInstruction( Bytecode instruction );
    void executeNextInstruction();

    void printNextInstruction( std::ostream & os = std::cout ) const;
    void printRegisters( std::ostream & os = std::cout ) const;

private:
    DataStack m_stack;

    std::vector< Bytecode > m_code;
    size_t m_nextInstructionIdx;

    Register m_accumulator;
};
