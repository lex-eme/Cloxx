#pragma once

#include <vector>

#include "Value.h"

enum OpCode
{
    OP_CONSTANT,
    OP_RETURN,
};

class Chunk
{
private:


public:
    std::vector<unsigned __int8> code;
    std::vector<int> lines;
    ValueArray constants;

    Chunk();

    void writeChunk(unsigned __int8 byte, int line);
    int addConstant(Value value);
};
