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
    emitConstant(value);
}

void Compiler::unary()
{
    const TokenType operatorType = parser.previous.type;

    parsePrecedence(UNARY);

    switch (operatorType)
    {
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
    case TOKEN_PLUS:     emitByte(OP_ADD); break;
    case TOKEN_MINUS:    emitByte(OP_SUBTRACT); break;
    case TOKEN_STAR:     emitByte(OP_MULTIPLY); break;
    case TOKEN_SLASH:    emitByte(OP_DIVIDE); break;
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
         {&Compiler::grouping,      nullptr,                Precedence::NONE},
         {nullptr,                  nullptr,                Precedence::NONE},
         {nullptr,                  nullptr,                Precedence::NONE},
         {nullptr,                  nullptr,                Precedence::NONE},
         {nullptr,                  nullptr,                Precedence::NONE},
         {nullptr,                  nullptr,                Precedence::NONE},
         {&Compiler::unary,         &Compiler::binary,      Precedence::TERM},
         {nullptr,                  &Compiler::binary,      Precedence::TERM},
         {nullptr,                  nullptr,                Precedence::NONE},
         {nullptr,                  &Compiler::binary,      Precedence::FACTOR},
         {nullptr,                  &Compiler::binary,      Precedence::FACTOR},
         {nullptr,                  nullptr,                Precedence::NONE},
         {nullptr,                  nullptr,                Precedence::NONE},
         {nullptr,                  nullptr,                Precedence::NONE},
         {nullptr,                  nullptr,                Precedence::NONE},
         {nullptr,                  nullptr,                Precedence::NONE},
         {nullptr,                  nullptr,                Precedence::NONE},
         {nullptr,                  nullptr,                Precedence::NONE},
         {nullptr,                  nullptr,                Precedence::NONE},
         {nullptr,                  nullptr,                Precedence::NONE},
         {nullptr,                  nullptr,                Precedence::NONE},
         {&Compiler::number,        nullptr,                Precedence::NONE},
         {nullptr,                  nullptr,                Precedence::NONE},
         {nullptr,                  nullptr,                Precedence::NONE},
         {nullptr,                  nullptr,                Precedence::NONE},
         {nullptr,                  nullptr,                Precedence::NONE},
         {nullptr,                  nullptr,                Precedence::NONE},
         {nullptr,                  nullptr,                Precedence::NONE},
         {nullptr,                  nullptr,                Precedence::NONE},
         {nullptr,                  nullptr,                Precedence::NONE},
         {nullptr,                  nullptr,                Precedence::NONE},
         {nullptr,                  nullptr,                Precedence::NONE},
         {nullptr,                  nullptr,                Precedence::NONE},
         {nullptr,                  nullptr,                Precedence::NONE},
         {nullptr,                  nullptr,                Precedence::NONE},
         {nullptr,                  nullptr,                Precedence::NONE},
         {nullptr,                  nullptr,                Precedence::NONE},
         {nullptr,                  nullptr,                Precedence::NONE},
         {nullptr,                  nullptr,                Precedence::NONE},
         {nullptr,                  nullptr,                Precedence::NONE},
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