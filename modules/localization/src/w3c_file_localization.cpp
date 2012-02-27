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
 * @file    w3c_file_localization.cpp
 * @author  Lukasz Wrzosek (l.wrzosek@samsung.com)
 * @version 1.0
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <dpl/localization/w3c_file_localization.h>

#include <dpl/wrt-dao-rw/widget_dao.h>
#include <dpl/localization/localization_utils.h>

#include <dpl/log/log.h>
#include <dpl/string.h>
#include <dpl/optional.h>
#include <dpl/foreach.h>

using namespace WrtDB;

namespace {
const DPL::String FILE_URI_BEGIN = L"file://";
const DPL::String WIDGET_URI_BEGIN = L"widget://";

DPL::Optional<std::string> GetFilePathInWidgetPackageInternal(
        const LanguageTagsList &tags,
        const std::string& basePath,
        std::string filePath)
{
    LogDebug("Looking for file: " << filePath << "  in: " << basePath);
    using namespace LocalizationUtils;

    //Check if string isn't empty
    if (filePath.size() == 0) { return DPL::Optional<std::string>::Null; }
    //Removing preceding '/'
    if (filePath[0] == '/') { filePath.erase(0, 1); }
    //Check if string isn't empty
    if (filePath.size() == 0) { return DPL::Optional<std::string>::Null; }

    LogDebug("locales size = " << tags.size());
    for (LanguageTagsList::const_iterator it = tags.begin();
         it != tags.end();
         ++it) {
        LogDebug("Trying locale: " << *it);
        std::string path = basePath;
        if (path[path.size() - 1] == '/') {
            path.erase(path.size() - 1);
        }

        if (it->empty()) {
            path += "/" + filePath;
        } else {
            path += "/locales/" + DPL::ToUTF8String(*it) + "/" + filePath;
        }

        LogDebug("Trying locale: " << *it << " | " << path);
        struct stat buf;
        if (0 == stat(path.c_str(), &buf)) {
            if ((buf.st_mode & S_IFMT) == S_IFREG) {
                path.erase(0, basePath.length());
                return DPL::Optional<std::string>(path);
            }
        }
    }

    return DPL::Optional<std::string>::Null;
}

DPL::Optional<DPL::String> GetFilePathInWidgetPackageInternal(
        const LanguageTagsList &languageTags,
        const DPL::String& basePath,
        const DPL::String& filePath)
{
    DPL::Optional<std::string> path =
        GetFilePathInWidgetPackageInternal(languageTags,
                                           DPL::ToUTF8String(basePath),
                                           DPL::ToUTF8String(filePath));
    DPL::Optional<DPL::String> dplPath;
    if (!!path) {
        dplPath = DPL::FromUTF8String(*path);
    }
    return dplPath;
}
}

