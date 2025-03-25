
#include <Test/Test.h>

#include "BytecodeTest.h"

int
main() {
    Tm42_TestContext ctx;

    testBytecodeAdd( &ctx );

    return 0;
}
