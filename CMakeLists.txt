# FreeCube CMake file
# (c) 2025 libre-hobbyists
# SPDX-License-Identifier: GPL-3.0-only
#

cmake_minimum_required(VERSION 4.1.0)

project(freecube
  VERSION 0.0.1
  DESCRIPTION "A free, open-source Gamecube emulator."
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
    set_compile_options(/W4 /WX)
  else()
    set_compile_options(-Wall -Wextra -Wpedantic -Werror)
  endif()
else()
  if(MSVC)
    set_compile_options(/W2)
  else()
    set_compile_options(-Wall -Wextra)
  endif()
endif()

include_directories(${CMAKE_SOURCE_DIR}/include)

set(FREECUBE_SOURCES
  # Add sources as they're created
)

set(FREECUBE_HEADERS
  # Add headers as they're created
)
