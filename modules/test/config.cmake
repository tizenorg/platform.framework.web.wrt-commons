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

SET(DPL_TEST_ENGINE_SOURCES
    ${PROJECT_SOURCE_DIR}/modules/test/src/test_results_collector.cpp
    ${PROJECT_SOURCE_DIR}/modules/test/src/test_runner.cpp
    PARENT_SCOPE
)


SET(DPL_TEST_ENGINE_HEADERS
    ${PROJECT_SOURCE_DIR}/modules/test/include/dpl/test/test_results_collector.h
    ${PROJECT_SOURCE_DIR}/modules/test/include/dpl/test/test_runner.h
    PARENT_SCOPE
)

SET(DPL_TEST_ENGINE_INCLUDE_DIR
    ${PROJECT_SOURCE_DIR}/modules/test/include
    PARENT_SCOPE
)
