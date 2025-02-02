#include "Debug.h"

#include <cstdio>
#include <cstdint>

#include "Chunk.h"
#include "Value.h"

int simpleInstruction(const std::string& name, const int offset)
{
    printf("%s\n", name.c_str());
    return offset + 1;
}

int constantInstruction(const std::string& name, const Chunk& chunk, const int offset)
{
    const std::uint8_t constant = chunk.code[offset + 1];

    printf("%-16s %4d '", name.c_str(), constant);
    printValue(chunk.constants.values[constant]);
    printf("'\n");

    return offset + 2;
}

int disassembleInstruction(const Chunk& chunk, const int offset)
{
    printf("%04d ", offset);

    if (offset > 0 && chunk.lines[offset] == chunk.lines[offset - 1])
    {
        printf("   | ");
    }
    else
    {
        printf("%4d ", chunk.lines[offset]);
    }

    const std::uint8_t instruction = chunk.code[offset];
    switch (instruction) {
    case OpCode::OP_CONSTANT:
        return constantInstruction("OP_CONSTANT", chunk, offset);
    case OpCode::OP_NIL:
        return simpleInstruction("OP_NIL", offset);
    case OpCode::OP_TRUE:
        return simpleInstruction("OP_TRUE", offset);
    case OpCode::OP_FALSE:
        return simpleInstruction("OP_FALSE", offset);
    case OpCode::OP_EQUAL:
        return simpleInstruction("OP_EQUAL", offset);
    case OpCode::OP_GREATER:
        return simpleInstruction("OP_GREATER", offset);
    case OpCode::OP_LESS:
        return simpleInstruction("OP_LESS", offset);
    case OpCode::OP_ADD:
        return simpleInstruction("OP_ADD", offset);
    case OpCode::OP_SUBTRACT:
        return simpleInstruction("OP_SUBTRACT", offset);
    case OpCode::OP_MULTIPLY:
        return simpleInstruction("OP_MULTIPLY", offset);
    case OpCode::OP_DIVIDE:
        return simpleInstruction("OP_DIVIDE", offset);
    case OpCode::OP_NOT:
        return simpleInstruction("OP_NOT", offset);
    case OpCode::OP_NEGATE:
        return simpleInstruction("OP_NEGATE", offset);
    case OpCode::OP_RETURN:
        return simpleInstruction("OP_RETURN", offset);
    default:
        printf("Unknown opcode %d\n", instruction);
        return offset + 1;
    }
}

void disassembleChunk(const Chunk& chunk, const std::string& name)
{
    printf("== %s ==\n", name.c_str());

    for (int offset = 0; offset < chunk.code.size();)
    {
        offset = disassembleInstruction(chunk, offset);
    }
}
