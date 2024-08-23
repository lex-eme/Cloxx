#pragma once

#include "Source.h"

class Compiler
{
public:
    Compiler() = default;

    void compile(const Source& source);    
};
