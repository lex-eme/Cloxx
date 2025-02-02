#pragma once

#include <cstdint>
#include <array>

#include "Chunk.h"
#include "Value.h"
#include "Source.h"

constexpr int STACK_MAX = 256;

enum InterpretResult: std::uint8_t
{
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR,
};

struct VM
{
public:
    VM();

    InterpretResult interpret(const Source& source);

private:
    Chunk* chunk = nullptr;
    uint8_t* ip = nullptr;
    std::array<Value, STACK_MAX> stack;
    Value* stackTop;


    InterpretResult run();
    inline uint8_t readByte();
    inline Value readConstant();
    inline Value peek(int distance) const;
    static inline bool isFalsey(const Value& value);
    static bool valuesEqual(const Value& a, const Value& b);
    void resetStack();
    void push(Value value);
    Value pop();
    void runtimeError(const char* format, ...);
};
