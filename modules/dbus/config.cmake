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
#
# @file        config.cmake
# @author      Lukasz Marek (l.marek@samsung.com)
# @version     1.0
# @brief
#

SET(DPL_DBUS_SOURCES
    ${PROJECT_SOURCE_DIR}/modules/dbus/src/connection.cpp
    ${PROJECT_SOURCE_DIR}/modules/dbus/src/dispatcher.cpp
    ${PROJECT_SOURCE_DIR}/modules/dbus/src/interface.cpp
    ${PROJECT_SOURCE_DIR}/modules/dbus/src/object.cpp
    ${PROJECT_SOURCE_DIR}/modules/dbus/src/object_proxy.cpp
    ${PROJECT_SOURCE_DIR}/modules/dbus/src/server.cpp
    PARENT_SCOPE
)


SET(DPL_DBUS_HEADERS
    ${PROJECT_SOURCE_DIR}/modules/dbus/include/dpl/dbus/connection.h
    ${PROJECT_SOURCE_DIR}/modules/dbus/include/dpl/dbus/dbus_client.h
    ${PROJECT_SOURCE_DIR}/modules/dbus/include/dpl/dbus/dbus_deserialization.h
    ${PROJECT_SOURCE_DIR}/modules/dbus/include/dpl/dbus/dbus_interface_dispatcher.h
    ${PROJECT_SOURCE_DIR}/modules/dbus/include/dpl/dbus/dbus_serialization.h
    ${PROJECT_SOURCE_DIR}/modules/dbus/include/dpl/dbus/dbus_server_deserialization.h
    ${PROJECT_SOURCE_DIR}/modules/dbus/include/dpl/dbus/dbus_server_serialization.h
    ${PROJECT_SOURCE_DIR}/modules/dbus/include/dpl/dbus/dbus_signature.h
    ${PROJECT_SOURCE_DIR}/modules/dbus/include/dpl/dbus/dispatcher.h
    ${PROJECT_SOURCE_DIR}/modules/dbus/include/dpl/dbus/exception.h
    ${PROJECT_SOURCE_DIR}/modules/dbus/include/dpl/dbus/interface.h
    ${PROJECT_SOURCE_DIR}/modules/dbus/include/dpl/dbus/method_proxy.h
    ${PROJECT_SOURCE_DIR}/modules/dbus/include/dpl/dbus/object.h
    ${PROJECT_SOURCE_DIR}/modules/dbus/include/dpl/dbus/object_proxy.h
    ${PROJECT_SOURCE_DIR}/modules/dbus/include/dpl/dbus/server.h
    PARENT_SCOPE
)

SET(DPL_DBUS_INCLUDE_DIR
    ${PROJECT_SOURCE_DIR}/modules/dbus/include
    PARENT_SCOPE
)
