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
 * @file        dbus_server_serialization.h
 * @author      Tomasz Swierczek (t.swierczek@samsung.com)
 * @version     1.0
 * @brief       Header file for DBus data serialization to GVariant
 */

#ifndef DPL_DBUS_DBUS_SERVER_SERIALIZATION_H_
#define DPL_DBUS_DBUS_SERVER_SERIALIZATION_H_

#include <string>
#include <gio/gio.h>

namespace DPL {
namespace DBus {

struct ServerSerialization {

    template<typename ...Args>
    static GVariant* serialize(Args... args)
    {
        GVariantBuilder* builder = g_variant_builder_new(G_VARIANT_TYPE_TUPLE);
        if (NULL == builder) {
            return NULL;
        }
        serializeBuilder(builder, args ...);
        return g_variant_builder_end(builder);
    }

    // serialization on GVariantBuilder
    template<typename T, typename ...Args>
    static void serializeBuilder(GVariantBuilder* builder,
                                 const T& arg,
                                 Args... args)
    {
        serializeElem(builder, arg);
        serializeBuilder(builder, args ...);
    }

    template<typename T>
    static void serializeBuilder(GVariantBuilder* builder, const T& arg)
    {
        serializeElem(builder, arg);
    }

    // type specialization
    static void serializeElem(GVariantBuilder* builder, int arg)
    {
        g_variant_builder_add_value(builder, g_variant_new_int32(arg));
    }

    static void serializeElem(GVariantBuilder* builder, unsigned arg)
    {
        g_variant_builder_add_value(builder, g_variant_new_uint32(arg));
    }

    static void serializeElem(GVariantBuilder* builder, bool arg)
    {
        g_variant_builder_add_value(builder, g_variant_new_boolean(arg));
    }

    static void serializeElem(GVariantBuilder* builder, float arg)
    {
        gdouble d = static_cast<gdouble>(arg);
        g_variant_builder_add_value(builder, g_variant_new_double(d));
    }

    static void serializeElem(GVariantBuilder* builder, const char* arg)
    {
        g_variant_builder_add_value(builder, g_variant_new_string(arg));
    }

    static void serializeElem(GVariantBuilder* builder,
                                 const std::string& arg)
    {
        g_variant_builder_add_value(builder, g_variant_new_string(arg.c_str()));
    }
};

} // namespace DBus
} // namespace DPL

#endif // DPL_DBUS_DBUS_SERVER_SERIALIZATION_H_
