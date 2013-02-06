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
 * @file        dbus_deserialization.h
 * @author      Tomasz Swierczek (t.swierczek@samsung.com)
 * @version     1.0
 * @brief       Header file for DBus data deserialization
 */

#ifndef DPL_DBUS_DBUS_DESERIALIZATION_H_
#define DPL_DBUS_DBUS_DESERIALIZATION_H_

#include <map>
#include <vector>
#include <list>
#include <set>
#include <string>
#include <dbus/dbus.h>
#include <dpl/dbus/dbus_signature.h>

namespace DPL {
namespace DBus {
struct Deserialization
{
    static bool deserializePrimitive(
        DBusMessageIter* responseIterator,
        void* arg,
        int type)
    {
        if (dbus_message_iter_get_arg_type(responseIterator) != type) {
            return false;
        }
        dbus_message_iter_get_basic(responseIterator, arg);
        return true;
    }

    // char* and all integer types + doubles
    template<typename T>
    static bool deserialize(DBusMessageIter* responseIterator, T* arg)
    {
        if (dbus_message_iter_get_arg_type(responseIterator)
            != SimpleType<T>::value)
        {
            return false;
        }
        dbus_message_iter_get_basic(responseIterator, arg);
        return true;
    }

    // float case - read as double
    static bool deserialize(DBusMessageIter* responseIterator, float* arg)
    {
        double d;
        if (!deserialize(responseIterator, &d)) {
            return false;
        }
        *arg = static_cast<float>(d);
        return true;
    }

    // std::string
    static bool deserialize(
        DBusMessageIter* responseIterator,
        std::string* arg)
    {
        char* str = NULL;
        if (!deserialize(responseIterator, &str)) {
            return false;
        }
        *arg = std::string(str);
        return true;
    }

    // dbus array deserialization
    template<typename T>
    static bool deserializeContainer(
        DBusMessageIter* responseIterator,
        T* arg)
    {
        if (dbus_message_iter_get_arg_type(responseIterator)
            != DBUS_TYPE_ARRAY)
        {
            return false;
        }
        DBusMessageIter subIterator;
        dbus_message_iter_recurse(responseIterator, &subIterator);
        while (dbus_message_iter_get_arg_type(&subIterator)
               != DBUS_TYPE_INVALID)
        {
            arg->push_back(typename T::value_type());
            if (!deserialize(&subIterator, &arg->back())) {
                return false;
            }
            dbus_message_iter_next(&subIterator);
        }
        return true;
    }

    // std::vector
    template<typename T>
    static bool deserialize(
        DBusMessageIter* responseIterator,
        std::vector<T>* arg)
    {
        return deserializeContainer(responseIterator, arg);
    }

    // std::list
    template<typename T>
    static bool deserialize(
        DBusMessageIter* responseIterator,
        std::list<T>* arg)
    {
        return deserializeContainer(responseIterator, arg);
    }

    // std::set
    template<typename T>
    static bool deserialize(
        DBusMessageIter* responseIterator,
        std::set<T>* arg)
    {
        if (dbus_message_iter_get_arg_type(responseIterator)
            != DBUS_TYPE_ARRAY)
        {
            return false;
        }
        DBusMessageIter subIterator;
        dbus_message_iter_recurse(responseIterator, &subIterator);
        while (dbus_message_iter_get_arg_type(&subIterator)
               != DBUS_TYPE_INVALID)
        {
            typename std::set<T>::value_type element;
            if (!deserialize(&subIterator, &element)) {
                return false;
            }
            arg->insert(element);
            dbus_message_iter_next(&subIterator);
        }
        return true;
    }

    // std::pair
    template<typename A, typename B>
    static bool deserialize(
        DBusMessageIter* argsIterator,
        const std::pair<A, B>* arg)
    {
        if (dbus_message_iter_get_arg_type(argsIterator)
            != DBUS_TYPE_DICT_ENTRY)
        {
            return false;
        }
        DBusMessageIter dictEntryIterator;
        dbus_message_iter_recurse(argsIterator, &dictEntryIterator);
        if (!deserialize(dictEntryIterator, &arg->first)) {
            return false;
        }
        dbus_message_iter_next(&dictEntryIterator);
        if (!deserialize(dictEntryIterator, &arg->second)) {
            return false;
        }
        return true;
    }

    // std::map
    template<typename K, typename V>
    static bool deserialize(
        DBusMessageIter* responseIterator,
        const std::map<K, V>* arg)
    {
        if (dbus_message_iter_get_arg_type(responseIterator)
            != DBUS_TYPE_ARRAY)
        {
            return false;
        }
        DBusMessageIter subIterator;
        dbus_message_iter_recurse(responseIterator, &subIterator);
        while (dbus_message_iter_get_arg_type(&subIterator)
               != DBUS_TYPE_INVALID)
        {
            typename std::pair<K, V> element;
            if (!deserialize(&subIterator, &element)) {
                return false;
            }
            arg->insert(element);
            dbus_message_iter_next(&subIterator);
        }
        return true;
    }
};

template<>
inline bool Deserialization::deserialize<bool>(
    DBusMessageIter* responseIterator,
    bool* arg)
{
    unsigned int value;
    if (dbus_message_iter_get_arg_type(responseIterator)
        != SimpleType<bool>::value)
    {
        return false;
    }
    dbus_message_iter_get_basic(responseIterator, &value);
    *arg = static_cast<bool>(value);
    return true;
}
} // namespace DBus
} // namespace DPL

#endif // DPL_DBUS_DBUS_DESERIALIZATION_H_
