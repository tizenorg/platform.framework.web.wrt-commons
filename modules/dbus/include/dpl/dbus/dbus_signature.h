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
 * @brief       Header file for DBus data signatures
 */

#ifndef DPL_DBUS_SIGNATURE_H
#define DPL_DBUS_SIGNATURE_H

#include <dbus/dbus.h>
#include <string>

namespace DPL {
namespace DBus {

template<typename T>
struct SimpleType;

template<typename T>
struct Signature
{
    static inline const char* value()
    {
        static const char signature[] =
        { (char) SimpleType<T>::value, 0 };
        return signature;
    }
};

// signed integer types
template<int size>
struct __SignedIntegerType;

template<>
struct __SignedIntegerType<1>
{
    static const int value = DBUS_TYPE_INT16;
};

template<>
struct __SignedIntegerType<2>
{
    static const int value = DBUS_TYPE_INT16;
};

template<>
struct __SignedIntegerType<4>
{
    static const int value = DBUS_TYPE_INT32;
};

template<>
struct __SignedIntegerType<8>
{
    static const int value = DBUS_TYPE_INT64;
};

// unsigned integer types
template<int size>
struct __UnsignedIntegerType;

template<>
struct __UnsignedIntegerType<1>
{
    static const int value = DBUS_TYPE_BYTE;
};
template<>
struct __UnsignedIntegerType<2>
{
    static const int value = DBUS_TYPE_UINT16;
};
template<>
struct __UnsignedIntegerType<4>
{
    static const int value = DBUS_TYPE_UINT32;
};
template<>
struct __UnsignedIntegerType<8>
{
    static const int value = DBUS_TYPE_UINT64;
};

// basic types
template<>
struct SimpleType<bool>
{
    static const int value = DBUS_TYPE_BOOLEAN;
};

template<>
struct SimpleType<char>
{
    static const int value = __UnsignedIntegerType<sizeof(char)>::value;
};

template<>
struct SimpleType<signed char>
{
    static const int value = __SignedIntegerType<sizeof(signed char)>::value;
};

template<>
struct SimpleType<unsigned char>
{
    static const int value =
            __UnsignedIntegerType<sizeof(unsigned char)>::value;
};

template<>
struct SimpleType<short>
{
    static const int value = __SignedIntegerType<sizeof(short)>::value;
};

template<>
struct SimpleType<unsigned short>
{
    static const int value =
            __UnsignedIntegerType<sizeof(unsigned short)>::value;
};

template<>
struct SimpleType<int>
{
    static const int value = __SignedIntegerType<sizeof(int)>::value;
};

template<>
struct SimpleType<unsigned int>
{
    static const int value =
            __UnsignedIntegerType<sizeof(unsigned int)>::value;
};

template<>
struct SimpleType<long>
{
    static const int value = __SignedIntegerType<sizeof(long)>::value;
};

template<>
struct SimpleType<unsigned long>
{
    static const int value =
            __UnsignedIntegerType<sizeof(unsigned long)>::value;
};

template<>
struct SimpleType<long long>
{
    static const int value = __SignedIntegerType<sizeof(long long)>::value;
};

template<>
struct SimpleType<unsigned long long>
{
    static const int value = __UnsignedIntegerType<
            sizeof(unsigned long long)>::value;
};

template<>
struct SimpleType<float>
{
    static const int value = DBUS_TYPE_DOUBLE;
};

template<>
struct SimpleType<double>
{
    static const int value = DBUS_TYPE_DOUBLE;
};

template<>
struct SimpleType<const char *>
{
    static const int value = DBUS_TYPE_STRING;
};

template<>
struct SimpleType<char *>
{
    static const int value = DBUS_TYPE_STRING;
};

template<>
struct SimpleType<std::string>
{
    static const int value = DBUS_TYPE_STRING;
};

// STL containers signatures

// generic array
template<typename T>
struct ArraySignature
{
    static inline const char* value()
    {
        static const std::string signature = std::string(
                DBUS_TYPE_ARRAY_AS_STRING) + Signature<T>::value();
        return signature.c_str();
    }
};

// std::vector
template<typename T>
struct Signature<std::vector<T> > : public ArraySignature<T>
{
};

// std::list
template<typename T>
struct Signature<std::list<T> > : public ArraySignature<T>
{
};

// std::set
template<typename T>
struct Signature<std::set<T> > : public ArraySignature<T>
{
};

// std::pair
template<typename K, typename V>
struct Signature<std::pair<K, V> >
{
    static inline const char* value()
    {
        static const std::string signature = std::string(
                DBUS_DICT_ENTRY_BEGIN_CHAR_AS_STRING)
                + Signature<K>::value() + Signature<V>::value()
                + DBUS_DICT_ENTRY_END_CHAR_AS_STRING;
        return signature.c_str();
    }
};

// std::map
template<typename K, typename V>
struct Signature<std::map<K, V> > : public ArraySignature<std::pair<K, V> >
{
};

} // namespace DBus
} // namespace DPL

#endif // DPL_DBUS_SIGNATURE_H
