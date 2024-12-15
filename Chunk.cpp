#include "Chunk.h"
#include "Value.h"

#include <cstdint>

void Chunk::writeChunk(const std::uint8_t byte, const int line) {
    code.push_back(byte);
    lines.push_back(line);
}

int Chunk::addConstant(Value value)
{
    constants.writeValue(value);
    return constants.values.size() - 1;
}
