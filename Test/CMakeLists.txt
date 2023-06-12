cmake_minimum_required(VERSION 3.0.0)
project(TextEditorCoreTests CXX)

set(CMAKE_CXX_FLAGS "")
set(CMAKE_EXE_LINKER_FLAGS "")
set(CMAKE_CXX_STANDARD 20)

#======================================================

Include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.0.1 # or a later release
)
FetchContent_MakeAvailable(Catch2)

#======================================================

add_executable(TextEditorCoreTests
    main.cpp
    
    TextManagerTests.cpp
    FileManagerTests.cpp
    DocumentTests.cpp
)


target_link_libraries(TextEditorCoreTests
    PUBLIC TextEditorCoreLib
    PRIVATE Catch2::Catch2WithMain
)