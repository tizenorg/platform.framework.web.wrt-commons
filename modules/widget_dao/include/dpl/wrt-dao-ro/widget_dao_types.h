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
 * @file    widget_dao_types.h
 * @author  Leerang Song (leerang.song@samsung.com)
 * @version 1.0
 * @brief This file contains the declaration of
 *           common data types forwidget database.
 */
#ifndef _WIDGET_DAO_TYPES_H_
#define _WIDGET_DAO_TYPES_H_

#include <list>
#include <memory>
#include <map>
#include <dpl/string.h>

namespace WrtDB {

enum Feature
{
    FEATURE_START = 0,
    FEATURE_GEOLOCATION = 0,
    FEATURE_WEB_NOTIFICATION,
    FEATURE_USER_MEDIA,
    FEATURE_FULLSCREEN_MODE,
    FEATURE_END = FEATURE_FULLSCREEN_MODE
};
extern const std::map<std::string, Feature> g_W3CPrivilegeTextMap;
} // namespace WrtDB

#endif // _WIDGET_DAO_TYPES_H_
