cmake_minimum_required(VERSION 3.28)

project(Cloxx VERSION 0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

add_executable(${PROJECT_NAME}
    Cloxx.cpp
    Chunk.cpp
    Value.cpp
    Debug.cpp
    VM.cpp
    Compiler.cpp
    Scanner.cpp
    )
