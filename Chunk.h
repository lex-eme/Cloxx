#pragma once

#include <vector>
#include <cstdint>

#include "Value.h"

enum OpCode
{
    OP_CONSTANT,
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_NEGATE,
    OP_RETURN,
};

class Chunk
{
private:


public:
    std::vector<std::uint8_t> code;
    std::vector<int> lines;
    ValueArray constants;

    Chunk() = default;

    void writeChunk(std::uint8_t byte, int line);
    int addConstant(Value value);
};
