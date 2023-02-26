cmake_minimum_required(VERSION 3.24)

project(HatofiDB)
set(CMAKE_CXX_STANDARD 20)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra -mavx2")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_subdirectory(lib/fmt-9.1.0 EXCLUDE_FROM_ALL)

add_executable(hatofi lib/CLI11.hpp HaUtils.cpp HaUtils.h HaTable.cpp HaTable.h HaDB.cpp HaDB.h hatofi.cpp hatofi.h lib/uuid_v4.h lib/endianness.h lib/cpp-base64/base64.h lib/cpp-base64/base64.cpp lib/crypto/md5.h lib/crypto/md5.cpp Log.h Log.cpp)
find_package(fmt)
target_link_libraries(hatofi fmt::fmt)