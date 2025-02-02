#pragma once

#include <vector>
#include <cstdint>

#include "Value.h"

enum OpCode: std::uint8_t
{
    OP_CONSTANT,
    OP_NIL,
    OP_TRUE,
    OP_FALSE,
    OP_EQUAL,
    OP_GREATER,
    OP_LESS,
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_NOT,
    OP_NEGATE,
    OP_RETURN,
};

class Chunk
{
public:
    std::vector<std::uint8_t> code;
    std::vector<int> lines;
    ValueArray constants;

    Chunk() = default;

    void writeChunk(std::uint8_t byte, int line);
    int addConstant(Value value);
};
