#pragma once

#include <vector>

using Value = double;

void printValue(Value value);

class ValueArray
{
public:
    std::vector<Value> values;

    ValueArray() = default;

    void writeValue(Value value);
};
