# FreeCube CMake file
# (c) 2025 libre-hobbyists
# SPDX-License-Identifier: GPL-3.0-only
#

cmake_minimum_required(VERSION 4.1.0)

project(freecube
  VERSION 0.0.1
  DESCRIPTION "A free, open-source GameCube emulator."
  LANGUAGES CXX C
)

# Require C++17 and C17 with NO extensions!
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Exporting those compile commands
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(FREECUBE_BUILD_TESTS "Build unit tests" OFF)
option(FREECUBE_PEDANTIC "Compiles with maximum errorchecking, flags all warnings as errors" OFF)

if(FREECUBE_PEDANTIC)
  if(MSVC)
    add_compile_options(/W4 /WX)
  else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
  endif()
else()
  if(MSVC)
    add_compile_options(/W2)
  else()
    add_compile_options(-Wall -Wextra)
  endif()
endif()

include_directories(${CMAKE_SOURCE_DIR}/include)

set(FREECUBE_SOURCES
  ${CMAKE_SOURCE_DIR}/src/main.cpp
  ${CMAKE_SOURCE_DIR}/src/validate.cpp
  ${CMAKE_SOURCE_DIR}/src/dol_loader.cpp
)

set(FREECUBE_HEADERS
  ${CMAKE_SOURCE_DIR}/include/util/log.hpp
  ${CMAKE_SOURCE_DIR}/include/dol/validate.hpp
  ${CMAKE_SOURCE_DIR}/include/dol/dol_loader.hpp
  ${CMAKE_SOURCE_DIR}/include/loader/iso.hpp
  ${CMAKE_SOURCE_DIR}/include/loader/loader.hpp
)

# Handy compile time definitions
string(TIMESTAMP FREECUBE_BUILD_TIME "%Y-%m-%dT%H:%M:%SZ" UTC)

add_compile_definitions(FREECUBE_CMAKE_SYSTEM_TARGET="${CMAKE_SYSTEM_NAME}")
add_compile_definitions(FREECUBE_CMAKE_ARCH_TARGET="${CMAKE_SYSTEM_PROCESSOR}")
add_compile_definitions(FREECUBE_CMAKE_BUILD_TIME="${FREECUBE_BUILD_TIME}")

add_executable(freecube ${FREECUBE_SOURCES} ${FREECUBE_HEADERS})