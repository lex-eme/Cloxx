#include "Compiler.h"

#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <string>
#include <climits>
#include <iostream>

#include "Scanner.h"
#include "Source.h"
#include "Chunk.h"
#include "Value.h"
#include "Common.h"

#ifdef DEBUG_PRINT_CODE
#include "Debug.h"
#endif

bool Compiler::compile(const Source& source, Chunk* c)
{
    scanner = Scanner(source);
    parser.hadError = false;
    parser.panicMode = false;

    chunk = c;
    advance();
    expression();
    consume(TokenType::TOKEN_EOF, "Expect end of expression.");
    endCompiler();
    return !parser.hadError;
}

void Compiler::advance()
{
    parser.previous = parser.current;

    for (;;)
    {
        parser.current = scanner.nextToken();
        if (parser.current.type != TOKEN_ERROR)
        {
            break;
        }

        errorAtCurrent(parser.current.start);
    }
}

void Compiler::consume(TokenType type, const std::string& message)
{
    if (parser.current.type == type)
    {
        advance();
        return;
    }

    errorAtCurrent(message);
}

void Compiler::emitByte(const std::uint8_t byte) const {
    chunk->writeChunk(byte, parser.previous.line);
}

void Compiler::emitBytes(const std::uint8_t byte1, const std::uint8_t byte2) const {
    emitByte(byte1);
    emitByte(byte2);
}

void Compiler::emitReturn() const {
    emitByte(OP_RETURN);
}

void Compiler::emitConstant(const Value value)
{
    emitBytes(OP_CONSTANT, makeConstant(value));
}

std::uint8_t Compiler::makeConstant(const Value value)
{
    const int constant = chunk->addConstant(value);
    if (constant > UCHAR_MAX)
    {
        error("Too many constants in one chunk.");
        return 0;
    }

    return static_cast<std::uint8_t>(constant);
}

void Compiler::endCompiler() const {
    emitReturn();

#ifdef DEBUG_PRINT_CODE
    if (!parser.hadError) {
        disassembleChunk(*chunk, "code");
    }
#endif
}

void Compiler::expression()
{
    parsePrecedence(ASSIGNMENT);
}

void Compiler::number()
{
    const double value = strtod(parser.previous.start, nullptr);
    emitConstant(NUMBER_VAL(value));
}

void Compiler::unary()
{
    const TokenType operatorType = parser.previous.type;

    parsePrecedence(UNARY);

    switch (operatorType)
    {
    case TOKEN_BANG: emitByte(OP_NOT); break;
    case TOKEN_MINUS: emitByte(OP_NEGATE); break;
    default: return;
    }
}

void Compiler::binary()
{
    const TokenType operatorType = parser.previous.type;
    const auto rule = getRule(operatorType);
    parsePrecedence(static_cast<Precedence>(rule.precedence + 1)); // +1 because each binary operator's right hand operand is one level higher than its own. (Binary operator are left-associative)

    switch(operatorType)
    {
    case TOKEN_BANG_EQUAL:      emitBytes(OP_EQUAL, OP_NOT); break;
    case TOKEN_EQUAL_EQUAL:     emitByte(OP_EQUAL); break;
    case TOKEN_GREATER:         emitByte(OP_GREATER); break;
    case TOKEN_GREATER_EQUAL:   emitBytes(OP_LESS, OP_NOT); break;
    case TOKEN_LESS:            emitByte(OP_LESS); break;
    case TOKEN_LESS_EQUAL:      emitBytes(OP_GREATER, OP_NOT); break;
    case TOKEN_PLUS:            emitByte(OP_ADD); break;
    case TOKEN_MINUS:           emitByte(OP_SUBTRACT); break;
    case TOKEN_STAR:            emitByte(OP_MULTIPLY); break;
    case TOKEN_SLASH:           emitByte(OP_DIVIDE); break;
    default: return; // Unreachable
    }
}

void Compiler::literal()
{
    switch (parser.previous.type)
    {
        case TOKEN_FALSE: emitByte(OP_FALSE); break;
        case TOKEN_NIL: emitByte(OP_NIL); break;
        case TOKEN_TRUE: emitByte(OP_TRUE); break;
        default: return; // Unreachable
    }
}

