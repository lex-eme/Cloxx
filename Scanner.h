#pragma once

#include <cstdint>

#include "Source.h"

enum TokenType: std::uint8_t
{
    // Single-character tokens.
    TOKEN_LEFT_PAREN, TOKEN_RIGHT_PAREN,
    TOKEN_LEFT_BRACE, TOKEN_RIGHT_BRACE,
    TOKEN_COMMA, TOKEN_DOT, TOKEN_MINUS, TOKEN_PLUS,
    TOKEN_SEMICOLON, TOKEN_SLASH, TOKEN_STAR,
    // One or two character tokens.
    TOKEN_BANG, TOKEN_BANG_EQUAL,
    TOKEN_EQUAL, TOKEN_EQUAL_EQUAL,
    TOKEN_GREATER, TOKEN_GREATER_EQUAL,
    TOKEN_LESS, TOKEN_LESS_EQUAL,
    // Literals.
    TOKEN_IDENTIFIER, TOKEN_STRING, TOKEN_NUMBER,
    // Keywords.
    TOKEN_AND, TOKEN_CLASS, TOKEN_ELSE, TOKEN_FALSE,
    TOKEN_FOR, TOKEN_FUN, TOKEN_IF, TOKEN_NIL, TOKEN_OR,
    TOKEN_PRINT, TOKEN_RETURN, TOKEN_SUPER, TOKEN_THIS,
    TOKEN_TRUE, TOKEN_VAR, TOKEN_WHILE,

    TOKEN_ERROR, TOKEN_EOF
};

struct Token
{
    TokenType type;
    const char* start;
    int length;
    int line;
};

class Scanner
{
public:
    const char* start;
    const char* current;
    int line;

    explicit Scanner(const Source& source);
    Token nextToken();

private:
    static bool isDigit(char c);
    static bool isAlpha(char c);

    bool isAtEnd() const;
    Token makeToken(TokenType type) const;
    Token errorToken(const char* message) const;
    char advance();
    bool match(char expected);
    void skipWhitespace();
    char peek() const;
    char peekNext();
    Token string();
    Token number();
    Token identifier();
    TokenType identifierType();
    TokenType checkKeyword(int start, int length, const char* rest, TokenType type) const;
};