namespace W3CFileLocalization {
DPL::Optional<DPL::String> getFilePathInWidgetPackageFromUrl(
        DbWidgetHandle widgetHandle,
        const LanguageTagsList &languageTags,
        const DPL::String &url)
{
    DPL::String req = url;
    WidgetDAO dao(widgetHandle);

    if (req.find(WIDGET_URI_BEGIN) == 0) {
        req.erase(0, WIDGET_URI_BEGIN.length());
    } else if (req.find(FILE_URI_BEGIN) == 0) {
        req.erase(0, FILE_URI_BEGIN.length());
        if (req.find(dao.getPath()) == 0) {
            req.erase(0, dao.getPath().length());
        }
    } else {
        LogDebug("Unknown path format, ignoring");
        return DPL::Optional<DPL::String>::Null;
    }

    auto widgetPath = dao.getPath();

    DPL::Optional<DPL::String> found =
        GetFilePathInWidgetPackageInternal(languageTags, widgetPath, req);

    if (!found) {
        LogError("Path not found within current locale in current widget");
        return DPL::Optional<DPL::String>::Null;
    }

    found = widgetPath + *found;

    return found;
}

DPL::Optional<DPL::String> getFilePathInWidgetPackage(
        WrtDB::DbWidgetHandle widgetHandle,
        const LanguageTagsList &languageTags,
        const DPL::String& file)
{
    WidgetDAO dao(widgetHandle);
    return GetFilePathInWidgetPackageInternal(languageTags, dao.getPath(), file);
}

DPL::OptionalString getStartFile(const WrtDB::DbWidgetHandle widgetHandle)
{
    using namespace LocalizationUtils;

    WidgetDAO dao(widgetHandle);

    WidgetDAO::LocalizedStartFileList locList = dao.getLocalizedStartFileList();
    WidgetDAO::WidgetStartFileList list = dao.getStartFileList();
    LanguageTagsList tagsList = LocalizationUtils::GetUserAgentLanguageTags();

    DPL::OptionalString defaultLoc = dao.getDefaultlocale();
    if (!!defaultLoc) {
        tagsList.push_back(*defaultLoc);
    }

    FOREACH(tag, tagsList)
    {
        FOREACH(sFile, locList)
        {
            if (*tag == sFile->widgetLocale) {
                FOREACH(it, list)
                {
                    if (it->startFileId == sFile->startFileId) {
                        return it->src;
                    }
                }
            }
        }
    }

    return DPL::OptionalString::Null;
}

OptionalWidgetIcon getIcon(const WrtDB::DbWidgetHandle widgetHandle)
{
    using namespace LocalizationUtils;
    WidgetDAO dao(widgetHandle);

    WidgetDAO::WidgetLocalizedIconList locList = dao.getLocalizedIconList();
    WidgetDAO::WidgetIconList list = dao.getIconList();
    LanguageTagsList tagsList = LocalizationUtils::GetUserAgentLanguageTags();

    DPL::OptionalString defaultLoc = dao.getDefaultlocale();
    if (!!defaultLoc) {
        tagsList.push_back(*defaultLoc);
    }

    FOREACH(tag, tagsList)
    {
        FOREACH(icon, locList)
        {
            if (*tag == icon->widgetLocale) {
                FOREACH(it, list)
                {
                    if (it->iconId == icon->iconId) {
                        WidgetIcon ret;
                        ret.src = it->iconSrc;
                        ret.width = it->iconWidth;
                        ret.height = it->iconHeight;
                        return ret;
                    }
                }
            }
        }
    }

    return OptionalWidgetIcon::Null;
}

WidgetIconList getValidIconsList(
        WrtDB::DbWidgetHandle widgetHandle,
        const LanguageTagsList &languageTags)
{
    using namespace LocalizationUtils;
    WidgetDAO dao(widgetHandle);
    WidgetDAO::WidgetIconList list = dao.getIconList();

    WidgetIconList outlist;

    FOREACH(it, list)
    {
        LogDebug(":" << it->iconSrc);
        if (!!getFilePathInWidgetPackage(widgetHandle,
                                         languageTags,
                                         it->iconSrc))
        {
            WidgetIcon ret;
            ret.src = it->iconSrc;
            ret.width = it->iconWidth;
            ret.height = it->iconHeight;
            outlist.push_back(ret);
        }
    }
    return outlist;
}

OptionalWidgetStartFileInfo getStartFileInfo(
        WrtDB::DbWidgetHandle widgetHandle,
        const LanguageTagList &tagsList)
{
    using namespace LocalizationUtils;

    WidgetStartFileInfo info;

    WidgetDAO dao(widgetHandle);
    WidgetDAO::LocalizedStartFileList locList =
        dao.getLocalizedStartFileList();
    WidgetDAO::WidgetStartFileList list = dao.getStartFileList();

    FOREACH(tag, tagsList)
    {
        FOREACH(sFile, locList)
        {
            if (*tag == sFile->widgetLocale) {
                FOREACH(it, list)
                {
                    if (it->startFileId ==
                        sFile->startFileId) {
                        info.file = it->src;
                        info.encoding = sFile->encoding;
                        info.type = sFile->type;
                        if (tag->empty()) {
                            info.localizedPath = it->src;
                        } else {
                            info.localizedPath = L"locales/" + *tag;
                            info.localizedPath += it->src;
                        }
                        return info;
                    }
                }
            }
        }
    }

    return OptionalWidgetStartFileInfo::Null;
}

WidgetLocalizedInfo getLocalizedInfo(const WrtDB::DbWidgetHandle handle)
{
    LanguageTagList languages =
        LocalizationUtils::GetUserAgentLanguageTags();
    WidgetDAO dao(handle);
    DPL::OptionalString dl = dao.getDefaultlocale();
    if (!!dl) {
        languages.push_back(*dl);
    }

    WidgetLocalizedInfo result;
    FOREACH(i, languages)
    {
        WidgetLocalizedInfo languageResult = dao.getLocalizedInfo(*i);

#define OVERWRITE_IF_NULL(FIELD) if (!result.FIELD) { \
        result.FIELD = languageResult.FIELD; \
}

        OVERWRITE_IF_NULL(name);
        OVERWRITE_IF_NULL(shortName);
        OVERWRITE_IF_NULL(description);
        OVERWRITE_IF_NULL(license);
        OVERWRITE_IF_NULL(licenseHref);

#undef OVERWRITE_IF_NULL
    }

    return result;
}
}
