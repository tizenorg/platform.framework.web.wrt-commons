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
 *
 * @file    security_origin_dao_types.h
 * @author  Jihoon Chung (jihoon.chung@samsung.com)
 * @version 1.0
 * @brief This file contains the declaration of
 *           common data types for security origin database.
 */
#ifndef _SECURITY_ORIGIN_DAO_TYPES_H_
#define _SECURITY_ORIGIN_DAO_TYPES_H_

#include <list>
#include <dpl/string.h>

namespace SecurityOriginDB {

enum Feature
{
    FEATURE_GEOLOCATION = 0,
    FEATURE_FILESYSTEM,
    FEATURE_WEB_NOTIFICATION
};

enum Result
{
    RESULT_UNKNOWN = 0,
    RESULT_USER_ALLOWED,
    RESULT_USER_DENIED,
    RESULT_EXCEPTION_ALLOWED,
    RESULT_EXCEPTION_DENIED
};

struct Origin
{
    DPL::String scheme;
    DPL::String host;
    unsigned int port;

    Origin(const DPL::String& Scheme,
           const DPL::String& Host,
           const unsigned int Port) :
        scheme(Scheme),
        host(Host),
        port(Port)
    {
    }

     bool operator== (const Origin& other) const
    {
        return (!DPL::StringCompare(scheme, other.scheme) &&
               !DPL::StringCompare(host, other.host) &&
               port == other.port);
    }

    bool operator!= (const Origin& other) const
    {
        return !(*this == other);
    }
};

struct SecurityOriginData
{
    Feature feature;
    Origin origin;

    SecurityOriginData(const Feature features, const Origin& ori) :
        feature(features),
        origin(ori)
    {
    }

    bool operator== (const SecurityOriginData& other) const
    {
        return !(origin == other.origin) &&
               !(feature == other.feature);
    }

    bool operator!= (const SecurityOriginData& other) const
    {
        return !(*this == other);
    }
};

} // namespace SecurityOriginDB

#endif // _SECURITY_ORIGIN_DAO_TYPES_H_