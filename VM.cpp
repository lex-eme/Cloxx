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
#define BINARY_OP(valueType, op) \
    do { \
      if (!IS_NUMBER(peek(0)) || !IS_NUMBER(peek(1))) {\
        runtimeError("Operands must be numbers"); \
        return INTERPRET_RUNTIME_ERROR; \
      } \
      const double b = AS_NUMBER(pop()); \
      const double a = AS_NUMBER(pop()); \
      push(valueType(a op b)); \
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
        case OP_NIL: push(NIL_VAL); break;
        case OP_TRUE: push(BOOL_VAL(true)); break;
        case OP_FALSE: push(BOOL_VAL(false)); break;
        case OP_EQUAL: {
            const Value b = pop();
            const Value a = pop();
            push(BOOL_VAL(valuesEqual(a, b)));
            break;
        }
        case OP_GREATER:  BINARY_OP(BOOL_VAL, >); break;
        case OP_LESS:     BINARY_OP(BOOL_VAL, <); break;
        case OP_ADD:      BINARY_OP(NUMBER_VAL, +); break;
        case OP_SUBTRACT: BINARY_OP(NUMBER_VAL, -); break;
        case OP_MULTIPLY: BINARY_OP(NUMBER_VAL, *); break;
        case OP_DIVIDE:   BINARY_OP(NUMBER_VAL, /); break;
        case OP_NOT:
            push(BOOL_VAL(isFalsey(pop())));
            break;
        case OP_NEGATE:
            if (!IS_NUMBER(peek(0)))
            {
                runtimeError("Operand must be a number.");
                return INTERPRET_RUNTIME_ERROR;
            }
            push(NUMBER_VAL(-AS_NUMBER(pop())));
            break;
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

Value VM::peek(const int distance) const {
    return stackTop[-1 - distance];
}

bool VM::isFalsey(const Value value)
{
    return IS_NIL(value) || (IS_BOOL(value) && !AS_BOOL(value));
}

bool VM::valuesEqual(Value a, Value b)
{
    if (a.type != b.type)
    {
        return false;
    }

    switch (a.type)
    {
    case VAL_BOOL:      return AS_BOOL(a) == AS_BOOL(b);
    case VAL_NIL:       return true;
    case VAL_NUMBER:    return AS_NUMBER(a) == AS_NUMBER(b);
    default:            return false; // Unreachable
    }
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

void VM::runtimeError(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fputs("\n", stderr);

    const size_t instruction = ip - chunk->code.data() - 1;
    const int line = chunk->lines[instruction];
    fprintf(stderr, "[line %d] in script\n", line);
    resetStack();
}
