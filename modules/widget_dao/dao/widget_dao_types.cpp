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
 * @file    widget_dao_types.cpp
 * @author  Leerang Song (leerang.song@samsung.com)
 * @version 1.0
 * @brief This file contains the declaration of
 *           common data types forwidget database.
 */

#include <dpl/wrt-dao-ro/widget_dao_types.h>
#include <dpl/log/log.h>

namespace WrtDB {

const std::map<std::string, Feature> g_W3CPrivilegeTextMap = {
    {"http://tizen.org/privilege/location",     FEATURE_GEOLOCATION},
    {"http://tizen.org/privilege/notification", FEATURE_WEB_NOTIFICATION},
    {"http://tizen.org/privilege/mediacapture", FEATURE_USER_MEDIA},
    {"http://tizen.org/privilege/fullscreen", FEATURE_FULLSCREEN_MODE}
};
} // namespace SecurityOriginDB
