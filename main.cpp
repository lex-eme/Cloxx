#include <iostream>

#include "Chunk.h"
#include "Debug.h"
#include "VM.h"

int main(int argc, char* argv[])
{
    VM vm;
    Chunk chunk;

    int constant = chunk.addConstant(1.2);
    chunk.writeChunk(OpCode::OP_CONSTANT, 1);
    chunk.writeChunk(constant, 1);

    constant = chunk.addConstant(3.4);
    chunk.writeChunk(OpCode::OP_CONSTANT, 2);
    chunk.writeChunk(constant, 2);

    chunk.writeChunk(OpCode::OP_ADD, 3);

    constant = chunk.addConstant(5.6);
    chunk.writeChunk(OpCode::OP_CONSTANT, 4);
    chunk.writeChunk(constant, 4);

    chunk.writeChunk(OpCode::OP_DIVIDE, 5);
    chunk.writeChunk(OpCode::OP_NEGATE, 6);
    chunk.writeChunk(OpCode::OP_RETURN, 7);

    //disassembleChunk(chunk, "test chunk");
    vm.interpret(&chunk);

    return 0;
}
