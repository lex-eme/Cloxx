#include "VM.h"

#include <cstdio>

#include "debug.h"

VM::VM()
{
    resetStack();
}

InterpretResult VM::interpret(Chunk* c)
{
    chunk = c;
    ip = chunk->code.data();
    return run();
}

InterpretResult VM::run()
{
#define BINARY_OP(op) \
    do { \
      double b = pop(); \
      double a = pop(); \
      push(a op b); \
    } while (false)

    for (;;)
    {
    #ifdef DEBUG_TRACE_EXECUTION
        printf("          ");
        for (Value* slot = stack; slot < stackTop; slot++)
        {
            printf("[ ");
            printValue(*slot);
            printf(" ]");
        }
        printf("\n");
        disassembleInstruction(*chunk, (int)(ip - chunk->code.data()));
    #endif

        unsigned __int8 instruction;
        switch (instruction = readByte())
        {
        case OpCode::OP_CONSTANT:
        {
            Value constant = readConstant();
            push(constant);
            break;
        }
        case OpCode::OP_ADD:      BINARY_OP(+); break;
        case OpCode::OP_SUBTRACT: BINARY_OP(-); break;
        case OpCode::OP_MULTIPLY: BINARY_OP(*); break;
        case OpCode::OP_DIVIDE:   BINARY_OP(/); break;
        case OpCode::OP_NEGATE:   push(-pop()); break;
        case OpCode::OP_RETURN:
        {
            printValue(pop());
            printf("\n");
            return InterpretResult::INTERPRET_OK;
        }
        }
    }

    return InterpretResult::INTERPRET_RUNTIME_ERROR;

#undef BINARY_OP
}

inline unsigned __int8 VM::readByte()
{
    return *ip++;
}

inline Value VM::readConstant()
{
    return chunk->constants.values[readByte()];
}

void VM::resetStack()
{
    stackTop = stack;
}

void VM::push(Value value)
{
    *stackTop = value;
    stackTop++;
}

Value VM::pop()
{
    stackTop--;
    return *stackTop;
}
