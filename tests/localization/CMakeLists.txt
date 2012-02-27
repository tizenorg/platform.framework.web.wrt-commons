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
# @author      Bartlomiej Grzelewski (b.grzelewski@samsung.com)
# @version     1.0
# @brief
#

#
# Test files
#
# Define all DPL tests sources.
# Runner is responsible for runnint it all and
# generating proper output files
#

SET(TARGET_LOC "dpl-tests-loc")

SET(MAIN_DIR ${PROJECT_SOURCE_DIR}/tests/localization)

SET(LOC_TESTS_SOURCES
    ${MAIN_DIR}/test_localization.cpp
    ${MAIN_DIR}/test_suite01.cpp
    ${MAIN_DIR}/mockup_src/widget_dao.cpp
    ${PROJECT_SOURCE_DIR}/modules/localization/src/localization_utils.cpp
    ${PROJECT_SOURCE_DIR}/modules/localization/src/w3c_file_localization.cpp
)

INCLUDE_DIRECTORIES(
    ${SYS_EFL_INCLUDE_DIRS}
    ${DPL_TEST_INCLUDE_DIR}
    ${MAIN_DIR}/mockup_include
    ${PROJECT_SOURCE_DIR}/modules/localization/include
)

LINK_DIRECTORIES(${SYS_EFL_LIBRARY_DIRS})

ADD_EXECUTABLE(${TARGET_LOC} ${LOC_TESTS_SOURCES})

TARGET_LINK_LIBRARIES(
    ${TARGET_LOC}
    ${SYS_EFL_LIBRARIES}
    ${TARGET_DPL_EFL}
    ${TARGET_DPL_TEST_ENGINE_EFL}
)

SET_TARGET_PROPERTIES(${TARGET_LOC} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/
    BUILD_WITH_INSTALL_RPATH ON
    INSTALL_RPATH_USE_LINK_PATH ON
)

INSTALL(TARGETS ${TARGET_LOC}
    DESTINATION bin
    PERMISSIONS OWNER_READ
                OWNER_WRITE
                OWNER_EXECUTE
                GROUP_READ
                GROUP_EXECUTE
                WORLD_READ
                WORLD_EXECUTE
    )

ADD_SUBDIRECTORY(files)
