#include "Compiler.h"

#include <cstdio>

#include "Scanner.h"
#include "Source.h"


void Compiler::compile(const Source& source)
{
    Scanner scanner(source);

    int line = -1;
    for (;;)
    {
        const Token token = scanner.nextToken();
        if (token.line != line)
        {
            printf("%4d ", token.line);
            line = token.line;
        }
        else
        {
            printf("   | ");
        }
        printf("%2d '%.*s'\n", token.type, token.length, token.start);

        if (token.type == TOKEN_EOF)
        {
            break;
        }
    }
}
