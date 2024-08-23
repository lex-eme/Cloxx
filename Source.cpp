#include "Source.h"

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>

Source::Source(const char* path)
{
    std::ifstream file(path, std::ios::in | std::ios::ate);

    if (!file.is_open())
    {
        fprintf(stderr, "Could not open file \"%s\".\n", path);
        std::exit(74);
    }

    const std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    buffer = std::make_unique<char[]>(size + 1);
    if (buffer == nullptr) {
        fprintf(stderr, "Not enough memory to read \"%s\".\n", path);
        std::exit(74);
    }

    if (!file.read(buffer.get(), size))
    {
        fprintf(stderr, "Could not read file \"%s\".\n", path);
        std::exit(74);
    }

    buffer[size] = '\0';
}

char* Source::operator[](std::size_t idx) const
{
    return buffer.get() + idx;
}