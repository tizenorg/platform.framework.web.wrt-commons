/*
 * Copyright (c) 2011 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */
/**
 * @file    object.cpp
 * @author  Zbigniew Kostrzewa (z.kostrzewa@samsung.com)
 * @version 1.0
 * @brief
 */
#include <stddef.h>
#include <dpl/dbus/object.h>

namespace DPL {
namespace DBus {
ObjectPtr Object::create(const std::string& path, const InterfacePtr& interface)
{
    return ObjectPtr(new Object(path, interface));
}

std::string Object::getPath() const
{
    return m_path;
}

InterfacePtr Object::getInterface() const
{
    return m_interface;
}

Object::Object(const std::string& path, const InterfacePtr& interface) :
    m_path(path),
    m_interface(interface)
{}
}
}