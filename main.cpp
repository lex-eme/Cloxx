#include <iostream>

#include "Chunk.h"
#include "Debug.h"

int main(int argc, char* argv[])
{
    Chunk chunk;

    int constant = chunk.addConstant(1.2);
    std::cout << constant << std::endl;
    chunk.writeChunk(OP_CONSTANT, 123);
    chunk.writeChunk(constant, 123);

    chunk.writeChunk(OpCode::OP_RETURN, 123);

    disassembleChunk(chunk, "test chunk");

    return 0;
}
