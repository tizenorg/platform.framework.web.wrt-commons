# Copyright (c) 2011 Samsung Electronics Co., Ltd All Rights Reserved
#
#    Licensed under the Apache License, Version 2.0 (the "License");
#    you may not use this file except in compliance with the License.
#    You may obtain a copy of the License at
#
#        http://www.apache.org/licenses/LICENSE-2.0
#
#    Unless required by applicable law or agreed to in writing, software
#    distributed under the License is distributed on an "AS IS" BASIS,
#    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#    See the License for the specific language governing permissions and
#    limitations under the License.
#
#
# @file        CMakeLists.txt
# @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
# @version     1.0
# @brief
#

# Check minimum CMake version
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

# Project name
PROJECT(dpl)

STRING(REGEX MATCH "([^.]*)" API_VERSION "${VERSION}")
ADD_DEFINITIONS("-DAPI_VERSION=\"$(API_VERSION)\"")

# Comment this to disable control of global settings with environment variable
ADD_DEFINITIONS("-DGLOBAL_SETTINGS_CONTROL")

# Build type
IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE "Release")
ENDIF(NOT CMAKE_BUILD_TYPE)

# Options
OPTION(DPL_LOG "DPL logs status" OFF)
IF(DPL_LOG AND NOT CMAKE_BUILD_TYPE MATCHES "profiling")
    MESSAGE(STATUS "Logging enabled for DPL")
    ADD_DEFINITIONS("-DDPL_LOGS_ENABLED")
ELSE(DPL_LOG AND NOT CMAKE_BUILD_TYPE MATCHES "profiling")
    MESSAGE(STATUS "Logging disabled for DPL")
ENDIF(DPL_LOG AND NOT CMAKE_BUILD_TYPE MATCHES "profiling")

OPTION(WITH_TESTS "Build tests" OFF)

# Compiler flags
SET(CMAKE_C_FLAGS_PROFILING    "-O2")
SET(CMAKE_CXX_FLAGS_PROFILING  "-O2 -std=c++0x")
SET(CMAKE_C_FLAGS_DEBUG        "-O0 -g")
SET(CMAKE_CXX_FLAGS_DEBUG      "-O0 -std=c++0x -g")
SET(CMAKE_C_FLAGS_RELEASE      "-O2 -g")
SET(CMAKE_CXX_FLAGS_RELEASE    "-O2 -std=c++0x -g")
SET(CMAKE_CXX_FLAGS_CCOV       "-O0 -std=c++0x -g --coverage")

ADD_DEFINITIONS("-fPIC")                        # If supported for the target machine, emit position-independent code, suitable for dynamic linking and avoiding any limit on the size of the global offset table. This option makes a difference on the m68k, PowerPC and SPARC. (BJ: our ARM too?)

