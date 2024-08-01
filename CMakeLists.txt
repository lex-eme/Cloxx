cmake_minimum_required(VERSION 3.30)

project(Cloxx VERSION 0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(${PROJECT_NAME}
    main.cpp
    Chunk.cpp
    Value.cpp
    Debug.cpp
    )
