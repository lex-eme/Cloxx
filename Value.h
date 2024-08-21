#pragma once

#include <vector>

typedef double Value;

void printValue(Value value);

class ValueArray
{
private:

public:
    std::vector<Value> values;

    ValueArray();

    void writeValue(Value value);
};
