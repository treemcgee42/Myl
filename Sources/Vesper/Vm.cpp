// Copyright (C) 2025 by Varun Malladi

#include <assert.h>
#include <iomanip>

#include "Ui.h"
#include "Vm.h"

std::ostream & operator<<( std::ostream & os, Register reg ) {
    return os << "<Register i32(" << reg.i32 << ")>";
}

DataStack::DataStack(): m_stack(), m_baseIdx( 0 ), m_topIdx( 0 ) {}

Register
DataStack::get( size_t offsetFromBase ) {
    return this->m_stack[ this->m_baseIdx + offsetFromBase ];
}

void
DataStack::push( Register value ) {
    this->m_stack.push_back( value );
    this->m_topIdx += 1;
}

Vm::Vm()
    : m_stack(),
      m_code(),
      m_nextInstructionIdx( 0 ),
      m_accumulator( 0 ) {}

Vm::Vm( std::vector< Bytecode > && code )
    : m_stack(),
      m_code( std::move( code ) ),
      m_nextInstructionIdx( 0 ),
      m_accumulator( 0 ) {}

Register
Vm::accumulatorValue() {
    return this->m_accumulator;
}

void
Vm::pushDataOntoStack( Register value ) {
    this->m_stack.push( value );
}

void
Vm::setAccumulator( Register value ) {
    this->m_accumulator = value;
}

void
Vm::executeInstruction( Bytecode instruction ) {
    switch ( instruction.op ) {
    case Opcode::ADD:
        this->m_accumulator.i32 = ( this->m_stack.get( instruction.arg ).i32 +
                                    this->m_accumulator.i32 );
        break;
    default:
        assert( false );
    };
}

void
Vm::executeNextInstruction() {
    this->executeInstruction( this->m_code[ m_nextInstructionIdx ] );
    this->m_nextInstructionIdx += 1;
}

void
Vm::printNextInstruction( std::ostream & os ) const {
    os << "  " << this->m_nextInstructionIdx << " | "
       << this->m_code[ this->m_nextInstructionIdx ] << "\n";
}

static void
printRegister( Register reg ) {
    printDataAsHexSpaced( &reg, sizeof( reg ) );
    std::cout << "    " << reg << "\n";
}

void
Vm::printRegisters( std::ostream & os ) const {
    os << "--- ACC ---\n";
    printRegister( this->m_accumulator );
    os << "--- REGISTERS ---\n";
    const auto sbp = this->m_stack.m_baseIdx;
    const auto stp = this->m_stack.m_topIdx;
    os << "sbp: " << sbp << " stp: " << stp << "\n";
    for ( size_t i = sbp; i < stp; ++i ) {
        std::cout << std::setw( 2 ) << std::setfill( ' ' ) << i << " | ";
        printRegister( this->m_stack.m_stack[ i ] );
    }
}
