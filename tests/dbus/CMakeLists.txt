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

INCLUDE(FindPkgConfig)

SET(TARGET_DBUS_TESTS "dpl-tests-dbus")
SET(TARGET_DBUS_TEST_SERVICE "dpl-dbus-test-service")

PKG_CHECK_MODULES(DBUS_PKG
  ecore
  appcore-efl
  gio-2.0
  gobject-2.0
  REQUIRED
)

SET(DBUS_TESTS_SRCS
    ${PROJECT_SOURCE_DIR}/tests/dbus/main.cpp
    ${PROJECT_SOURCE_DIR}/tests/dbus/test_cases.cpp
    ${PROJECT_SOURCE_DIR}/tests/dbus/dbus_test.cpp
    ${PROJECT_SOURCE_DIR}/tests/dbus/loop_control.cpp
)

SET(DBUS_TEST_SERVICE_SRCS
    ${PROJECT_SOURCE_DIR}/tests/dbus/test_service.cpp
    ${PROJECT_SOURCE_DIR}/tests/dbus/loop_control.cpp
)

INCLUDE_DIRECTORIES(
    ${DBUS_PKG_INCLUDE_DIRS}
    ${DPL_TEST_INCLUDE_DIR}
)

LINK_DIRECTORIES(${DBUS_PKG_LIBRARY_DIRS})

ADD_EXECUTABLE(${TARGET_DBUS_TESTS}
               ${DBUS_TESTS_SRCS}
)

TARGET_LINK_LIBRARIES(${TARGET_DBUS_TESTS}
                      ${DBUS_PKG_LIBRARIES}
                      ${TARGET_DPL_EFL}
                      ${TARGET_DPL_DBUS_EFL}
                      ${TARGET_DPL_TEST_ENGINE_EFL}
)

SET_TARGET_PROPERTIES(${TARGET_DBUS_TESTS} PROPERTIES
    BUILD_WITH_INSTALL_RPATH ON
    INSTALL_RPATH_USE_LINK_PATH ON
)

ADD_EXECUTABLE(${TARGET_DBUS_TEST_SERVICE}
               ${DBUS_TEST_SERVICE_SRCS}
)

TARGET_LINK_LIBRARIES(${TARGET_DBUS_TEST_SERVICE}
                      ${DBUS_PKG_LIBRARIES}
                      ${TARGET_DPL_EFL}
                      ${TARGET_DPL_DBUS_EFL}
)

SET_TARGET_PROPERTIES(${TARGET_DBUS_TEST_SERVICE} PROPERTIES
    BUILD_WITH_INSTALL_RPATH ON
    INSTALL_RPATH_USE_LINK_PATH ON
)

INSTALL(TARGETS ${TARGET_DBUS_TESTS} ${TARGET_DBUS_TEST_SERVICE}
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
        ${PROJECT_SOURCE_DIR}/tests/dbus/data/org.tizen.DBusTestService.service
        DESTINATION /usr/share/dbus-1/services
)
