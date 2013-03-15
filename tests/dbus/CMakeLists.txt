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

INCLUDE(FindPkgConfig)

PKG_CHECK_MODULES(DEPENDENCIES gthread-2.0 REQUIRED)

SET(TARGET_DBUS_TESTS "wrt-commons-tests-dbus")
SET(TARGET_DBUS_TEST_SERVICE "wrt-commons-tests-dbus-test-service")

SET(DBUS_TESTS_SRCS
    ${TESTS_DIR}/dbus/main.cpp
    ${TESTS_DIR}/dbus/test_cases.cpp
    ${TESTS_DIR}/dbus/dbus_test.cpp
    ${TESTS_COMMON_DIR}/src/loop_control.cpp
)

SET(DBUS_TEST_SERVICE_SRCS
    ${TESTS_DIR}/dbus/test_service.cpp
    ${TESTS_COMMON_DIR}/src/loop_control.cpp
)

WRT_TEST_ADD_INTERNAL_DEPENDENCIES(${TARGET_DBUS_TESTS} ${TARGET_DPL_DBUS_EFL})
WRT_TEST_INCLUDE_DIRECTORIES(${TARGET_DBUS_TESTS}
    ${TESTS_COMMON_DIR}/include
    ${DEPENDENCIES_INCLUDE_DIRS}
)
WRT_TEST_LINK_DIRECTORIES(${TARGET_DBUS_TESTS} ${DEPENDENCIES_LIBRARY_DIRS})
WRT_TEST_TARGET_LINK_LIBRARIES(${TARGET_DBUS_TESTS} ${DEPENDENCIES_LIBRARIES})
WRT_TEST_BUILD(${TARGET_DBUS_TESTS} ${DBUS_TESTS_SRCS})
WRT_TEST_INSTALL(${TARGET_DBUS_TESTS})

WRT_TEST_ADD_INTERNAL_DEPENDENCIES(${TARGET_DBUS_TEST_SERVICE} ${TARGET_DPL_DBUS_EFL})
WRT_TEST_INCLUDE_DIRECTORIES(${TARGET_DBUS_TEST_SERVICE}
    ${TESTS_COMMON_DIR}/include
    ${DEPENDENCIES_INCLUDE_DIRS}
)
WRT_TEST_LINK_DIRECTORIES(${TARGET_DBUS_TEST_SERVICE} ${DEPENDENCIES_LIBRARY_DIRS})
WRT_TEST_TARGET_LINK_LIBRARIES(${TARGET_DBUS_TEST_SERVICE} ${DEPENDENCIES_LIBRARIES})
WRT_TEST_BUILD(${TARGET_DBUS_TEST_SERVICE} ${DBUS_TEST_SERVICE_SRCS})
WRT_TEST_INSTALL(${TARGET_DBUS_TEST_SERVICE})

INSTALL(FILES
        ${TESTS_DIR}/dbus/data/org.tizen.DBusTestService.service
        DESTINATION /usr/share/dbus-1/services
)
