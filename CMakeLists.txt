cmake_minimum_required(VERSION 4.1)
project(aramTool)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_executable(aramTool
#        src/main.cpp
#        src/mongoose.c
#        include/database.h
#        src/database.cpp
        testcase/main.cpp
)

# include 경로
target_include_directories(aramTool PRIVATE include)

# pthread 링크 (리눅스에서 필수)
target_link_libraries(aramTool PRIVATE pthread)

