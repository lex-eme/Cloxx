#pragma once

#include "Chunk.h"
#include "Value.h"

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

    InterpretResult interpret(Chunk* c);

private:
    Chunk* chunk = nullptr;
    unsigned __int8* ip = nullptr;
    Value stack[STACK_MAX];
    Value* stackTop;

    InterpretResult run();
    inline unsigned __int8 readByte();
    inline Value readConstant();
    void resetStack();
    void push(Value value);
    Value pop();
};
