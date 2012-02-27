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
 * @file    object.h
 * @author  Zbigniew Kostrzewa (z.kostrzewa@samsung.com)
 * @version 1.0
 * @brief
 */

#ifndef DPL_DBUS_OBJECT_H
#define DPL_DBUS_OBJECT_H

#include <memory>
#include <string>
#include <gio/gio.h>
#include <dpl/dbus/interface.h>

namespace DPL {
namespace DBus {

class Object;
typedef std::shared_ptr<Object> ObjectPtr;

class Object
{
public:
    /**
     * Creates an object.
     *
     * @param path Object's path.
     * @param interface Interface the object supports.
     * @return Object shared pointer.
     */
    static ObjectPtr create(const std::string& path,
                            const InterfacePtr& interface);

    /**
     * Gets object's path.
     *
     * @return Object's path.
     */
    std::string getPath() const;

    /**
     * Gets object's interface.
     *
     * @return Object's interface.
     */
    InterfacePtr getInterface() const;

private:
    Object(const std::string& path, const InterfacePtr& interface);

    std::string m_path;
    InterfacePtr m_interface;
};

}
}

#endif // WRT_SRC_DBUS_OBJECT_H
