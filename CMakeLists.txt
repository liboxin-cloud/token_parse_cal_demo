# 指定 CMake 最低版本要求
cmake_minimum_required(VERSION 3.10)

# 项目名称（可自定义）
project(token_parser_calculator)

# 设置 C 标准（根据需要调整，例如 C99、C11）
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# 添加源文件
# 这里假设 test.c 是主程序入口，parser.c 是实现文件
set(SOURCES
    test.c
    parser.c
)

# 添加头文件路径（如果有自定义头文件目录需要指定，这里当前目录已包含）
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# 生成可执行文件（可执行文件名自定义，例如 token_calc）
add_executable(token_calc ${SOURCES})
