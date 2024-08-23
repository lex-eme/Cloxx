#pragma once

#include <cstddef>
#include <memory>

class Source
{
public:
    explicit Source(const char* path);

    char* operator[](std::size_t idx) const;

private:
    std::unique_ptr<char[]> buffer;
};