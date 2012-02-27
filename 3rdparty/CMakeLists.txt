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
# @file        CMakeLists.txt
# @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
# @version     1.0
# @brief
#

# Check required modules
INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(SYS_3RDPARTY REQUIRED)

# Set 3rd party sources
SET(DPL_3RDPARTY_SOURCES
    # Minizip
    ${PROJECT_SOURCE_DIR}/3rdparty/minizip/ioapi.c
#   ${PROJECT_SOURCE_DIR}/3rdparty/minizip/iowin32.c
    ${PROJECT_SOURCE_DIR}/3rdparty/minizip/miniunz.c
    ${PROJECT_SOURCE_DIR}/3rdparty/minizip/minizip.c
    ${PROJECT_SOURCE_DIR}/3rdparty/minizip/mztools.c
    ${PROJECT_SOURCE_DIR}/3rdparty/minizip/unzip.c
    ${PROJECT_SOURCE_DIR}/3rdparty/minizip/zip.c)

# Set 3rd party headers
SET(DPL_3RDPARTY_HEADERS
    # Minizip
    ${PROJECT_SOURCE_DIR}/3rdparty/minizip/crypt.h
    ${PROJECT_SOURCE_DIR}/3rdparty/minizip/ioapi.h
#   ${PROJECT_SOURCE_DIR}/3rdparty/minizip/iowin32.h
    ${PROJECT_SOURCE_DIR}/3rdparty/minizip/mztools.h
    ${PROJECT_SOURCE_DIR}/3rdparty/minizip/unzip.h
    ${PROJECT_SOURCE_DIR}/3rdparty/minizip/zip.h)

# Set 3rd party include directory
SET(DPL_3RDPARTY_INCLUDE_DIR
    ${PROJECT_SOURCE_DIR}/3rdparty)

# Set name of binaries being created
SET(TARGET_3RDPARTY "lib3rdparty")

# Add 3rd party include directories
INCLUDE_DIRECTORIES(${DPL_3RDPARTY_INCLUDE_DIR})

# Add system include files
INCLUDE_DIRECTORIES(${SYS_3RDPARTY_INCLUDE_DIRS})
LINK_DIRECTORIES(${SYS_3RDPARTY_LIBRARY_DIRS})

# Build shared library
ADD_LIBRARY(${TARGET_3RDPARTY} STATIC ${DPL_3RDPARTY_SOURCES})
TARGET_LINK_LIBRARIES(${TARGET_3RDPARTY} ${SYS_3RDPARTY_LIBRARIES} z)

