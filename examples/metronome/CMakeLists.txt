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
PKG_CHECK_MODULES(METRONOME_SYS dpl-efl REQUIRED)

SET(METRONOME_CLIENT_SOURCES
    metronome_client.cpp)

SET(METRONOME_SERVER_SOURCES
    metronome_server.cpp)

ADD_DEFINITIONS("-D_DEBUG")

INCLUDE_DIRECTORIES(${METRONOME_SYS_INCLUDE_DIRS})
LINK_DIRECTORIES(${METRONOME_SYS_LIBRARY_DIRS})

ADD_EXECUTABLE(metronome_client ${METRONOME_CLIENT_SOURCES})
TARGET_LINK_LIBRARIES(metronome_client ${METRONOME_SYS_LIBRARIES})

ADD_EXECUTABLE(metronome_server ${METRONOME_SERVER_SOURCES})
TARGET_LINK_LIBRARIES(metronome_server ${METRONOME_SYS_LIBRARIES})
