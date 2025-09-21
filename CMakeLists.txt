cmake_minimum_required(VERSION 3.29)
project(Stockmanager C)

set(CMAKE_C_STANDARD 11)

# Include the sqlite3 library directory
include_directories(lib/sqlite)

# Add sqlite3.c as part of the project
add_executable(Stockmanager main.c lib/sqlite/sqlite3.c)
