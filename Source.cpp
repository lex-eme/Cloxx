#include "Source.h"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>

Source::Source(const char* path)
{
    std::ifstream file(path, std::ios::in | std::ios::ate);

    if (!file.is_open())
    {
        std::cerr << "Failed to open file \"" << path << "\"." << std::endl;
        std::exit(74);
    }

    const std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    buffer = std::make_unique<char[]>(size + 1);
    if (buffer == nullptr) {
        std::cerr << "Failed to allocate buffer." << std::endl;
        std::exit(74);
    }

    if (!file.read(buffer.get(), size))
    {
        std::cerr << "Failed to read file \"" << path << "\"." << std::endl;
        std::exit(74);
    }

    buffer[size] = '\0';
}

char* Source::operator[](const std::size_t idx) const
{
    return buffer.get() + idx;
}