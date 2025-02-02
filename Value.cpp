#include "Value.h"

#include <cstdio>

void printValue(const Value value)
{
    switch (value.type)
    {
        case VAL_BOOL: printf(AS_BOOL(value) ? "true" : "false"); break;
        case VAL_NIL: printf("nil"); break;
        case VAL_NUMBER: printf("%g", AS_NUMBER(value)); break;
    }
}

void ValueArray::writeValue(const Value value)
{
    values.push_back(value);
}
