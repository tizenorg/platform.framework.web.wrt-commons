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

SET(DPL_EVENT_SOURCES
    ${PROJECT_SOURCE_DIR}/modules/event/src/abstract_event_call.cpp
    ${PROJECT_SOURCE_DIR}/modules/event/src/abstract_event_dispatcher.cpp
    ${PROJECT_SOURCE_DIR}/modules/event/src/controller.cpp
    ${PROJECT_SOURCE_DIR}/modules/event/src/event_listener.cpp
    ${PROJECT_SOURCE_DIR}/modules/event/src/event_support.cpp
    ${PROJECT_SOURCE_DIR}/modules/event/src/generic_event_call.cpp
    ${PROJECT_SOURCE_DIR}/modules/event/src/main_event_dispatcher.cpp
    ${PROJECT_SOURCE_DIR}/modules/event/src/thread_event_dispatcher.cpp
    ${PROJECT_SOURCE_DIR}/modules/event/src/inter_context_delegate.cpp
    ${PROJECT_SOURCE_DIR}/modules/event/src/model.cpp
    PARENT_SCOPE
)

SET(DPL_EVENT_HEADERS
    ${PROJECT_SOURCE_DIR}/modules/event/include/dpl/event/abstract_event_call.h
    ${PROJECT_SOURCE_DIR}/modules/event/include/dpl/event/abstract_event_dispatcher.h
    ${PROJECT_SOURCE_DIR}/modules/event/include/dpl/event/controller.h
    ${PROJECT_SOURCE_DIR}/modules/event/include/dpl/event/event_listener.h
    ${PROJECT_SOURCE_DIR}/modules/event/include/dpl/event/event_support.h
    ${PROJECT_SOURCE_DIR}/modules/event/include/dpl/event/generic_event_call.h
    ${PROJECT_SOURCE_DIR}/modules/event/include/dpl/event/main_event_dispatcher.h
    ${PROJECT_SOURCE_DIR}/modules/event/include/dpl/event/thread_event_dispatcher.h
    ${PROJECT_SOURCE_DIR}/modules/event/include/dpl/event/inter_context_delegate.h
    ${PROJECT_SOURCE_DIR}/modules/event/include/dpl/event/model.h
    ${PROJECT_SOURCE_DIR}/modules/event/include/dpl/event/property.h
    ${PROJECT_SOURCE_DIR}/modules/event/include/dpl/event/model_bind_to_dao.h
    PARENT_SCOPE
)

SET(DPL_EVENT_INCLUDE_DIR
    ${PROJECT_SOURCE_DIR}/modules/event/include/
    PARENT_SCOPE
)
