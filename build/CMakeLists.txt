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
# @author      Lukasz Marek (l.marek@samsung.com)
# @version     1.0
# @brief
#

ADD_SUBDIRECTORY(core)
ADD_SUBDIRECTORY(dbus)
ADD_SUBDIRECTORY(db)
ADD_SUBDIRECTORY(event)
ADD_SUBDIRECTORY(socket)
ADD_SUBDIRECTORY(rpc)
ADD_SUBDIRECTORY(test)
#ADD_SUBDIRECTORY(log)
ADD_SUBDIRECTORY(widget_dao)
ADD_SUBDIRECTORY(auto_save_dao)
ADD_SUBDIRECTORY(security_origin_dao)
ADD_SUBDIRECTORY(custom_handler_dao)
ADD_SUBDIRECTORY(utils)
ADD_SUBDIRECTORY(support)
ADD_SUBDIRECTORY(encryption)
