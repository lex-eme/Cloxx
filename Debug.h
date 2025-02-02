#pragma once

#include "Chunk.h"

int disassembleInstruction(const Chunk& chunk, int offset);
void disassembleChunk(const Chunk& chunk, const std::string& name);
