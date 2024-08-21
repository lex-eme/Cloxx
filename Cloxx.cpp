#include <cstdio>
#include <cstdlib>

#include "VM.h"

char* readFile(const char* path)
{
    FILE* file = fopen(path, "rb");

    if (file == nullptr)
    {
        fprintf(stderr, "Could not open file \"%s\".\n", path);
        std::exit(74);
    }

    fseek(file, 0L, SEEK_END);
    const size_t fileSize = ftell(file);
    rewind(file);

    char* buffer = (char*)malloc(fileSize + 1);
    if (buffer == nullptr) {
        fprintf(stderr, "Not enough memory to read \"%s\".\n", path);
        std::exit(74);
    }

    const size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);
    if (bytesRead < fileSize) {
        fprintf(stderr, "Could not read file \"%s\".\n", path);
        std::exit(74);
    }

    buffer[bytesRead] = '\0';

    fclose(file);
    return buffer;
}

void runFile(const char* path)
{
    VM vm;
    char* source = readFile(path);
    const InterpretResult result = vm.interpret(source);
    free(source);

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
