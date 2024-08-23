#pragma once

#include <string>
#include <cstdio>

#include "Chunk.h"

#define DEBUG_TRACE_EXECUTION

int disassembleInstruction(Chunk& chunk, int offset);
void disassembleChunk(Chunk& chunk, const std::string& name);
