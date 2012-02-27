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

#
# Test files
#
# Define all DPL tests sources.
# Runner is responsible for runnint it all and
# generating proper output files
#

SET(TARGET_NAME "dpl-tests-db")

# Set DPL tests sources
SET(DPL_TESTS_SOURCES
    ${PROJECT_SOURCE_DIR}/tests/db/main.cpp
    ${PROJECT_SOURCE_DIR}/tests/db/test_orm.cpp
    ${PROJECT_SOURCE_DIR}/tests/db/test_sql_connection.cpp
)

ADD_SUBDIRECTORY(orm)

INCLUDE_DIRECTORIES(
    ${SYS_EFL_INCLUDE_DIRS}
    ${DPL_TEST_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/orm
)

LINK_DIRECTORIES(${SYS_EFL_LIBRARY_DIRS})

ADD_EXECUTABLE(${TARGET_NAME} ${DPL_TESTS_SOURCES})

TARGET_LINK_LIBRARIES(
    ${TARGET_NAME}
    ${TARGET_DPL_EFL}
    ${TARGET_DPL_DB_EFL}
    ${TARGET_DPL_TEST_ENGINE_EFL}
)

SET_TARGET_PROPERTIES(${TARGET_NAME} PROPERTIES
    BUILD_WITH_INSTALL_RPATH ON
    INSTALL_RPATH_USE_LINK_PATH ON
)

INSTALL(TARGETS ${TARGET_NAME}
        DESTINATION bin
        PERMISSIONS OWNER_READ
                    OWNER_WRITE
                    OWNER_EXECUTE
                    GROUP_READ
                    GROUP_EXECUTE
                    WORLD_READ
                    WORLD_EXECUTE
)

INSTALL(FILES
        ${PROJECT_SOURCE_DIR}/tests/db/orm/dpl_orm_test.db
        DESTINATION /opt/apps/wrt/wrt-commons/tests/db
)