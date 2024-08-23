#include <cstdio>
#include <cstdlib>

#include "VM.h"
#include "Source.h"
#include "Chunk.h"

void runFile(const char* path)
{
    VM vm;
    const Source source(path);
    const InterpretResult result = vm.interpret(source);

    if (result == InterpretResult::INTERPRET_COMPILE_ERROR)
    {
        std::exit(65);
    }
    if (result == InterpretResult::INTERPRET_RUNTIME_ERROR)
    {
        std::exit(70);
    }
}

int main(int argc, char* argv[])
{
    if (argc == 2)
    {
        runFile(argv[1]);
    }
    else
    {
        fprintf(stderr, "Usage: clox [path]\n");
        exit(64);
    }

    return 0;
}
