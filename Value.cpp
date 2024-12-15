#include "Value.h"

#include <cstdio>

void printValue(const Value value)
{
    printf("%g", value);
}

void ValueArray::writeValue(const Value value)
{
    values.push_back(value);
}
