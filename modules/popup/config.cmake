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
# @file     CMakeLists.txt
# @author   Pawel Sikorski (p.sikorski@samsung.com)
# @version     1.0
#

SET(DPL_POPUP_SOURCES
    ${PROJECT_SOURCE_DIR}/modules/popup/src/popup_manager.cpp
    ${PROJECT_SOURCE_DIR}/modules/popup/src/evas_object.cpp
    ${PROJECT_SOURCE_DIR}/modules/popup/src/popup_renderer.cpp
    ${PROJECT_SOURCE_DIR}/modules/popup/src/popup_controller.cpp
    PARENT_SCOPE
    )

SET(DPL_POPUP_HEADERS
    ${PROJECT_SOURCE_DIR}/modules/popup/include/dpl/popup/popup.h
    ${PROJECT_SOURCE_DIR}/modules/popup/include/dpl/popup/evas_object.h
    ${PROJECT_SOURCE_DIR}/modules/popup/include/dpl/popup/popup_controller.h
    ${PROJECT_SOURCE_DIR}/modules/popup/include/dpl/popup/popup_manager.h
    ${PROJECT_SOURCE_DIR}/modules/popup/include/dpl/popup/popup_object.h
    ${PROJECT_SOURCE_DIR}/modules/popup/include/dpl/popup/popup_renderer.h
    PARENT_SCOPE
    )

SET(DPL_POPUP_INCLUDE_DIR
    ${PROJECT_SOURCE_DIR}/modules/popup/include
    PARENT_SCOPE
)