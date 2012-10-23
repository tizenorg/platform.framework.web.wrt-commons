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
 * @file    localization_utils.h
 * @author  Bartosz Janiak (b.janiak@samsung.com)
 * @version 1.0
 */

#ifndef LOCALIZATION_UTILS_H
#define LOCALIZATION_UTILS_H

#include <list>

#include <dpl/log/log.h>
#include <dpl/optional.h>
#include <dpl/read_write_mutex.h>
#include <dpl/string.h>

/**
 * WidgetIcon
 * Structure to hold information about widget icon.
 */

struct WidgetIcon
{
    WidgetIcon() :
        width(DPL::Optional<int>::Null),
        height(DPL::Optional<int>::Null)
    {
    }

    /*
     * a valid URI to an image file inside the widget package that represents an
     * iconic representation of the widget
     */
    DPL::String src;
    DPL::Optional<int> width;       /// the width of the icon in pixels
    DPL::Optional<int> height;      /// the height of the icon in pixels

    bool operator==(const WidgetIcon &other) const
    {
        return src == other.src;
    }
};

struct WidgetStartFileInfo
{
    DPL::String file;
    DPL::String localizedPath;
    DPL::String encoding;
    DPL::String type;

    bool operator==(const WidgetStartFileInfo& other) const
    {
        return file == other.file &&
               localizedPath == other.localizedPath &&
               encoding == other.encoding &&
               type == other.type;
    }
};

typedef DPL::Optional<WidgetIcon> OptionalWidgetIcon;
typedef DPL::Optional<WidgetStartFileInfo> OptionalWidgetStartFileInfo;

namespace LocalizationUtils {
/* Function below is deprecated - please use static functions of LanguageTagsProvider:
 * BCP47LanguageTagToLocale() and LocaleToBCP47LanguageTag() */
DPL::String BCP47LanguageTagToLocale(const DPL::String&) __attribute__((deprecated));

}
#endif //LOCALIZATION_UTILS_H
