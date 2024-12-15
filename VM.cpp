#include "VM.h"

#include <cstdio>
#include <cstdint>

#include "Debug.h"
#include "Compiler.h"
#include "Source.h"
#include "Value.h"
#include "Chunk.h"
#include "Common.h"

VM::VM()
{
    resetStack();
}

InterpretResult VM::interpret(const Source& source)
{
    Compiler compiler;
    chunk = new Chunk();
    if (!compiler.compile(source, chunk))
    {
        free(chunk);
        return INTERPRET_COMPILE_ERROR;
    }

    ip = chunk->code.data();

    const InterpretResult result = run();

    free(chunk);
    return result;
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
        for (const Value* slot = stack.data(); slot < stackTop; slot++)
        {
            printf("[ ");
            printValue(*slot);
            printf(" ]");
        }
        printf("\n");
        disassembleInstruction(*chunk, static_cast<int>(ip - chunk->code.data()));
    #endif

        const std::uint8_t instruction = readByte();
        switch (instruction)
        {
        case OP_CONSTANT:
        {
            const Value constant = readConstant();
            push(constant);
            break;
        }
        case OP_ADD:      BINARY_OP(+); break;
        case OP_SUBTRACT: BINARY_OP(-); break;
        case OP_MULTIPLY: BINARY_OP(*); break;
        case OP_DIVIDE:   BINARY_OP(/); break;
        case OP_NEGATE:   push(-pop()); break;
        case OP_RETURN:
        {
            printValue(pop());
            printf("\n");
            return INTERPRET_OK;
        }
            default: ;
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

void VM::push(const Value value)
{
    *stackTop = value;
    stackTop++;
}

Value VM::pop()
{
    stackTop--;
    return *stackTop;
}
