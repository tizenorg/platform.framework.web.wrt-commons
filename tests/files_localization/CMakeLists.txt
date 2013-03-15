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

SET(TARGET_LOC "wrt-commons-tests-loc")

SET(LOC_TESTS_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/test_localization.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_suite01.cpp
)

WRT_TEST_ADD_INTERNAL_DEPENDENCIES(${TARGET_LOC} ${TARGET_WRT_DAO_RW_LIB} ${TARGET_CUSTOM_HANDLER_DAO_RW_LIB})
WRT_TEST_BUILD(${TARGET_LOC} ${LOC_TESTS_SOURCES})
WRT_TEST_INSTALL(${TARGET_LOC})

ADD_SUBDIRECTORY(files)

INSTALL(PROGRAMS "${CMAKE_CURRENT_SOURCE_DIR}/wrt_db_localization_prepare.sh"
        DESTINATION bin)
