cmake_minimum_required(VERSION 3.0.0)
project(TextEditorCoreLib CXX)

set(CMAKE_CXX_FLAGS "")
set(CMAKE_EXE_LINKER_FLAGS "")
set(CMAKE_CXX_STANDARD 20)

add_compile_options(
    -fPIC
    
    # -Werror
    -Wall
    -Wno-comment
    -Wno-switch
    
    -Wextra
    -Wno-unused-parameter
    
    -Wpedantic
    -Wcast-align
    -Wcast-qual
    
    # -Wconversion
    -Wctor-dtor-privacy
    -Wenum-compare
    -Wfloat-equal
    -Wnon-virtual-dtor
    
    # -Wold-style-cast
    -Woverloaded-virtual
    -Wredundant-decls
    
    # -Wsign-conversion
    -Wsign-promo
    -Wno-unknown-pragmas
)

find_package(Threads REQUIRED)

add_library(TextEditorCoreLib STATIC
    FileManager.h
    FileManager.cpp
    
    TextManager.h
    TextManager.cpp
)

target_link_libraries(TextEditorCoreLib
    PRIVATE Threads::Threads
)

target_include_directories(TextEditorCoreLib PUBLIC .)