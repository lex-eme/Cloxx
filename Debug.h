#pragma once

#include <string>
#include <cstdio>

#include "Chunk.h"
#include "Value.h"

int disassembleInstruction(Chunk& chunk, int offset);
void disassembleChunk(Chunk& chunk, const std::string& name);
