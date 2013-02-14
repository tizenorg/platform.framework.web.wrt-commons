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
SET(DPL_TESTS_DB_SOURCES
    ${TESTS_DPL_DIR}/db/main.cpp
    ${TESTS_DPL_DIR}/db/test_orm.cpp
    ${TESTS_DPL_DIR}/db/test_sql_connection.cpp
)

ADD_SUBDIRECTORY(orm)

#include subdirectory
WRT_INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/orm)
WRT_TEST_ADD_INTERNAL_DEPENDENCIES(${TARGET_NAME} ${TARGET_DPL_DB_EFL})
WRT_TEST_BUILD(${TARGET_NAME} ${DPL_TESTS_DB_SOURCES})
WRT_TEST_INSTALL(${TARGET_NAME})

INSTALL(FILES
        ${TESTS_DPL_DIR}/db/orm/dpl_orm_test.db
        DESTINATION /opt/share/wrt/wrt-commons/tests/db
)
