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
 * @file        dbus_server_deserialization.h
 * @author      Tomasz Swierczek (t.swierczek@samsung.com)
 * @version     1.0
 * @brief       Header file for DBus data deserialization from GVariant
 */

#ifndef DPL_DBUS_DBUS_SERVER_DESERIALIZATION_H_
#define DPL_DBUS_DBUS_SERVER_DESERIALIZATION_H_

#include <string>
#include <vector>
#include <list>
#include <map>
#include <gio/gio.h>
#include <dpl/assert.h>

namespace DPL {
namespace DBus {

struct ServerDeserialization {

    template<typename T, typename ...Args>
    static bool deserialize(GVariant* g, T* arg1, Args... args)
    {
        Assert(NULL != g);
        Assert(NULL != arg1);
        GVariantIter* iterator = g_variant_iter_new(g);
        if (NULL == iterator) {
            return false;
        }
        if (!deserializeIterator(iterator, arg1)) {
            g_variant_iter_free(iterator);
            return false;
        }
        if (!deserializeIterator(iterator, args...)) {
            g_variant_iter_free(iterator);
            return false;
        }
        g_variant_iter_free(iterator);
        return true;
    }

    template<typename T>
    static bool deserialize(GVariant* g, T* arg1)
    {
        Assert(NULL != g);
        Assert(NULL != arg1);
        GVariantIter* iterator = g_variant_iter_new(g);
        if (NULL == iterator) {
            return false;
        }
        if (!deserializeIterator(iterator, arg1)) {
            g_variant_iter_free(iterator);
            return false;
        }
        g_variant_iter_free(iterator);
        return true;
    }

    // deserialization from GVariant tuple iterator
    template<typename T, typename ...Args>
    static bool deserializeIterator(GVariantIter* g, T* arg1, Args... args)
    {
        Assert(NULL != g);
        Assert(NULL != arg1);
        GVariant* elem = g_variant_iter_next_value(g);
        if (NULL == elem) {
            return false;
        }
        if (!deserializeElem(elem, arg1)) {
            return false;
        }
        if (!deserializeIterator(g, args...)) {
            return false;
        }
        return true;
    }

    template<typename T>
    static bool deserializeIterator(GVariantIter* g, T* arg1)
    {
        Assert(NULL != g);
        Assert(NULL != arg1);
        GVariant* elem = g_variant_iter_next_value(g);
        if (NULL == elem) {
            return false;
        }
        if (!deserializeElem(elem, arg1)) {
            return false;
        }
        g_variant_unref(elem);
        return true;
    }

    // type specialization
    static bool deserializeElem(GVariant* parameters, std::string* outStr)
    {
        const gchar* arg = g_variant_get_string(parameters, NULL);
        *outStr = std::string(arg);
        return true;
    }

    static bool deserializeElem(GVariant* parameters, int* outInt)
    {
        gint32 arg = g_variant_get_int32(parameters);
        *outInt = arg;
        return true;
    }

    static bool deserializeElem(GVariant* parameters, unsigned* outInt)
    {
        guint32 arg = g_variant_get_uint32(parameters);
        *outInt = arg;
        return true;
    }

    static bool deserializeElem(GVariant* parameters, bool* outInt)
    {
        gboolean arg = g_variant_get_boolean(parameters);
        *outInt = arg;
        return true;
    }

    static bool deserializeElem(GVariant* parameters, float* outInt)
    {
        gdouble arg = g_variant_get_double(parameters);
        *outInt = static_cast<float>(arg);
        return true;
    }

    static bool deserializeElem(GVariant* parameters,
                            std::vector<std::string>* outArray)
    {
        unsigned int i = 0;
        gsize length = 0;
        const gchar** args = g_variant_get_strv(
                parameters,
                &length);
        for (i = 0; i < length; ++i) {
            outArray->push_back(std::string(args[i]));
        }
        g_free(args);
        return true;
    }

    static bool deserializeElem(GVariant* parameters,
                                std::list<std::string>* outArray)
    {
        unsigned int i = 0;
        gsize length = 0;
        const gchar** args = g_variant_get_strv(
                parameters,
                &length);
        for (i = 0; i < length; ++i) {
            outArray->push_back(std::string(args[i]));
        }
        g_free(args);
        return true;
    }
};

} // namespace DBus
} // namespace DPL

#endif // DPL_DBUS_DBUS_SERVER_DESERIALIZATION_H_
