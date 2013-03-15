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
# @author      Krzysztof Jackiewicz (k.jackiewicz@samsung.com)
# @version     1.0
# @brief
#

SET(TARGET_NAME "wrt-commons-tests-utils")

# Set DPL tests sources
SET(DPL_TESTS_UTIL_SOURCES
    ${TESTS_DIR}/utils/main.cpp
    ${TESTS_DIR}/utils/widget_version.cpp
    ${TESTS_DIR}/utils/bash_utils.cpp
    ${TESTS_DIR}/utils/wrt_utility.cpp
    ${TESTS_DIR}/utils/path_tests.cpp
)

#WRT_TEST_ADD_INTERNAL_DEPENDENCIES(${TARGET_NAME} ${TARGET_DPL_UTILS_EFL})
WRT_TEST_BUILD(${TARGET_NAME} ${DPL_TESTS_UTIL_SOURCES})
WRT_TEST_INSTALL(${TARGET_NAME})
