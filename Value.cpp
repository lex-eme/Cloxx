#include "Value.h"

#include <cstdio>

void printValue(Value value)
{
    printf("%g", value);
}

void ValueArray::writeValue(Value value)
{
    values.push_back(value);
}
