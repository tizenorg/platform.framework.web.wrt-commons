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
# @author      Zbigniew Kostrzewa (z.kostrzewa@samsung.com)
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

SET(TARGET_NAME "wrt-commons-tests-i18n")

# Set DPL tests sources
SET(DPL_TESTS_I18N_SOURCES
    ${TESTS_DIR}/i18n/main.cpp
    ${TESTS_DIR}/i18n/test_i18n_dao_read_only.cpp
)

#include subdirectory
WRT_TEST_ADD_INTERNAL_DEPENDENCIES(${TARGET_NAME} ${TARGET_I18N_DAO_RO_LIB})
WRT_TEST_BUILD(${TARGET_NAME} ${DPL_TESTS_I18N_SOURCES})
WRT_TEST_INSTALL(${TARGET_NAME})
