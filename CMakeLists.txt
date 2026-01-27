cmake_minimum_required(VERSION 4.1)
project(aramTool)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_executable(aramTool
#        src/main.cpp
        include/database.h
        src/database.cpp
#        testcase/database_test.cpp
#        src/crawler.cpp
#        include/crawler.h
#        include/crow.h
)

# include 경로
target_include_directories(aramTool PRIVATE include)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/templates DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
find_package(Threads REQUIRED)

target_link_libraries(aramTool
        PUBLIC pthread
        PRIVATE Threads::Threads
)

