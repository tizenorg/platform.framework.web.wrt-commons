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
 * This file contains the declaration of widget dao class.
 *
 * @file    widget_dao_read_only.cpp
 * @author  Yang Jie (jie2.yang@samsung.com)
 * @author  Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @author  Pawel Sikorski (p.sikorski@samsung.com)
 * @version 1.0
 * @brief   This file contains the declaration of widget dao
 */

//#include "../mockup_include/dpl/wrt-dao-rw/widget_dao.h"
#include <dpl/wrt-dao-rw/widget_dao.h>

#include <sstream>
#include <dpl/foreach.h>
#include <dpl/log/log.h>
#include <dpl/sstream.h>

namespace WrtDB {
std::map<DbWidgetHandle,
         WidgetDAO::WidgetStartFileList> WidgetDAO::s_startFileMap;
std::map<DbWidgetHandle,
         WidgetDAO::LocalizedStartFileList> WidgetDAO::s_localizedStartFileMap;
std::map<DbWidgetHandle, DPL::String> WidgetDAO::s_pathMap;

DbWidgetHandle WidgetDAO::getHandle(const WidgetGUID /* GUID */) const
{
    LogError("Not impleneted!");
    return 0;
}

DbWidgetHandle WidgetDAO::getHandle(const DPL::String /* tizenAppId */)
{
    LogError("Not implemented!");
    return 0;
}

DPL::String WidgetDAO::getPath() const
{
    return s_pathMap[m_widgetHandle];
}

void WidgetDAO::setPath(const DPL::String &path) const
{
    s_pathMap[m_widgetHandle] = path;
}

WidgetLocalizedInfo
WidgetDAO::getLocalizedInfo(const DPL::String& /* languageTag */)
const
{
    LogError("Not implemented!");
    return WidgetLocalizedInfo();
}

DPL::OptionalString WidgetDAO::getDefaultlocale() const
{
    LogError("Not implemented!");
    return DPL::OptionalString();
}

WidgetDAO::WidgetLocalizedIconList WidgetDAO::getLocalizedIconList() const
{
    LogError("Not implemented!");
    return WidgetLocalizedIconList();
}

WidgetDAO::WidgetIconList WidgetDAO::getIconList() const
{
    LogError("Not implemented!");
    return WidgetIconList();
}

WidgetDAO::LocalizedStartFileList WidgetDAO::getLocalizedStartFileList() const
{
    return s_localizedStartFileMap[m_widgetHandle];
}

void WidgetDAO::setLocalizedStartFileList(const LocalizedStartFileList &list)
const
{
    s_localizedStartFileMap[m_widgetHandle] = list;
}

WidgetDAO::WidgetStartFileList WidgetDAO::getStartFileList() const
{
    return s_startFileMap[m_widgetHandle];
}

void WidgetDAO::setStartFileList(const WidgetStartFileList &list) const
{
    s_startFileMap[m_widgetHandle] = list;
}
} // namespace WrtDB
