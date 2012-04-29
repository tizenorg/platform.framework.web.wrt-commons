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
PKG_CHECK_MODULES(SYS_EFL_UTILS
    dlog
    libiri
    appcore-efl
    libidn
    REQUIRED
)

# Add core include directories
INCLUDE_DIRECTORIES(
    ${DPL_LOG_INCLUDE_DIR}
    ${DPL_CORE_INCLUDE_DIR}
    ${DPL_DB_INCLUDE_DIR}
    ${SYS_EFL_UTILS_INCLUDE_DIRS}
    ${DPL_UTILS_INCLUDE_DIR}
    ${DPL_LOCALIZATION_INCLUDE_DIR}
)

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/modules/widget_dao/include)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/modules/vcore/src/vcore)

LINK_DIRECTORIES(
    ${SYS_EFL_UTILS_LIBRARY_DIRS}
)

# Base EFL based DPL library
SET(DPL_EFL_UTILS_LIBRARY "${PROJECT_NAME}-utils-efl")

# Build shared library

ADD_LIBRARY(${TARGET_DPL_UTILS_EFL} SHARED
    ${DPL_UTILS_SOURCES}
    ${DPL_LOCALIZATION_SOURCES}
)

TARGET_LINK_LIBRARIES(${TARGET_DPL_UTILS_EFL}
    ${SYS_EFL_UTILS_LIBRARIES}
    ${TARGET_DPL_EFL}
    ${TARGET_WRT_DAO_RW_LIB}
    ${SYS_EFL_DB_LIBRARIES}
)

# Target library properties
SET_TARGET_PROPERTIES(${TARGET_DPL_UTILS_EFL} PROPERTIES
    SOVERSION ${API_VERSION} 
 VERSION ${VERSION}
    CLEAN_DIRECT_OUTPUT 1
    OUTPUT_NAME ${DPL_EFL_UTILS_LIBRARY})

# Install libraries
INSTALL(TARGETS ${TARGET_DPL_UTILS_EFL}
        DESTINATION lib)

# Install detail headers
INSTALL(FILES ${DPL_UTILS_HEADERS}
        DESTINATION include/dpl-efl/dpl/utils)

INSTALL(FILES ${DPL_LOCALIZATION_HEADERS}
        DESTINATION include/dpl-efl/dpl/localization)

# Install pkgconfig script
configure_and_install_pkg(dpl-utils-efl.pc)