# CMake settings
MESSAGE(STATUS "========================================")
MESSAGE(STATUS "CMAKE_BINARY_DIR:         " ${CMAKE_BINARY_DIR})
MESSAGE(STATUS "CMAKE_CURRENT_BINARY_DIR: " ${CMAKE_CURRENT_BINARY_DIR})
MESSAGE(STATUS "CMAKE_SOURCE_DIR:         " ${CMAKE_SOURCE_DIR})
MESSAGE(STATUS "CMAKE_CURRENT_SOURCE_DIR: " ${CMAKE_CURRENT_SOURCE_DIR})
MESSAGE(STATUS "PROJECT_BINARY_DIR: " ${PROJECT_BINARY_DIR})
MESSAGE(STATUS "PROJECT_SOURCE_DIR: " ${PROJECT_SOURCE_DIR})
MESSAGE(STATUS "EXECUTABLE_OUTPUT_PATH: " ${EXECUTABLE_OUTPUT_PATH})
MESSAGE(STATUS "LIBRARY_OUTPUT_PATH:     " ${LIBRARY_OUTPUT_PATH})
MESSAGE(STATUS "CMAKE_MODULE_PATH: " ${CMAKE_MODULE_PATH})
MESSAGE(STATUS "CMAKE_COMMAND: " ${CMAKE_COMMAND})
MESSAGE(STATUS "CMAKE_ROOT: " ${CMAKE_ROOT})
MESSAGE(STATUS "CMAKE_CURRENT_LIST_FILE: " ${CMAKE_CURRENT_LIST_FILE})
MESSAGE(STATUS "CMAKE_CURRENT_LIST_LINE: " ${CMAKE_CURRENT_LIST_LINE})
MESSAGE(STATUS "CMAKE_INCLUDE_PATH: " ${CMAKE_INCLUDE_PATH})
MESSAGE(STATUS "CMAKE_LIBRARY_PATH: " ${CMAKE_LIBRARY_PATH})
MESSAGE(STATUS "CMAKE_SYSTEM: " ${CMAKE_SYSTEM})
MESSAGE(STATUS "CMAKE_SYSTEM_NAME: " ${CMAKE_SYSTEM_NAME})
MESSAGE(STATUS "CMAKE_SYSTEM_VERSION: " ${CMAKE_SYSTEM_VERSION})
MESSAGE(STATUS "CMAKE_SYSTEM_PROCESSOR: " ${CMAKE_SYSTEM_PROCESSOR})
MESSAGE(STATUS "UNIX: " ${UNIX})
MESSAGE(STATUS "WIN32: " ${WIN32})
MESSAGE(STATUS "APPLE: " ${APPLE})
MESSAGE(STATUS "MINGW: " ${MINGW})
MESSAGE(STATUS "CYGWIN: " ${CYGWIN})
MESSAGE(STATUS "BORLAND: " ${BORLAND})
MESSAGE(STATUS "MSVC: " ${MSVC})
MESSAGE(STATUS "MSVC_IDE: " ${MSVC_IDE})
MESSAGE(STATUS "MSVC60: " ${MSVC60})
MESSAGE(STATUS "MSVC70: " ${MSVC70})
MESSAGE(STATUS "MSVC71: " ${MSVC71})
MESSAGE(STATUS "MSVC80: " ${MSVC80})
MESSAGE(STATUS "CMAKE_COMPILER_2005: " ${CMAKE_COMPILER_2005})
MESSAGE(STATUS "CMAKE_SKIP_RULE_DEPENDENCY: " ${CMAKE_SKIP_RULE_DEPENDENCY})
MESSAGE(STATUS "CMAKE_SKIP_INSTALL_ALL_DEPENDENCY: " ${CMAKE_SKIP_INSTALL_ALL_DEPENDENCY})
MESSAGE(STATUS "CMAKE_SKIP_RPATH: " ${CMAKE_SKIP_RPATH})
MESSAGE(STATUS "CMAKE_VERBOSE_MAKEFILE: " ${CMAKE_VERBOSE_MAKEFILE})
MESSAGE(STATUS "CMAKE_SUPPRESS_REGENERATION: " ${CMAKE_SUPPRESS_REGENERATION})
MESSAGE(STATUS "CMAKE_C_FLAGS: " ${CMAKE_C_FLAGS})
MESSAGE(STATUS "CMAKE_CXX_FLAGS: " ${CMAKE_CXX_FLAGS})
MESSAGE(STATUS "CMAKE_BUILD_TYPE: " ${CMAKE_BUILD_TYPE})
MESSAGE(STATUS "BUILD_SHARED_LIBS: " ${BUILD_SHARED_LIBS})
MESSAGE(STATUS "CMAKE_C_COMPILER: " ${CMAKE_C_COMPILER})
MESSAGE(STATUS "CMAKE_CXX_COMPILER: " ${CMAKE_CXX_COMPILER})
MESSAGE(STATUS "CMAKE_COMPILER_IS_GNUCC: " ${CMAKE_COMPILER_IS_GNUCC})
MESSAGE(STATUS "CMAKE_COMPILER_IS_GNUCXX : " ${CMAKE_COMPILER_IS_GNUCXX})
MESSAGE(STATUS "CMAKE_AR: " ${CMAKE_AR})
MESSAGE(STATUS "CMAKE_RANLIB: " ${CMAKE_RANLIB})
MESSAGE(STATUS "WITH_TESTS: " ${WITH_TESTS})
MESSAGE(STATUS "========================================")

