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
# @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
# @version     1.0
# @brief
#

# Check minimum CMake version
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

# Project name
PROJECT(dpl-examples)

# Set common compiler flags
ADD_DEFINITIONS("-Wall -Wextra -ansi -pedantic")

# Add examples
ADD_SUBDIRECTORY(simple)
ADD_SUBDIRECTORY(rpc)
ADD_SUBDIRECTORY(fake_rpc)
ADD_SUBDIRECTORY(binary_queue)
ADD_SUBDIRECTORY(socket)
ADD_SUBDIRECTORY(tcpsock)
ADD_SUBDIRECTORY(timed_event)
ADD_SUBDIRECTORY(single_instance)
ADD_SUBDIRECTORY(crypto_hash)
ADD_SUBDIRECTORY(metronome)
ADD_SUBDIRECTORY(copy)
