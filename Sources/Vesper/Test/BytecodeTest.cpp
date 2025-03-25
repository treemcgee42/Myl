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