# Compiler flags
ADD_DEFINITIONS("-fvisibility=default")         # mark all exported symbols as visible

# Warnings mode
#ADD_DEFINITIONS("-Werror")                      # Make all warnings into errors.

# Warning flags
ADD_DEFINITIONS("-Wall")                        # Generate all warnings
ADD_DEFINITIONS("-Wextra")                      # Generate even more extra warnings
ADD_DEFINITIONS("-pedantic")                    # Accept only pedantic code
#ADD_DEFINITIONS("-Weffc++")                     # Accept only effective C++ code
ADD_DEFINITIONS("-Wwrite-strings")              # Do not accept writing to contant string memory
ADD_DEFINITIONS("-Winit-self")                  # Do not accept initializing variable with itself
ADD_DEFINITIONS("-Wcast-align")                 # Do not accept misaligning with casting
ADD_DEFINITIONS("-Wcast-qual")                  # Do not accept removing qualifiers with casting
#ADD_DEFINITIONS("-Wold-style-cast")             # Do not accept old style casting
ADD_DEFINITIONS("-Wpointer-arith")              # Warn about void pointer arthimetic
ADD_DEFINITIONS("-Wstrict-aliasing")            # Ensure strict aliasing
ADD_DEFINITIONS("-Wuninitialized")              # Do not accept uninitialized variables
ADD_DEFINITIONS("-Wmissing-declarations")       # Warn about global and non-accesible functions
ADD_DEFINITIONS("-Woverloaded-virtual")         # Warn about incidental overiding non-virtual base methods
ADD_DEFINITIONS("-Wnon-virtual-dtor")           # Warn about non-virtual destructor
ADD_DEFINITIONS("-Wctor-dtor-privacy")          # Warn about useless and non-constructible classes
ADD_DEFINITIONS("-Wlong-long")                  # Do not allow using long long
#ADD_DEFINITIONS("-Wunreachable-code")           # Warn about unreachable code
ADD_DEFINITIONS("-Wfloat-equal")                # Do not accept comparing floating points with equal operator
ADD_DEFINITIONS("-Wabi")                        # Warn about possible ABI problems
ADD_DEFINITIONS("-Wswitch-enum")                # Check switch enumeration
ADD_DEFINITIONS("-Wformat=2")                   # Check printf formatting
ADD_DEFINITIONS("-Wundef")                      # Warn if an undefined identifier is evaluated in an @if directive.
ADD_DEFINITIONS("-Wshadow")                     # Warn whenever a local variable shadows another local variable, parameter or global variable or whenever a built-in function is shadowed
ADD_DEFINITIONS("-Wconversion")                 # Warn for implicit conversions that may alter a value
ADD_DEFINITIONS("-Wlogical-op")                 # Warn about suspicious uses of logical operators in expressions
#ADD_DEFINITIONS("-Waggregate-return")           # Warn if any functions that return structures or unions are defined or called.
ADD_DEFINITIONS("-Wmissing-field-initializers") # Warn if a structure's initializer has some fields missing.
ADD_DEFINITIONS("-Wredundant-decls")            # Warn if anything is declared more than once in the same scope, even in cases where multiple declaration is valid and changes nothing.
#ADD_DEFINITIONS("-Wmissing-include-dirs")       # Warn if a user-supplied include directory does not exist.
ADD_DEFINITIONS("-Wswitch-default")             # Warn whenever a switch statement does not have a default case.
ADD_DEFINITIONS("-Wsync-nand")                  # Warn when __sync_fetch_and_nand and __sync_nand_and_fetch built-in functions are used. These functions changed semantics in GCC 4.4.
ADD_DEFINITIONS("-Wunused")                     # All the above -Wunused options combined.
ADD_DEFINITIONS("-Wstrict-overflow=5")          # Also warn about cases where the compiler reduces the magnitude of a constant involved in a comparison.
#ADD_DEFINITIONS("-Wunsafe-loop-optimizations")  # Warn if the loop cannot be optimized because the compiler could not assume anything on the bounds of the loop indices. With -funsafe-loop-optimizations warn if the compiler made such assumptions.
#ADD_DEFINITIONS("-Wmissing-format-attribute")   # Warn about function pointers which might be candidates for format attributes.
#ADD_DEFINITIONS("-Wpadded")                     # Warn if padding is included in a structure, either to align an element of the structure or to align the whole structure.
#ADD_DEFINITIONS("-Winline")                     # Warn if a function can not be inlined and it was declared as inline.
ADD_DEFINITIONS("-Wdisabled-optimization")      # Warn if a requested optimization pass is disabled.
ADD_DEFINITIONS("-std=c++0x")

