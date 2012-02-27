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

SET(DPL_RPC_SOURCES
    ${PROJECT_SOURCE_DIR}/modules/rpc/src/abstract_rpc_connection.cpp
    ${PROJECT_SOURCE_DIR}/modules/rpc/src/abstract_rpc_connector.cpp
    ${PROJECT_SOURCE_DIR}/modules/rpc/src/generic_rpc_connection.cpp
    ${PROJECT_SOURCE_DIR}/modules/rpc/src/generic_socket_rpc_client.cpp
    ${PROJECT_SOURCE_DIR}/modules/rpc/src/generic_socket_rpc_connection.cpp
    ${PROJECT_SOURCE_DIR}/modules/rpc/src/generic_socket_rpc_server.cpp
    ${PROJECT_SOURCE_DIR}/modules/rpc/src/unix_socket_rpc_client.cpp
    ${PROJECT_SOURCE_DIR}/modules/rpc/src/unix_socket_rpc_connection.cpp
    ${PROJECT_SOURCE_DIR}/modules/rpc/src/unix_socket_rpc_server.cpp
    PARENT_SCOPE
)

SET(DPL_RPC_HEADERS
    ${PROJECT_SOURCE_DIR}/modules/rpc/include/dpl/rpc/abstract_rpc_connection.h
    ${PROJECT_SOURCE_DIR}/modules/rpc/include/dpl/rpc/abstract_rpc_connector.h
    ${PROJECT_SOURCE_DIR}/modules/rpc/include/dpl/rpc/generic_rpc_connection.h
    ${PROJECT_SOURCE_DIR}/modules/rpc/include/dpl/rpc/generic_socket_rpc_client.h
    ${PROJECT_SOURCE_DIR}/modules/rpc/include/dpl/rpc/generic_socket_rpc_connection.h
    ${PROJECT_SOURCE_DIR}/modules/rpc/include/dpl/rpc/generic_socket_rpc_server.h
    ${PROJECT_SOURCE_DIR}/modules/rpc/include/dpl/rpc/rpc_function.h
    ${PROJECT_SOURCE_DIR}/modules/rpc/include/dpl/rpc/unix_socket_rpc_client.h
    ${PROJECT_SOURCE_DIR}/modules/rpc/include/dpl/rpc/unix_socket_rpc_connection.h
    ${PROJECT_SOURCE_DIR}/modules/rpc/include/dpl/rpc/unix_socket_rpc_server.h
    PARENT_SCOPE
)

SET(DPL_RPC_INCLUDE_DIR
    ${PROJECT_SOURCE_DIR}/modules/rpc/include
    PARENT_SCOPE
)
