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
# @author      Soyoung Kim(sy037.kim@samsung.com)
# @version     1.0
# @brief
#

SET(DPL_UTILS_SOURCES
    ${PROJECT_SOURCE_DIR}/modules/utils/src/file_utils.cpp
    ${PROJECT_SOURCE_DIR}/modules/utils/src/folder_size.cpp
    ${PROJECT_SOURCE_DIR}/modules/utils/src/mime_type_utils.cpp
    ${PROJECT_SOURCE_DIR}/modules/utils/src/warp_iri.cpp
    ${PROJECT_SOURCE_DIR}/modules/utils/src/widget_version.cpp
    ${PROJECT_SOURCE_DIR}/modules/utils/src/wrt_global_settings_internal.cpp
    ${PROJECT_SOURCE_DIR}/modules/utils/src/wrt_global_settings.cpp
    ${PROJECT_SOURCE_DIR}/modules/utils/src/wrt_utility.cpp
    PARENT_SCOPE
)


SET(DPL_UTILS_HEADERS
    ${PROJECT_SOURCE_DIR}/modules/utils/include/file_utils.h
    ${PROJECT_SOURCE_DIR}/modules/utils/include/folder_size.h
    ${PROJECT_SOURCE_DIR}/modules/utils/include/mime_type_utils.h
    ${PROJECT_SOURCE_DIR}/modules/utils/include/warp_iri.h
    ${PROJECT_SOURCE_DIR}/modules/utils/include/widget_version.h
    ${PROJECT_SOURCE_DIR}/modules/utils/include/wrt_global_settings.h
    ${PROJECT_SOURCE_DIR}/modules/utils/include/wrt_global_settings_internal.h
    ${PROJECT_SOURCE_DIR}/modules/utils/include/wrt_utility.h
    PARENT_SCOPE
)

SET(DPL_UTILS_INCLUDE_DIR
    ${PROJECT_SOURCE_DIR}/modules/utils/include
    PARENT_SCOPE
)
