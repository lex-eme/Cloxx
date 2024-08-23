#include "VM.h"

#include <cstdio>
#include <cstdint>

#include "Debug.h"
#include "Compiler.h"
#include "Source.h"
#include "Value.h"
#include "Chunk.h"

VM::VM()
{
    resetStack();
}

InterpretResult VM::interpret(const Source& source)
{
    Compiler compiler;
    compiler.compile(source);
    return InterpretResult::INTERPRET_OK;
}

InterpretResult VM::run()
{
#define BINARY_OP(op) \
    do { \
      const double b = pop(); \
      const double a = pop(); \
      push(a op b); \
    } while (false)

    for (;;)
    {
    #ifdef DEBUG_TRACE_EXECUTION
        printf("          ");
        for (Value* slot = stack.data(); slot < stackTop; slot++)
        {
            printf("[ ");
            printValue(*slot);
            printf(" ]");
        }
        printf("\n");
        disassembleInstruction(*chunk, (int)(ip - chunk->code.data()));
    #endif

        std::uint8_t instruction = readByte();
        switch (instruction)
        {
        case OpCode::OP_CONSTANT:
        {
            const Value constant = readConstant();
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

inline std::uint8_t VM::readByte()
{
    return *ip++;
}

inline Value VM::readConstant()
{
    return chunk->constants.values[readByte()];
}

void VM::resetStack()
{
    stackTop = stack.data();
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
