
#include <iostream>

#include "Repl.h"

static std::string READ_INPUT;

std::string *
getInputBasicRepl() {
    READ_INPUT.erase();
    std::cout << "MYL> ";
    auto & input = std::getline( std::cin, READ_INPUT );
    if ( input.fail() ) {
        std::cout << "\n";
        return nullptr;
    }
    return &READ_INPUT;
}
