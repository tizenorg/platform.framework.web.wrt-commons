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
/*
 * @file        dbus_serialization.h
 * @author      Tomasz Swierczek (t.swierczek@samsung.com)
 * @version     1.0
 * @brief       Header file for DBus data derialization
 */

#ifndef DPL_DBUS_DBUS_SERIALIZATION_H_
#define DPL_DBUS_DBUS_SERIALIZATION_H_

#include <map>
#include <vector>
#include <list>
#include <set>
#include <string>
#include <dbus/dbus.h>
#include <dpl/foreach.h>
#include <dpl/dbus/dbus_signature.h>

namespace DPL {
namespace DBus {
struct Serialization
{
    // std::string
    static bool serialize(
        DBusMessageIter* argsIterator,
        const std::string& str)
    {
        return serialize(argsIterator, str.c_str());
    }

    // float case - send as double
    static bool serialize(DBusMessageIter* argsIterator, const float& arg)
    {
        const double d = static_cast<double>(arg);
        return serialize(argsIterator, d);
    }

    // char* and all integer types + doubles
    template<typename T>
    static bool serialize(DBusMessageIter* argsIterator, const T& arg)
    {
        return dbus_message_iter_append_basic(argsIterator,
                                              SimpleType<T>::value,
                                              &arg);
    }

    // dbus array serialization
    template<typename T>
    static bool serializeContainer(
        DBusMessageIter* argsIterator,
        const T& arg)
    {
        typename T::const_iterator containerIt;
        DBusMessageIter subIterator;
        if (!dbus_message_iter_open_container(argsIterator, DBUS_TYPE_ARRAY,
                                              Signature<typename T::value_type>
                                                  ::value(), &subIterator))
        {
            return false;
        }
        FOREACH(containerIt, arg) {
            if (!serialize(&subIterator, *containerIt)) {
                return false;
            }
        }
        return dbus_message_iter_close_container(argsIterator, &subIterator);
    }

    // std::vector
    template<typename T>
    static bool serialize(
        DBusMessageIter* argsIterator,
        const std::vector<T> &arg)
    {
        return serializeContainer(argsIterator, arg);
    }

    // std::list
    template<typename T>
    static bool serialize(
        DBusMessageIter* argsIterator,
        const std::list<T> &arg)
    {
        return serializeContainer(argsIterator, arg);
    }

    // std::set
    template<typename T>
    static bool serialize(
        DBusMessageIter* argsIterator,
        const std::set<T> &arg)
    {
        return serializeContainer(argsIterator, arg);
    }

    // std::pair
    template<typename A, typename B>
    static bool serialize(
        DBusMessageIter* argsIterator,
        const std::pair<A, B> &arg)
    {
        DBusMessageIter dictEntryIterator;
        if (!dbus_message_iter_open_container(argsIterator,
                                              DBUS_TYPE_DICT_ENTRY, NULL,
                                              &dictEntryIterator))
        {
            return false;
        }
        if (!serialize(dictEntryIterator, arg.first)) {
            return false;
        }
        if (!serialize(dictEntryIterator, arg.second)) {
            return false;
        }
        return dbus_message_iter_close_container(argsIterator,
                                                 &dictEntryIterator);
    }

    // std::map
    template<typename K, typename V>
    static bool serialize(
        DBusMessageIter* argsIterator,
        const std::map<K, V> &arg)
    {
        return serializeContainer(argsIterator, arg);
    }
};

// char* and all integer types + doubles
template<>
inline bool Serialization::serialize<bool>(DBusMessageIter* argsIterator,
                                           const bool& arg)
{
    unsigned int value = static_cast<unsigned int>(arg);
    return dbus_message_iter_append_basic(argsIterator,
                                          SimpleType<bool>::value,
                                          &value);
}
} // namespace DBus
} // namespace DPL

#endif // DPL_DBUS_DBUS_SERIALIZATION_H_
