#pragma once

#include <vector>

enum ValueType: std::uint8_t
{
    VAL_BOOL,
    VAL_NIL,
    VAL_NUMBER,
};

struct Value
{
    ValueType type;
    union {
        bool boolean;
        double number;
    } as;
};

#define IS_BOOL(value)          ((value).type == VAL_BOOL)
#define IS_NIL(value)           ((value).type == VAL_NIL)
#define IS_NUMBER(value)        ((value).type == VAL_NUMBER)

#define BOOL_VAL(value)         ((Value){VAL_BOOL, {.boolean = value}})
#define NIL_VAL                 ((Value){VAL_NIL, {.number = 0}})
#define NUMBER_VAL(value)       ((Value){VAL_NUMBER, {.number = value}})

#define AS_BOOL(value)      ((value).as.boolean)
#define AS_NUMBER(value)    ((value).as.number)

void printValue(const Value& value);

class ValueArray
{
public:
    std::vector<Value> values;

    ValueArray() = default;

    void writeValue(const Value& value);
};
