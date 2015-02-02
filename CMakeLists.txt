cmake_minimum_required(VERSION 2.8.4)

project(h264_parser)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror")

set(SOURCE_FILES src/main.c src/h264-stream.c)

include_directories(${PROJECT_SOURCE_DIR}/deps "/usr/local/include")

link_directories("/usr/local/lib")

add_executable(h264_parser ${SOURCE_FILES})
