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
#include <stddef.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <assert.h>

#include <dpl/localization/w3c_file_localization.h>

#include <dpl/wrt-dao-ro/widget_dao_read_only.h>
#include <dpl/localization/localization_utils.h>

#include <dpl/log/log.h>
#include <dpl/string.h>
#include <dpl/optional.h>
#include <dpl/foreach.h>

#include <LanguageTagsProvider.h>

using namespace WrtDB;

namespace {
const DPL::String FILE_URI_BEGIN = L"file://";
const DPL::String WIDGET_URI_BEGIN = L"widget://";
const DPL::String APP_URI_BEGIN = L"app://";
const DPL::String LOCALE_PREFIX = L"locales/";

DPL::Optional<std::string> GetFilePathInWidgetPackageInternal(
    const std::string& basePath,
    std::string filePath)
{
    LogDebug("Looking for file: " << filePath << "  in: " << basePath);

    const LanguageTags& ltags =
        LanguageTagsProviderSingleton::Instance().getLanguageTags();

    //Check if string isn't empty
    if (filePath.size() == 0) {
        return DPL::Optional<std::string>::Null;
    }
    //Removing preceding '/'
    if (filePath[0] == '/') {
        filePath.erase(0, 1);
    }
    // In some cases (start file localization) url has unnecessary "/" at the
    // end
    if (filePath[filePath.size() - 1] == '/') {
        filePath.erase(filePath.size() - 1, 1);
    }
    //Check if string isn't empty
    if (filePath.size() == 0) {
        return DPL::Optional<std::string>::Null;
    }

    LogDebug("locales size = " << ltags.size());
    for (LanguageTags::const_iterator it = ltags.begin();
         it != ltags.end();
         ++it)
    {
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
    const DPL::String& basePath,
    const DPL::String& filePath)
{
    DPL::Optional<std::string> path =
        GetFilePathInWidgetPackageInternal(DPL::ToUTF8String(basePath),
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
    const DPL::String &url)
{
    return getFilePathInWidgetPackageFromUrl(
               WidgetDAOReadOnlyPtr(new WidgetDAOReadOnly(widgetHandle)),
               url);
}

DPL::Optional<DPL::String> getFilePathInWidgetPackageFromUrl(
    const WrtDB::WidgetPkgName &pkgname,
    const DPL::String &url)
{
    return getFilePathInWidgetPackageFromUrl(
               WidgetDAOReadOnlyPtr(new WidgetDAOReadOnly(pkgname)),
               url);
}

DPL::Optional<DPL::String> getFilePathInWidgetPackageFromUrl(
    WrtDB::WidgetDAOReadOnlyPtr dao,
    const DPL::String &url)
{
    DPL::String req = url;

    DPL::String::size_type pos = req.find_first_of('#');
    if(pos != DPL::String::npos)
    {
        req.resize(pos); //truncate fragment identifier
    }

    pos = req.find_first_of('?');
    if(pos != DPL::String::npos)
    {
        req.resize(pos); //truncate query string
    }

    if (req.find(WIDGET_URI_BEGIN) == 0) {
        req.erase(0, WIDGET_URI_BEGIN.length());
    } else if (req.find(FILE_URI_BEGIN) == 0) {
        req.erase(0, FILE_URI_BEGIN.length());
        if (req.find(dao->getPath()) == 0) {
            req.erase(0, dao->getPath().length());
        }
        if (req.find(LOCALE_PREFIX) == 0) {
            req.erase(0, LOCALE_PREFIX.length());
            size_t position = req.find('/');
            // should always be >0 as correct locales path is
            // always locales/xx/ or locales/xx-XX/
            if (position != std::string::npos && position > 0) {
                req.erase(0, position + 1);
            }
        }
    } else if(req.find(APP_URI_BEGIN) == 0) {
        req.erase(0, APP_URI_BEGIN.length());
        DPL::String id = *dao->getTizenAppId();
        if(req.substr(0, id.size()) != id)
        {
            LogError("Tizen id does not match, ignoring");
            return DPL::Optional<DPL::String>::Null;
        }
        req.erase(0, id.length());
    } else {
        LogDebug("Unknown path format, ignoring");
        return DPL::Optional<DPL::String>::Null;
    }

    auto widgetPath = dao->getPath();

    LogDebug("Required path: " << req);
    DPL::Optional<DPL::String> found =
        GetFilePathInWidgetPackageInternal(widgetPath, req);

    if (!found) {
        LogError("Path not found within current locale in current widget");
        return DPL::Optional<DPL::String>::Null;
    }

    found = widgetPath + *found;

    return found;
}

DPL::Optional<DPL::String> getFilePathInWidgetPackage(
    WrtDB::DbWidgetHandle widgetHandle,
    const DPL::String& file)
{
    return getFilePathInWidgetPackage(
               WidgetDAOReadOnlyPtr(new WidgetDAOReadOnly(widgetHandle)),
               file);
}

DPL::Optional<DPL::String> getFilePathInWidgetPackage(
    const WrtDB::WidgetPkgName &pkgname,
    const DPL::String& file)
{
    return getFilePathInWidgetPackage(
               WidgetDAOReadOnlyPtr(new WidgetDAOReadOnly(pkgname)),
               file);
}

DPL::Optional<DPL::String> getFilePathInWidgetPackage(
    WrtDB::WidgetDAOReadOnlyPtr dao,
    const DPL::String& file)
{
    return GetFilePathInWidgetPackageInternal(dao->getPath(), file);
}

DPL::OptionalString getStartFile(const WrtDB::WidgetPkgName & pkgname)
{
    return getStartFile(WidgetDAOReadOnlyPtr(new WidgetDAOReadOnly(pkgname)));
}

DPL::OptionalString getStartFile(const WrtDB::DbWidgetHandle handle)
{
    return getStartFile(WidgetDAOReadOnlyPtr(new WidgetDAOReadOnly(handle)));
}

DPL::OptionalString getStartFile(WrtDB::WidgetDAOReadOnlyPtr dao)
{
    WidgetDAOReadOnly::LocalizedStartFileList locList =
        dao->getLocalizedStartFileList();
    WidgetDAOReadOnly::WidgetStartFileList list = dao->getStartFileList();
    LanguageTags tagsList =
        LanguageTagsProviderSingleton::Instance().getLanguageTags();

    DPL::OptionalString defaultLoc = dao->getDefaultlocale();
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

OptionalWidgetIcon getIcon(const WrtDB::WidgetPkgName & pkgname)
{
    return getIcon(WidgetDAOReadOnlyPtr(new WidgetDAOReadOnly(pkgname)));
}

OptionalWidgetIcon getIcon(WrtDB::DbWidgetHandle widgetHandle)
{
    return getIcon(WidgetDAOReadOnlyPtr(new WidgetDAOReadOnly(widgetHandle)));
}

OptionalWidgetIcon getIcon(WrtDB::WidgetDAOReadOnlyPtr dao)
{
    WidgetDAOReadOnly::WidgetLocalizedIconList locList =
        dao->getLocalizedIconList();
    WidgetDAOReadOnly::WidgetIconList list = dao->getIconList();
    LanguageTags tagsList =
        LanguageTagsProviderSingleton::Instance().getLanguageTags();

    DPL::OptionalString defaultLoc = dao->getDefaultlocale();
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

WidgetIconList getValidIconsList(WrtDB::DbWidgetHandle widgetHandle)
{
    return getValidIconsList(
               WidgetDAOReadOnlyPtr(new WidgetDAOReadOnly(widgetHandle)));
}

WidgetIconList getValidIconsList(const WrtDB::WidgetPkgName &pkgname)
{
    return getValidIconsList(
               WidgetDAOReadOnlyPtr(new WidgetDAOReadOnly(pkgname)));
}

WidgetIconList getValidIconsList(WrtDB::WidgetDAOReadOnlyPtr dao)
{
    WidgetDAOReadOnly::WidgetIconList list = dao->getIconList();

    WidgetIconList outlist;

    FOREACH(it, list)
    {
        LogDebug(":" << it->iconSrc);
        if (!!getFilePathInWidgetPackage(dao->getHandle(),
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

OptionalWidgetStartFileInfo getStartFileInfo(WrtDB::DbWidgetHandle widgetHandle)
{
    return getStartFileInfo(
               WidgetDAOReadOnlyPtr(new WidgetDAOReadOnly(widgetHandle)));
}

OptionalWidgetStartFileInfo getStartFileInfo(
    const WrtDB::WidgetPkgName &pkgname)
{
    return getStartFileInfo(
               WidgetDAOReadOnlyPtr(new WidgetDAOReadOnly(pkgname)));
}

OptionalWidgetStartFileInfo getStartFileInfo(WrtDB::WidgetDAOReadOnlyPtr dao)
{
    WidgetStartFileInfo info;

    WidgetDAOReadOnly::LocalizedStartFileList locList =
        dao->getLocalizedStartFileList();
    WidgetDAOReadOnly::WidgetStartFileList list = dao->getStartFileList();
    const LanguageTags tagsList =
        LanguageTagsProviderSingleton::Instance().getLanguageTags();

    FOREACH(tag, tagsList)
    {
        FOREACH(sFile, locList)
        {
            if (*tag == sFile->widgetLocale) {
                FOREACH(it, list)
                {
                    if (it->startFileId ==
                        sFile->startFileId)
                    {
                        info.file = it->src;
                        info.encoding = sFile->encoding;
                        info.type = sFile->type;
                        if (tag->empty()) {
                            info.localizedPath = it->src;
                        } else {
                            info.localizedPath = L"locales/" + *tag + L"/";
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
    return getLocalizedInfo(WidgetDAOReadOnlyPtr(new WidgetDAOReadOnly(handle)));
}

WidgetLocalizedInfo getLocalizedInfo(const WrtDB::WidgetPkgName & pkgname)
{
    return getLocalizedInfo(WidgetDAOReadOnlyPtr(new WidgetDAOReadOnly(pkgname)));
}

WidgetLocalizedInfo getLocalizedInfo(WidgetDAOReadOnlyPtr dao)
{
    LanguageTags languages =
        LanguageTagsProviderSingleton::Instance().getLanguageTags();
    DPL::OptionalString dl = dao->getDefaultlocale();
    if (!!dl) {
        languages.push_back(*dl);
    }

    WidgetLocalizedInfo result;
    FOREACH(i, languages)
    {
        WidgetLocalizedInfo languageResult = dao->getLocalizedInfo(*i);

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
