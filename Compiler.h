#pragma once

#include <cstdint>

#include "Source.h"
#include "Chunk.h"
#include "Scanner.h"
#include "Value.h"

class Compiler;

struct Parser
{
    Token current;
    Token previous;
    bool hadError = false;
    bool panicMode = false;
};

enum Precedence: std::uint8_t
{
    NONE,
    ASSIGNMENT,     // =
    OR,             // or
    AND,            // and
    EQUALITY,       // == !=
    COMPARISON,     // < > <= >=
    TERM,           // + -
    FACTOR,         // * /
    UNARY,          // ! =
    CALL,           // . ()
    PRIMARY
};

struct ParseRule {
    void (Compiler::* prefix)() = nullptr;
    void (Compiler::* infix)() = nullptr;
    Precedence precedence;
};

class Compiler
{
public:
    Compiler() = default;

    bool compile(const Source& source, Chunk* chunk);

    void grouping();
    void number();
    void unary();
    void binary();
    void literal();

private:
    Scanner scanner{};
    Parser parser;
    Chunk* chunk = nullptr;

    void advance();
    void consume(TokenType type, const std::string& message);
    void emitByte(std::uint8_t byte) const;
    void emitBytes(std::uint8_t byte1, std::uint8_t byte2) const;
    void emitReturn() const;
    void emitConstant(Value value);
    std::uint8_t makeConstant(Value value);
    void endCompiler() const;

    void expression();

    void parsePrecedence(Precedence precedence);
    static const ParseRule& getRule(TokenType type);

    void errorAtCurrent(const std::string& message);
    void error(const std::string& message);
    void errorAt(const Token& token, const std::string& message);
};
