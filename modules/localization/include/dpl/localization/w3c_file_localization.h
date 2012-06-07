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
 * @file    w3c_file_localization.h
 * @author  Lukasz Wrzosek (l.wrzosek@samsung.com)
 * @version 1.0
 */

#ifndef W3C_FILE_LOCALIZATION_H
#define W3C_FILE_LOCALIZATION_H

#include <dpl/wrt-dao-ro/widget_dao_read_only.h>
#include <dpl/optional.h>
#include <dpl/string.h>
#include <list>

#include <dpl/localization/localization_utils.h>

// WrtDB::DbWidgetHandle
#include <dpl/wrt-dao-ro/common_dao_types.h>

namespace W3CFileLocalization {
typedef std::list<WidgetIcon> WidgetIconList;

DPL::Optional<DPL::String> getFilePathInWidgetPackageFromUrl(
        WrtDB::DbWidgetHandle widgetHandle,
        const LanguageTagsList &languageTags,
        const DPL::String &url);

DPL::Optional<DPL::String> getFilePathInWidgetPackage(
        WrtDB::DbWidgetHandle widgetHandle,
        const LanguageTagsList &languageTags,
        const DPL::String& file);

DPL::OptionalString getStartFile(WrtDB::DbWidgetHandle widgetHandle);
OptionalWidgetIcon getIcon(WrtDB::DbWidgetHandle widgetHandle);
WidgetIconList getValidIconsList(
        WrtDB::DbWidgetHandle widgetHandle,
        const LanguageTagsList &languageTags);

OptionalWidgetStartFileInfo getStartFileInfo(
        WrtDB::DbWidgetHandle widgetHandle,
        const LanguageTagsList &tagsList);
WrtDB::WidgetLocalizedInfo getLocalizedInfo(WrtDB::DbWidgetHandle widgetHandle);
}

#endif //W3C_FILE_LOCALIZATION_H
