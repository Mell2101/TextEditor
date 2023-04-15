cmake_minimum_required(VERSION 3.0.0)
project(TextEditorCoreTests CXX)

set(CMAKE_CXX_FLAGS "")
set(CMAKE_EXE_LINKER_FLAGS "")
set(CMAKE_CXX_STANDARD 20)

add_executable(TextEditorCoreTests
    main.cpp
    
    TextManagerTests.cpp
)

target_link_libraries(TextEditorCoreTests PUBLIC TextEditorCoreLib)