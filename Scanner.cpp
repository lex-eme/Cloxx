#include "Scanner.h"

#include <cstring>

#include "Source.h"

Scanner::Scanner(const Source& source)
    : start(source[0])
    , current(source[0])
    , line(1)
{
}

bool Scanner::isDigit(const char c)
{
    return c >= '0' && c <= '9';
}

bool Scanner::isAlpha(const char c)
{
    return  (c >= 'a' && c <= 'z') ||
            (c >= 'A' && c <= 'Z') ||
            c == '_';
}

Token Scanner::nextToken()
{
    skipWhitespace();
    start = current;

    if (isAtEnd()) return makeToken(TOKEN_EOF);

    const char c = advance();
    if (isAlpha(c)) return identifier();
    if (isDigit(c)) return number();

    switch(c)
    {
        case '(': return makeToken(TOKEN_LEFT_PAREN);
        case ')': return makeToken(TOKEN_RIGHT_PAREN);
        case '{': return makeToken(TOKEN_LEFT_BRACE);
        case '}': return makeToken(TOKEN_RIGHT_BRACE);
        case ';': return makeToken(TOKEN_SEMICOLON);
        case ',': return makeToken(TOKEN_COMMA);
        case '.': return makeToken(TOKEN_DOT);
        case '-': return makeToken(TOKEN_MINUS);
        case '+': return makeToken(TOKEN_PLUS);
        case '/': return makeToken(TOKEN_SLASH);
        case '*': return makeToken(TOKEN_STAR);
        case '!':
            return makeToken(match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
        case '=':
            return makeToken(match('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
        case '<':
            return makeToken(match('=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
        case '>':
            return makeToken(match('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
        case '"':
            return string();
        default: ;
    }

    return errorToken("Unexpected character.");
}

bool Scanner::isAtEnd() const
{
    return *current == '\0';
}

Token Scanner::makeToken(const TokenType type) const
{
    return Token{type, start, static_cast<int>(current - start), line};
}

Token Scanner::errorToken(const char* message) const
{
    return Token{TokenType::TOKEN_ERROR, message, static_cast<int>(strlen(message)), line};
}

char Scanner::advance()
{
    current++;
    return current[-1];
}

bool Scanner::match(const char expected)
{
    if (isAtEnd()) return false;
    if (*current != expected) return false;
    current++;
    return true;
}

void Scanner::skipWhitespace()
{
    for (;;)
    {
        const char c = peek();
        switch(c)
        {
        case ' ':
        case '\r':
        case '\t':
            advance();
            break;
        case '\n':
            line++;
            advance();
            break;
        case '/':
            if (peekNext() == '/')
                while (peek() != '\n' && !isAtEnd()) advance();
            else
                return;
        default:
            return;
        }
    }
}

char Scanner::peek() const
{
    return *current;
}

char Scanner::peekNext() const {
    if (isAtEnd()) return '\0';
    return current[1];
}

Token Scanner::string()
{
    while (peek() != '"' && !isAtEnd())
    {
        if (peek() == '\n') line++;
        advance();
    }

    if (isAtEnd()) return errorToken("Unterminated string.");

    advance();
    return makeToken(TOKEN_STRING);
}

Token Scanner::number()
{
    while (isDigit(peek())) advance();

    if (peek() == '.' && isDigit(peekNext()))
    {
        advance();

        while (isDigit(peek())) advance();
    }

    return makeToken(TOKEN_NUMBER);
}

Token Scanner::identifier()
{
    while (isAlpha(peek()) || isDigit(peek())) advance();
    return makeToken(identifierType());
}

TokenType Scanner::identifierType() const {
    switch (start[0])
    {
    case 'a': return checkKeyword(1, 2, "nd", TOKEN_AND);
    case 'c': return checkKeyword(1, 4, "lass", TOKEN_CLASS);
    case 'e': return checkKeyword(1, 3, "lse", TOKEN_ELSE);
    case 'i': return checkKeyword(1, 1, "f", TOKEN_IF);
    case 'f':
        if (current - start > 1)
        {
            switch (start[1])
            {
            case 'a': return checkKeyword(2, 3, "lse", TOKEN_FALSE);
            case 'o': return checkKeyword(2, 1, "r", TOKEN_FOR);
            case 'n': return checkKeyword(2, 1, "n", TOKEN_FUN);
            }
        }
        break;
    case 'n': return checkKeyword(1, 2, "il", TOKEN_NIL);
    case 'o': return checkKeyword(1, 1, "r", TOKEN_OR);
    case 'p': return checkKeyword(1, 4, "rint", TOKEN_PRINT);
    case 'r': return checkKeyword(1, 5, "eturn", TOKEN_RETURN);
    case 's': return checkKeyword(1, 4, "uper", TOKEN_SUPER);
    case 't':
        if (current - start > 1)
        {
            switch (start[1])
            {
            case 'h': return checkKeyword(2, 2, "is", TOKEN_THIS);
            case 'r': return checkKeyword(2, 2, "ue", TOKEN_TRUE);
            }
        }
    case 'v': return checkKeyword(1, 2, "ar", TOKEN_VAR);
    case 'w': return checkKeyword(1, 4, "hile", TOKEN_WHILE);
    }
    return TOKEN_IDENTIFIER;
}

TokenType Scanner::checkKeyword(const int s, const int length, const char* rest, const TokenType type) const
{
    if (current - start == s + length && memcmp(start + s, rest, length) == 0)
    {
        return type;
    }

    return TOKEN_IDENTIFIER;
}
