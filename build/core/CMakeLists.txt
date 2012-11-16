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
PKG_CHECK_MODULES(SYS_EFL
  ecore
  appcore-efl
  openssl
  dlog
  vconf
  libpcrecpp
  icu-uc
  REQUIRED)

# Add core include directories
INCLUDE_DIRECTORIES(
    ${DPL_LOG_INCLUDE_DIR}
    ${DPL_CORE_INCLUDE_DIR}
    ${DPL_3RDPARTY_INCLUDE_DIR}
)

INCLUDE_DIRECTORIES(SYSTEM ${SYS_EFL_INCLUDE_DIRS})

LINK_DIRECTORIES(${SYS_EFL_LIBRARY_DIRS})

# Base EFL based DPL library
SET(DPL_EFL_LIBRARY "${PROJECT_NAME}-efl")

# Build shared library
ADD_LIBRARY(${TARGET_DPL_EFL} SHARED
    ${DPL_CORE_SOURCES}
    ${DPL_LOG_SOURCES}
)

TARGET_LINK_LIBRARIES(${TARGET_DPL_EFL} ${SYS_EFL_LIBRARIES} "-lrt")

# Link with 3rd party libraries
LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/3rdparty)

TARGET_LINK_LIBRARIES(${TARGET_DPL_EFL} lib3rdparty)

# Target library properties
SET_TARGET_PROPERTIES(${TARGET_DPL_EFL} PROPERTIES
    SOVERSION ${API_VERSION} 
 VERSION ${VERSION}
    CLEAN_DIRECT_OUTPUT 1
    OUTPUT_NAME ${DPL_EFL_LIBRARY})

# Install libraries
INSTALL(TARGETS ${TARGET_DPL_EFL}
        DESTINATION lib)

# Install detail headers
INSTALL(FILES ${DPL_CORE_EFL_DETAIL_HEADERS}
        DESTINATION include/dpl-efl/dpl)

# Install core headers
INSTALL(FILES ${DPL_CORE_HEADERS}
        DESTINATION include/dpl-efl/dpl)

# Install log headers
INSTALL(FILES ${DPL_LOG_HEADERS}
        DESTINATION include/dpl-efl/dpl/log)

# Install 3rdparty headers
INSTALL(FILES ${DPL_3RDPARTY_HEADERS}
        DESTINATION include/dpl-efl/dpl/3rdparty/fastdelegate)

# Install pkgconfig script
configure_and_install_pkg(dpl-efl.pc)
