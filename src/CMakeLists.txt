cmake_minimum_required(VERSION 3.24)

project("hatofi-db")

set(CMAKE_CXX_STANDARD 20)

add_executable(gen_database gen_database.c gen_database.h)
add_executable(hatofi lib/CLI11.hpp HaUtils.cpp HaUtils.h HaTable.cpp HaTable.h HaDB.cpp HaDB.h hatofi.cpp hatofi.h)
