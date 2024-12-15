#include <cstdio>
#include <iostream>

#include "VM.h"
#include "Source.h"

void runFile(const char* path)
{
    VM vm;
    const Source source(path);
    const InterpretResult result = vm.interpret(source);

    if (result == INTERPRET_COMPILE_ERROR)
    {
        std::exit(65);
    }
    if (result == INTERPRET_RUNTIME_ERROR)
    {
        std::exit(70);
    }
}

int main(const int argc, char* argv[])
{
    if (argc == 2)
    {
        runFile(argv[1]);
    }
    else
    {
        std::cerr << "Usage: clox [path]" << std::endl;
        exit(64);
    }

    return 0;
}
