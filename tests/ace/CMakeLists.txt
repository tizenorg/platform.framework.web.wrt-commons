#
#Copyright (c) 2011 Samsung Electronics Co., Ltd All Rights Reserved
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.
#
# @file        CMakeLists.txt
# @author      Bartlomiej Grzelewski (b.grzelewski@samsung.com)
# @version     0.1
# @brief
#
INCLUDE(FindPkgConfig)
SET(TARGET_ACE_TEST "dpl-tests-ace")

PKG_CHECK_MODULES(ACE_TEST_DEP
    libxml-2.0
    gobject-2.0
    REQUIRED
    )

# Set DPL tests sources
SET(ACE_TESTS_SOURCES
    ${PROJECT_SOURCE_DIR}/tests/ace/ace_tests.cpp
    ${PROJECT_SOURCE_DIR}/tests/ace/loop_control.cpp
    ${PROJECT_SOURCE_DIR}/tests/ace/TestSuite01.cpp
    ${PROJECT_SOURCE_DIR}/tests/ace/TestSuite02.cpp
    ${PROJECT_SOURCE_DIR}/tests/ace/TestSuite03.cpp
    ${PROJECT_SOURCE_DIR}/tests/ace/TestSuite04.cpp
    ${PROJECT_SOURCE_DIR}/tests/ace/TestSuite05.cpp
    ${PROJECT_SOURCE_DIR}/tests/ace/TestSuite06.cpp
    ${PROJECT_SOURCE_DIR}/tests/ace/TestSuite07.cpp
    ${PROJECT_SOURCE_DIR}/tests/ace/Interfaces.cpp
    ${PROJECT_SOURCE_DIR}/tests/ace/PEPSingleton.cpp
    ${PROJECT_SOURCE_DIR}/tests/ace/AttributeSetter.cpp
    )

INCLUDE_DIRECTORIES(
    ${SYS_EFL_INCLUDE_DIRS}
    ${DPL_TEST_INCLUDE_DIR}
    ${PROJECT_SOURCE_DIR}/modules/ace/include
    ${PROJECT_SOURCE_DIR}/tests/ace
    ${ACE_TEST_DEP_INCLUDE_DIRS}
    )

ADD_EXECUTABLE(${TARGET_ACE_TEST} ${ACE_TESTS_SOURCES})

TARGET_LINK_LIBRARIES(${TARGET_ACE_TEST}
    ${SYS_EFL_LIBRARIES}
    ${TARGET_DPL_EFL}
    ${TARGET_DPL_TEST_ENGINE_EFL}
    ${TARGET_ACE_LIB}
    ${ACE_TEST_DEP_LIBRARIES}
    )

INSTALL(TARGETS ${TARGET_ACE_TEST}
    DESTINATION bin
    PERMISSIONS OWNER_READ
                OWNER_WRITE
                OWNER_EXECUTE
                GROUP_READ
                GROUP_EXECUTE
                WORLD_READ
                WORLD_EXECUTE
    )

ADD_SUBDIRECTORY(test-configuration)
