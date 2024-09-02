cmake_minimum_required(VERSION 3.12)
project(libfive)

# Properly distinguish between Apple and upstream Clang
cmake_policy(SET CMP0025 NEW)
include(CheckCXXCompilerFlag)

option(BUILD_STUDIO_APP "Build Studio application" OFF)
option(BUILD_GUILE_BINDINGS "Build Guile bindings" OFF)
option(BUILD_PYTHON_BINDINGS "Build Python bindings" OFF)

option(LIBFIVE_PACKED_OPCODES
    "Tightly pack opcodes (breaks compatibility with older saved f-reps)"
    OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RELEASE)
endif()

if (LIBFIVE_PACKED_OPCODES)
    message("Using packed opcodes, which breaks compatibility with saved f-reps!")
    add_definitions(-DLIBFIVE_PACKED_OPCODES)
endif()

# 查找和启用 OpenMP
find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    message("OpenMP 已找到并启用")
    add_definitions(${OpenMP_CXX_DEFINITIONS})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

# 如果找到 ccache 则启用 ccache
set(LIBFIVE_CCACHE_BUILD OFF CACHE BOOL "Set to ON for a ccache enabled build")
if(LIBFIVE_CCACHE_BUILD)
  find_program(CCACHE_PROGRAM ccache)
  if(CCACHE_PROGRAM)
      set(LIBFIVE_CCACHE_MAXSIZE "" CACHE STRING "Size of ccache")
      set(LIBFIVE_CCACHE_DIR "" CACHE STRING "Directory to keep ccached data")
      set(LIBFIVE_CCACHE_PARAMS "CCACHE_CPP2=yes CCACHE_HASHDIR=yes"
          CACHE STRING "Parameters to pass through to ccache")

      set(CCACHE_PROGRAM "${LIBFIVE_CCACHE_PARAMS} ${CCACHE_PROGRAM}")
      if (LIBFIVE_CCACHE_MAXSIZE)
        set(CCACHE_PROGRAM "CCACHE_MAXSIZE=${LIBFIVE_CCACHE_MAXSIZE} ${CCACHE_PROGRAM}")
      endif()
      if (LIBFIVE_CCACHE_DIR)
        set(CCACHE_PROGRAM "CCACHE_DIR=${LIBFIVE_CCACHE_DIR} ${CCACHE_PROGRAM}")
      endif()
      set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ${CCACHE_PROGRAM})
  else()
    message(FATAL_ERROR "Unable to find the program ccache. Set LIBFIVE_CCACHE_BUILD to OFF")
  endif()
endif()

find_package(PNG REQUIRED)
################################################################################

if(NOT MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -g -fPIC -pedantic -Werror=switch")
    # Additional flags for OpenMP
else()
    message("  Python bindings:   ✘   (skipping)")
endif()

if (BUILD_STUDIO_APP)
  if (GUILE_FOUND AND BUILD_GUILE_BINDINGS)
    set(STUDIO_WITH_GUILE ON)
  else()
    set(STUDIO_WITH_GUILE OFF)
  endif()

  if (Python3_FOUND AND BUILD_PYTHON_BINDINGS)
    set(STUDIO_WITH_PYTHON ON)
  else()
    set(STUDIO_WITH_PYTHON OFF)
  endif()

  if (QtCore_FOUND AND STUDIO_WITH_GUILE AND STUDIO_WITH_PYTHON)
    message("  Studio:          ✓   (Python + Guile)")
  elseif (QtCore_FOUND AND STUDIO_WITH_GUILE)
    message("  Studio:          ✓   (Guile only)")
  elseif (QtCore_FOUND AND STUDIO_WITH_PYTHON)
    message("  Studio:          ✓   (Python only)")
  else ()
    if (QtCore_FOUND)
      message("  Studio:                ✘   (needs Guile or Python)")
    elseif (STUDIO_WITH_GUILE OR STUDIO_WITH_PYTHON)
      message("  Studio:                ✘   (needs Qt 5.12 or later)")
    else()
      message("  Studio:                ✘   (needs Qt 5.12 or later, and Guile or Python")
    endif()
  endif()
else()
    message("  Studio:          ✘   (skipping)")
endif()

################################################################################

# 设置标志以检测用户在错误目录中运行 CMake 的情况
set(LIBFIVE_BUILD_FROM_ROOT true)

# 始终构建内核和测试套件
add_subdirectory(libfive)

if(BUILD_STUDIO_APP AND (STUDIO_WITH_GUILE OR STUDIO_WITH_PYTHON) AND QtCore_FOUND)
    add_subdirectory(studio)
endif()

