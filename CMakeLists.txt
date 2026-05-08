cmake_minimum_required(VERSION 4.2)
project(testProjects)

set(CMAKE_CXX_STANDARD 20)

add_executable(testProjects main.cpp
)

# --- ADD THIS PART ---
# This tells the compiler to be strict and help you find bugs
target_compile_options(testProjects PRIVATE

)