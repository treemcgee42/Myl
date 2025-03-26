// Copyright (C) 2025 by Varun Malladi

#include <assert.h>
#include <iomanip>

#include "Ui.h"
#include "Vm.h"

std::ostream & operator<<( std::ostream & os, Register reg ) {
    return os << "<Register i32(" << reg.i32 << ")>";
}

void
CallStack::push( CallStack::Frame frame ) {
    this->frames.push( frame );
}

CallStack::Frame
CallStack::pop() {
    const auto toReturn = this->frames.top();
    this->frames.pop();
    return toReturn;
}

DataStack::DataStack(): m_stack(), m_baseIdx( 0 ), m_topIdx( 0 ) {}

Register
DataStack::get( size_t offsetFromBase ) {
    return this->m_stack[ this->m_baseIdx + offsetFromBase ];
}

void
DataStack::set( size_t offsetFromBase, Register reg ) {
    this->m_stack[ this->m_baseIdx + offsetFromBase ] = reg;
}

void
DataStack::push( Register value ) {
    this->m_stack.push_back( value );
    this->m_topIdx += 1;
}

void
DataStack::reserve( size_t amount ) {
    this->m_stack.resize( this->m_baseIdx + amount, 0 );
}

void
DataStack::expand( size_t amount ) {
    this->m_stack.resize( this->m_baseIdx + amount, 0 );
    this->m_topIdx += amount;
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
    case Opcode::ADD_IMM:
        this->m_accumulator.i32 = instruction.arg + this->m_accumulator.i32;
        break;
    case Opcode::LOAD:
        this->m_accumulator = this->m_stack.get( instruction.arg );
        break;
    case Opcode::STORE:
        this->m_stack.set( instruction.arg, this->m_accumulator );
        break;
    case Opcode::ZERO_ACC:
        this->m_accumulator = 0;
        break;
    case Opcode::ARG:
        this->m_stack.push( this->m_stack.get( instruction.arg ) );
        this->m_accumulator.i32 += 1;
        break;
    case Opcode::ARG_IMM:
        this->m_stack.push( instruction.arg );
        this->m_accumulator.i32 += 1;
        break;
    case Opcode::CALL:
        this->callStack.push(
            { this->m_nextInstructionIdx,
              this->m_stack.m_baseIdx,
              this->m_stack.m_topIdx - this->m_accumulator.i32 } );
        this->m_stack.m_baseIdx = this->m_stack.m_topIdx - this->m_accumulator.i32;
        this->m_nextInstructionIdx = this->functionTable[ instruction.arg ];
        this->m_stack.reserve( this->functionFrameSizeTable[ instruction.arg ] );
        break;
    case Opcode::RET: {
        const auto frame = this->callStack.pop();
        this->m_nextInstructionIdx = frame.ip;
        this->m_stack.m_baseIdx = frame.sbp;
        this->m_stack.m_topIdx = frame.sp + instruction.arg;
        break;
    }
    default:
        assert( false );
    };
}

void
Vm::executeInstructions( const std::vector< Bytecode > & instructions ) {
    this->m_nextInstructionIdx = this->m_code.size();
    for ( const auto & instruction : instructions ) {
        this->m_code.push_back( instruction );
    }
    while ( this->m_nextInstructionIdx < this->m_code.size() ) {
        this->executeNextInstruction();
    }
}

void
Vm::executeNextInstruction() {
    // It makes sense to increment the IP before executing the instruction:
    // - For more opcodes, it doesn't matter.
    // - For CALL, if we pre-increment then the IP points to right after the
    //   CALL instruction, which is the right return address. Also, we can
    //   then set the IP to the function start, and this function won't then
    //   increment it.
    const auto instructionIdx = this->m_nextInstructionIdx;
    this->m_nextInstructionIdx += 1;
    this->executeInstruction( this->m_code[ instructionIdx ] );
}

void
Vm::printNextInstruction( std::ostream & os ) const {
    os << "--- NEXT INSTRUCTION ---\n";
    os << "  " << this->m_nextInstructionIdx << " | "
       << this->m_code[ this->m_nextInstructionIdx ] << "\n";
}

static void
printRegister( Register reg ) {
    printDataAsHexSpaced( &reg, sizeof( reg ) );
    std::cout << "    " << reg << "\n";
}

void
Vm::pushInstruction( Bytecode instruction ) {
    this->m_code.push_back( instruction );
}

void
Vm::pushCallInstruction( const std::string & label ) {
    const auto functionTableIdx = this->labels[ label ];
    assert( functionTableIdx < 256 );
    this->pushInstruction( { Opcode::CALL, U8( functionTableIdx ) } );
}

void
Vm::printRegisters( std::ostream & os ) const {
    os << "--- ACC ---\n";
    printRegister( this->m_accumulator );
    os << "--- REGISTERS ---\n";
    const auto sbp = this->m_stack.m_baseIdx;
    const auto sp = this->m_stack.m_topIdx;
    os << "sbp: " << sbp << " sp: " << sp << "\n";
    for ( size_t i = sbp; i < sp; ++i ) {
        std::cout << std::setw( 2 ) << std::setfill( ' ' ) << i - sbp << " | ";
        printRegister( this->m_stack.m_stack[ i ] );
    }
}

void
Vm::printFunctionTable( std::ostream & os ) const {
    os << "--- FUNCTION TABLE ---\n";
    for ( size_t i = 0; i < this->functionTable.size(); ++i ) {
        std::cout << std::setw( 4 ) << std::setfill( ' ' ) << i << " | "
                  << this->functionTable[ i ] << "\n";
    }
}

void
Vm::printCurrentState( std::ostream & os ) const {
    os << "\n";
    this->printNextInstruction();
    this->printRegisters();
    this->printFunctionTable();
    os << "\n";
}

size_t
Vm::beginLabel( const std::string & label, size_t frameSize ) {
    this->labels[ label ] = this->m_code.size();
    this->functionTable.push_back( this->m_code.size() );
    this->functionFrameSizeTable.push_back( frameSize );
    return this->functionTable.size() - 1;
}

void
Vm::endLabel() {}
