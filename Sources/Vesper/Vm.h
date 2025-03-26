/* Copyright (C) 2025 by Varun Malladi */

#pragma once

#include <cstdint>
#include <unordered_map>
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

class CallStack {
public:
    struct Frame {
        size_t ip;
        size_t sbp;
        size_t sp;
    };

    void push( Frame frame );
    Frame pop();

    std::stack< Frame > frames;
};

class DataStack {
public:
    DataStack();
    Register get( size_t offsetFromBase );
    void set( size_t offsetFromBase, Register reg );
    void push( Register value );
    // Reserve at least `amount` more slots on the stack.
    void reserve( size_t amount );
    // Increment the top of the stack by `amount`, allocating memory as necessary.
    void expand( size_t amount );

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
    void executeInstructions( const std::vector< Bytecode > & instructions );
    void executeNextInstruction();

    void printNextInstruction( std::ostream & os = std::cout ) const;
    void printRegisters( std::ostream & os = std::cout ) const;
    void printFunctionTable( std::ostream & os = std::cout ) const;
    void printCurrentState( std::ostream & os = std::cout ) const;

    void pushInstruction( Bytecode instruction );
    void pushCallInstruction( const std::string & label );

    // --- begin labels -------------------------------------------------------------
    // Labels don't actually exist in the bytecode. They are just a convenience
    // for writing bytecode.
    // - Creating a label means creating an entry in the function table, where
    //   entries are addressed by an index, not a name.
    // - Jumping to a label means looking up the corresponding index in the
    //   function table and emitting a corresponding CALL instruction.

    // Define a labeled piece of code. You don't need to worry about where to
    // place the code-- all those details will be handled. Just do your code
    // building calls inside here. Don't do anything silly like define nested
    // labels. I mean, it might work?

    // `frameSize` does not include the space taken by the return value or
    // incoming arguments.
    // Returns the index into the function table at which the label is created.
    size_t beginLabel( const std::string & label, size_t frameSize );
    void endLabel();

    // --- end labels ---------------------------------------------------------------

// private:
    DataStack m_stack;

    std::vector< Bytecode > m_code;
    size_t m_nextInstructionIdx;

    std::vector< size_t > functionTable;
    std::vector< size_t > functionFrameSizeTable;
    std::unordered_map< std::string, size_t > labels;
    CallStack callStack;

    Register m_accumulator;
};
