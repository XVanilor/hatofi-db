cmake_minimum_required(VERSION 3.24)

project(HatofiDB)
set(CMAKE_CXX_STANDARD 20)

add_subdirectory(lib/fmt-9.1.0 EXCLUDE_FROM_ALL)

add_executable(hatofi lib/CLI11.hpp HaUtils.cpp HaUtils.h HaTable.cpp HaTable.h HaDB.cpp HaDB.h hatofi.cpp hatofi.h)
find_package(fmt)
target_link_libraries(hatofi fmt::fmt)