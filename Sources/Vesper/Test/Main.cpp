
#include <Test/Test.h>

#include "BytecodeTest.h"

int
main() {
    Tm42_TestContext ctx;

    testBytecodeAdd( &ctx );
    testBytecodeLoad( &ctx );
    testBytecodeStore( &ctx );
    testBytecodeZeroAcc( &ctx );
    testBytecodeCall( &ctx );
    testBytecodeArg( &ctx );
    testBytecodeRet( &ctx );

    return 0;
}
