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

SET(DPL_LOCALIZATION_SOURCES
    ${PROJECT_SOURCE_DIR}/modules/localization/src/localization_utils.cpp
    ${PROJECT_SOURCE_DIR}/modules/localization/src/w3c_file_localization.cpp
    ${PROJECT_SOURCE_DIR}/modules/localization/src/LanguageTagsProvider.cpp
    PARENT_SCOPE
)


SET(DPL_LOCALIZATION_HEADERS
    ${PROJECT_SOURCE_DIR}/modules/localization/include/dpl/localization/localization_utils.h
    ${PROJECT_SOURCE_DIR}/modules/localization/include/dpl/localization/w3c_file_localization.h
    ${PROJECT_SOURCE_DIR}/modules/localization/include/LanguageTagsProvider.h
    PARENT_SCOPE
)

SET(DPL_LOCALIZATION_INCLUDE_DIR
    ${PROJECT_SOURCE_DIR}/modules/localization/include
    PARENT_SCOPE
)
