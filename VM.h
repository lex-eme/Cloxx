#pragma once

#include "Chunk.h"
#include "Value.h"

#include <cstdint>

constexpr int STACK_MAX = 256;

enum InterpretResult
{
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR,
};

struct VM
{
public:
    VM();

    InterpretResult interpret(const char* source);

private:
    Chunk* chunk = nullptr;
    uint8_t* ip = nullptr;
    Value stack[STACK_MAX];
    Value* stackTop;

    InterpretResult run();
    inline uint8_t readByte();
    inline Value readConstant();
    void resetStack();
    void push(Value value);
    Value pop();
};
