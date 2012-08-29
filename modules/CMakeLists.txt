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
# @file        CMakeLists.txt
# @author      Lukasz Marek (l.marek@samsung.com)
# @version     1.0
# @brief
#

include(core/config.cmake)
include(dbus/config.cmake)
include(db/config.cmake)
include(event/config.cmake)
include(socket/config.cmake)
include(rpc/config.cmake)
include(test/config.cmake)
include(log/config.cmake)
ADD_SUBDIRECTORY(widget_dao)
ADD_SUBDIRECTORY(auto_save_dao)
include(popup/config.cmake)
include(utils/config.cmake)
include(localization/config.cmake)
include(support/config.cmake)
include(encryption/config.cmake)
