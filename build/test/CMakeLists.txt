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
# @author      Lukasz Marek (l.marek@samsung.com)
# @version     1.0
# @brief
#

INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(SYS_EFL_TEST_ENGINE
  appcore-efl
  libxml-2.0
  REQUIRED)


# Add core include directories
INCLUDE_DIRECTORIES(
    ${DPL_LOG_INCLUDE_DIR}
    ${DPL_CORE_INCLUDE_DIR}
    ${DPL_TEST_ENGINE_INCLUDE_DIR}
    ${DPL_UTILS_INCLUDE_DIR}
)

INCLUDE_DIRECTORIES(SYSTEM ${SYS_EFL_TEST_ENGINE_INCLUDE_DIRS})

LINK_DIRECTORIES(
    ${SYS_EFL_TEST_ENGINE_LIBRARY_DIRS}
)

# Base EFL based DPL library
SET(DPL_EFL_TEST_ENGINE_LIBRARY "${PROJECT_NAME}-test-efl")

# Build shared library

ADD_LIBRARY(${TARGET_DPL_TEST_ENGINE_EFL} SHARED  ${DPL_TEST_ENGINE_SOURCES})

TARGET_LINK_LIBRARIES(${TARGET_DPL_TEST_ENGINE_EFL}
    ${TARGET_DPL_EFL}
    ${TARGET_DPL_UTILS_EFL}
)

# Target library properties
SET_TARGET_PROPERTIES(${TARGET_DPL_TEST_ENGINE_EFL} PROPERTIES
    SOVERSION ${API_VERSION} 
 VERSION ${VERSION}
    CLEAN_DIRECT_OUTPUT 1
    OUTPUT_NAME ${DPL_EFL_TEST_ENGINE_LIBRARY})

# Install libraries
INSTALL(TARGETS ${TARGET_DPL_TEST_ENGINE_EFL}
        DESTINATION ${LIB_INSTALL_DIR})

# Install detail headers
INSTALL(FILES ${DPL_TEST_ENGINE_HEADERS}
        DESTINATION include/dpl-efl/dpl/test)

# Install pkgconfig script
configure_and_install_pkg(dpl-test-efl.pc)