void Compiler::grouping()
{
    expression();
    consume(TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
}

void Compiler::parsePrecedence(const Precedence precedence)
{
    advance();
    const auto prefixRule = getRule(parser.previous.type).prefix;

    if (prefixRule == nullptr)
    {
        error("Expect expression.");
        return;
    }

    (this->*prefixRule)();

    while (precedence <= getRule(parser.current.type).precedence) {
        advance();
        const auto infixRule = getRule(parser.previous.type).infix;
        (this->*infixRule)();
    }
}

const ParseRule& Compiler::getRule(const TokenType type) {
    static const ParseRule rules[] =
     {
         {&Compiler::grouping,      nullptr,                Precedence::NONE},          // LEFT_PAREN
         {nullptr,                  nullptr,                Precedence::NONE},          // RIGHT_PAREN
         {nullptr,                  nullptr,                Precedence::NONE},          // LEFT_BRACE
         {nullptr,                  nullptr,                Precedence::NONE},          // RIGHT_BRACE
         {nullptr,                  nullptr,                Precedence::NONE},          // COMMA
         {nullptr,                  nullptr,                Precedence::NONE},          // DOT
         {&Compiler::unary,         &Compiler::binary,      Precedence::TERM},          // MINUS
         {nullptr,                  &Compiler::binary,      Precedence::TERM},          // PLUS
         {nullptr,                  nullptr,                Precedence::NONE},          // SEMICOLON
         {nullptr,                  &Compiler::binary,      Precedence::FACTOR},        // SLASH
         {nullptr,                  &Compiler::binary,      Precedence::FACTOR},        // STAR
         {&Compiler::unary,         nullptr,                Precedence::NONE},          // BANG
         {nullptr,                  &Compiler::binary,      Precedence::EQUALITY},      // BANG_EQUAL
         {nullptr,                  nullptr,                Precedence::NONE},          // EQUAL
         {nullptr,                  &Compiler::binary,      Precedence::EQUALITY},      // EQUAL_EQUAL
         {nullptr,                  &Compiler::binary,      Precedence::COMPARISON},    // GREATER
         {nullptr,                  &Compiler::binary,      Precedence::COMPARISON},    // GREATER_EQUAL
         {nullptr,                  &Compiler::binary,      Precedence::COMPARISON},    // LESS
         {nullptr,                  &Compiler::binary,      Precedence::COMPARISON},    // LESS_EQUAL
         {nullptr,                  nullptr,                Precedence::NONE},          // IDENTIFIER
         {nullptr,                  nullptr,                Precedence::NONE},          // STRING
         {&Compiler::number,        nullptr,                Precedence::NONE},          // NUMBER
         {nullptr,                  nullptr,                Precedence::NONE},          // AND
         {nullptr,                  nullptr,                Precedence::NONE},          // CLASS
         {nullptr,                  nullptr,                Precedence::NONE},          // ELSE
         {&Compiler::literal,       nullptr,                Precedence::NONE},          // FALSE
         {nullptr,                  nullptr,                Precedence::NONE},          // FOR
         {nullptr,                  nullptr,                Precedence::NONE},          // FUN
         {nullptr,                  nullptr,                Precedence::NONE},          // IF
         {&Compiler::literal,       nullptr,                Precedence::NONE},          // NIL
         {nullptr,                  nullptr,                Precedence::NONE},          // OR
         {nullptr,                  nullptr,                Precedence::NONE},          // PRINT
         {nullptr,                  nullptr,                Precedence::NONE},          // RETURN
         {nullptr,                  nullptr,                Precedence::NONE},          // SUPER
         {nullptr,                  nullptr,                Precedence::NONE},          // THIS
         {&Compiler::literal,       nullptr,                Precedence::NONE},          // TRUE
         {nullptr,                  nullptr,                Precedence::NONE},          // VAR
         {nullptr,                  nullptr,                Precedence::NONE},          // WHILE
         {nullptr,                  nullptr,                Precedence::NONE},          // ERROR
         {nullptr,                  nullptr,                Precedence::NONE},          // EOF
     };

     return rules[type];
}

void Compiler::errorAtCurrent(const std::string& message)
{
    errorAt(parser.current, message);
}

void Compiler::error(const std::string& message)
{
    errorAt(parser.previous, message);
}

void Compiler::errorAt(const Token& token, const std::string& message)
{
    if (parser.panicMode)
    {
        return;
    }

    parser.panicMode = true;
    fprintf(stderr, "[line %d] Error", token.line);

    if (token.type == TOKEN_EOF)
    {
        fprintf(stderr, " at end");
    }
    else if (token.type == TOKEN_ERROR)
    {
        // Nothing
    }
    else
    {
        fprintf(stderr, " at '%.*s'", token.length, token.start);
    }

    fprintf(stderr, ": %s\n", message.c_str());
    parser.hadError = true;
}