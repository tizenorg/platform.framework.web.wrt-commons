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
# @author      Soyoung Kim (sy037.kim@samsung.com)
# @version     1.0
# @brief
#

# Check required modules
INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(SYS_ENCRYPTION
    dlog
    openssl
    REQUIRED
)

# Add core include directories
INCLUDE_DIRECTORIES(
    ${DPL_LOG_INCLUDE_DIR}
    ${DPL_CORE_INCLUDE_DIR}
    ${DPL_DB_INCLUDE_DIR}
    ${DPL_ENCRYPTION_INCLUDE_DIR}
)

INCLUDE_DIRECTORIES(SYSTEM ${SYS_ENCRYPTION_INCLUDE_DIRS})

LINK_DIRECTORIES(
    ${SYS_ENCRYPTION_LIBRARY_DIRS}
)

# Base EFL based DPL library
SET(DPL_ENCRYPTION_LIBRARY "${PROJECT_NAME}-encryption")

# Build shared library

ADD_LIBRARY(${TARGET_DPL_ENCRYPTION} SHARED
    ${DPL_ENCRYPTION_SOURCES}
)

TARGET_LINK_LIBRARIES(${TARGET_DPL_ENCRYPTION}
    ${SYS_ENCRYPTION_LIBRARIES}
    ${TARGET_DPL_EFL}
)

# Target library properties
SET_TARGET_PROPERTIES(${TARGET_DPL_ENCRYPTION} PROPERTIES
    SOVERSION ${API_VERSION}
 VERSION ${VERSION}
    CLEAN_DIRECT_OUTPUT 1
    OUTPUT_NAME ${DPL_ENCRYPTION_LIBRARY})

# Install libraries
INSTALL(TARGETS ${TARGET_DPL_ENCRYPTION}
        DESTINATION lib)

# Install detail headers

INSTALL(FILES ${DPL_ENCRYPTION_HEADERS}
        DESTINATION include/dpl-efl/dpl/encryption)

# Install pkgconfig script
configure_and_install_pkg(dpl-encryption.pc)
