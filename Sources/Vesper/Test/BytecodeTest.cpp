// Copyright (C) 2025 by Varun Malladi

#include "Vesper/Bytecode.h"
#include "Vesper/Vm.h"
#include "BytecodeTest.h"

void
testBytecodeAdd( Tm42_TestContext * ctx ) {
    TM42_BEGIN_TEST( "ADD opcode" );

    Vm vm( { { Opcode::ADD, 0 },
             { Opcode::ADD, 1 } } );
    vm.pushDataOntoStack( Register( 123 ) );
    vm.pushDataOntoStack( Register( 9 ) );
    vm.setAccumulator( Register( 456 ) );
    vm.executeNextInstruction();
    TM42_TEST_ASSERT( ctx, vm.accumulatorValue().i32 == 579 );
    vm.executeNextInstruction();
    TM42_TEST_ASSERT( ctx, vm.accumulatorValue().i32 == 588 );

    TM42_END_TEST();
}

void
testBytecodeLoad( Tm42_TestContext * ctx ) {
    TM42_BEGIN_TEST( "LOAD opcode" );

    Vm vm( { { Opcode::LOAD, 0 } } );
    vm.pushDataOntoStack( Register( 123 ) );
    vm.executeNextInstruction();
    TM42_TEST_ASSERT( ctx, vm.accumulatorValue().i32 == 123 );

    TM42_END_TEST();
}

void
testBytecodeStore( Tm42_TestContext * ctx ) {
    TM42_BEGIN_TEST( "STORE opcode" );

    Vm vm( { { Opcode::STORE, 0 } } );
    vm.m_stack.expand( 1 );
    vm.setAccumulator( Register( 123 ) );
    vm.executeNextInstruction();
    TM42_TEST_ASSERT( ctx, vm.m_stack.get( 0 ).i32 == 123 );

    TM42_END_TEST();
}

void
testBytecodeZeroAcc( Tm42_TestContext * ctx ) {
    TM42_BEGIN_TEST( "ZERO_ACC opcode" );

    Vm vm( { { Opcode::ZERO_ACC, 0 } } );
    vm.setAccumulator( Register( 123 ) );
    TM42_TEST_ASSERT( ctx, vm.accumulatorValue().i32 == 123 );
    vm.executeNextInstruction();
    TM42_TEST_ASSERT( ctx, vm.accumulatorValue().i32 == 0 );

    TM42_END_TEST();
}

void
testBytecodeCall( Tm42_TestContext * ctx ) {
    TM42_BEGIN_TEST( "CALL opcode" );

    Vm vm;
    vm.pushInstruction( { Opcode::ZERO_ACC, 0 } );
    vm.pushCallInstruction( "foo" );
    vm.beginLabel( "foo" );
    vm.pushInstruction( { Opcode::ADD_IMM, 7 } );
    vm.endLabel();

    vm.executeNextInstruction();
    vm.executeNextInstruction();
    vm.executeNextInstruction();
    TM42_TEST_ASSERT( ctx, vm.accumulatorValue().i32 == 7 );

    TM42_END_TEST();
}

void
testBytecodeArg( Tm42_TestContext * ctx ) {
    TM42_BEGIN_TEST( "ARG opcode" );

    Vm vm;
    vm.pushDataOntoStack( Register( 1 ) );
    vm.pushDataOntoStack( Register( 2 ) );

    vm.pushInstruction( { Opcode::ZERO_ACC, 0 } );
    vm.pushInstruction( { Opcode::ARG, 0 } );
    vm.pushInstruction( { Opcode::ARG, 1 } );
    vm.pushCallInstruction( "foo" );

    vm.beginLabel( "foo" );
    vm.pushInstruction( { Opcode::LOAD, 0 } );
    vm.pushInstruction( { Opcode::ADD, 1 } );
    vm.endLabel();

    for ( int i = 0; i < 6; ++i ) {
        vm.executeNextInstruction();
    }
    TM42_TEST_ASSERT( ctx, vm.accumulatorValue().i32 == 3 );

    TM42_END_TEST();
}
