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
INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(TCPSOCK_SYS dpl-efl REQUIRED)

SET(TCPSOCK_SOURCES
    tcpsock.cpp)

ADD_DEFINITIONS("-D_DEBUG")

INCLUDE_DIRECTORIES(${TCPSOCK_SYS_INCLUDE_DIRS})
LINK_DIRECTORIES(${TCPSOCK_SYS_LIBRARY_DIRS})

ADD_EXECUTABLE(tcpsock ${TCPSOCK_SOURCES})
TARGET_LINK_LIBRARIES(tcpsock ${TCPSOCK_SYS_LIBRARIES})