#
# Core library files
#
# Define all core library headers and sources. As detail files
# are usually templated and though recompiled in each file, we
# have to compile full source for each target.
#

# Set DPL 3rd party  include directory
SET(DPL_3RDPARTY_INCLUDE_DIR
    ${PROJECT_SOURCE_DIR}/3rdparty)

# Set names of binaries being created
SET(TARGET_DPL_EFL "lib${PROJECT_NAME}-efl")
SET(TARGET_DPL_DBUS_EFL "lib${PROJECT_NAME}-dbus-efl")
SET(TARGET_DPL_DB_EFL "lib${PROJECT_NAME}-db-efl")
SET(TARGET_DPL_EVENT_EFL "lib${PROJECT_NAME}-event-efl")
SET(TARGET_DPL_SOCKET_EFL "lib${PROJECT_NAME}-socket-efl")
SET(TARGET_DPL_RPC_EFL "lib${PROJECT_NAME}-rpc-efl")
SET(TARGET_DPL_TEST_ENGINE_EFL "lib${PROJECT_NAME}-test-efl")
SET(TARGET_DPL_LOG_EFL "lib${PROJECT_NAME}-log-efl")
SET(TARGET_WRT_DAO_RW_LIB "dpl-wrt-dao-rw")
SET(TARGET_WRT_DAO_RO_LIB "dpl-wrt-dao-ro")
SET(TARGET_AUTO_SAVE_DAO_LIB "wrt-commons-auto-save-dao")
SET(TARGET_AUTO_SAVE_DAO_RW_LIB "wrt-commons-auto-save-dao-rw")
SET(TARGET_AUTO_SAVE_DAO_RO_LIB "wrt-commons-auto-save-dao-ro")
SET(TARGET_CUSTOM_HANDLER_DAO_RW_LIB "wrt-commons-custom-handler-dao-rw")
SET(TARGET_CUSTOM_HANDLER_DAO_RO_LIB "wrt-commons-custom-handler-dao-ro")
SET(TARGET_SECURITY_ORIGIN_DAO_LIB "wrt-commons-security-origin-dao")
SET(TARGET_CERTIFICATE_DAO_LIB "wrt-commons-certificate-dao")
SET(TARGET_DPL_UTILS_EFL "lib${PROJECT_NAME}-utils-efl")
SET(TARGET_DPL_ENCRYPTION "lib${PROJECT_NAME}-encryption")

macro(configure_and_install_pkg PKG_FILE)
    CONFIGURE_FILE(${PKG_FILE}.in ${PKG_FILE} @ONLY)
    INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PKG_FILE} DESTINATION lib/pkgconfig)
endmacro(configure_and_install_pkg)

ADD_SUBDIRECTORY(modules)

ADD_SUBDIRECTORY(build)
ADD_SUBDIRECTORY(etc)

IF(WITH_TESTS)
  ADD_SUBDIRECTORY(tests)
ENDIF(WITH_TESTS)
