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

#
# Test files
#
# Define all DPL tests sources.
# Runner is responsible for runnint it all and
# generating proper output files
#

SET(TARGET_NAME "wrt-commons-tests-core")

# Set DPL tests sources
SET(DPL_TESTS_CORE_SOURCES
    ${TESTS_DIR}/core/main.cpp
    ${TESTS_DIR}/core/test_address.cpp
    ${TESTS_DIR}/core/test_binary_queue.cpp
    ${TESTS_DIR}/core/test_foreach.cpp
    ${TESTS_DIR}/core/test_fast_delegate.cpp
    ${TESTS_DIR}/core/test_log_unhandled_exception.cpp
    ${TESTS_DIR}/core/test_once.cpp
    ${TESTS_DIR}/core/test_serialization.cpp
    ${TESTS_DIR}/core/test_scoped_array.cpp
    ${TESTS_DIR}/core/test_scoped_close.cpp
    ${TESTS_DIR}/core/test_scoped_fclose.cpp
    ${TESTS_DIR}/core/test_scoped_free.cpp
    ${TESTS_DIR}/core/test_scoped_ptr.cpp
    ${TESTS_DIR}/core/test_semaphore.cpp
    ${TESTS_DIR}/core/test_shared_ptr.cpp
    ${TESTS_DIR}/core/test_string.cpp
    ${TESTS_DIR}/core/test_thread.cpp
    ${TESTS_DIR}/core/test_type_list.cpp
    ${TESTS_DIR}/core/test_zip_input.cpp
)

WRT_TEST_ADD_INTERNAL_DEPENDENCIES(${TARGET_NAME} ${TARGET_DPL_EFL})
WRT_TEST_BUILD(${TARGET_NAME} ${DPL_TESTS_CORE_SOURCES})
WRT_TEST_INSTALL(${TARGET_NAME})

INSTALL(FILES
        ${TESTS_DIR}/core/data/sample.zip
        DESTINATION /opt/share/wrt/wrt-commons/tests/core
    )
