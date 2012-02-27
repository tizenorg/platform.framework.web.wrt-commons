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

SET(DPL_DB_SOURCES
    ${PROJECT_SOURCE_DIR}/modules/db/src/naive_synchronization_object.cpp
    ${PROJECT_SOURCE_DIR}/modules/db/src/orm.cpp
    ${PROJECT_SOURCE_DIR}/modules/db/src/sql_connection.cpp
    ${PROJECT_SOURCE_DIR}/modules/db/src/thread_database_support.cpp
    PARENT_SCOPE
)


SET(DPL_DB_HEADERS
    ${PROJECT_SOURCE_DIR}/modules/db/include/dpl/db/naive_synchronization_object.h
    ${PROJECT_SOURCE_DIR}/modules/db/include/dpl/db/orm_generator.h
    ${PROJECT_SOURCE_DIR}/modules/db/include/dpl/db/orm.h
    ${PROJECT_SOURCE_DIR}/modules/db/include/dpl/db/orm_interface.h
    ${PROJECT_SOURCE_DIR}/modules/db/include/dpl/db/orm_macros.h
    ${PROJECT_SOURCE_DIR}/modules/db/include/dpl/db/sql_connection.h
    ${PROJECT_SOURCE_DIR}/modules/db/include/dpl/db/thread_database_support.h
    PARENT_SCOPE
)

SET(DPL_DB_INCLUDE_DIR
    ${PROJECT_SOURCE_DIR}/modules/db/include
    PARENT_SCOPE
)
