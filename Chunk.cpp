#include "Chunk.h"

Chunk::Chunk()
{
}

void Chunk::writeChunk(unsigned __int8 byte, int line)
{
    code.push_back(byte);
    lines.push_back(line);
}

int Chunk::addConstant(Value value)
{
    constants.writeValue(value);
    return constants.values.size() - 1;
}
