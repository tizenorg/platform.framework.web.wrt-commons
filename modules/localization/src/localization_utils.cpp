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
 * @file    localization_utils.cpp
 * @author  Bartosz Janiak (b.janiak@samsung.com)
 * @version 1.0
 */

#include <dpl/localization/localization_utils.h>

#include <appcore-efl.h>
#include <vconf.h>
#include <dpl/framework_efl.h>

#include <dpl/foreach.h>
#include <dpl/mutex.h>
#include <dpl/wrt-dao-rw/widget_dao.h>

namespace {

static int LanguageChanged(void *)
{
    char* lang = vconf_get_str(VCONFKEY_LANGSET);
    if (!lang) {
        LogError("Cannot get locale settings from vconf");
        return 0;
    }
    LogDebug("Language set to: " << lang);

    using namespace LocalizationUtils;

    LanguageTagsList list;
    list.push_back(DPL::FromUTF8String(lang));
    SetSystemLanguageTags(list);

    LogDebug("LanguageChanged to " << lang);

    return 0;
}
}

namespace LocalizationUtils {
static LanguageTagsList m_systemLanguageTags;
static LanguageTagsList m_userLanguageTags;
static LanguageTagsList m_languageTags;
static DPL::ReadWriteMutex m_readWriteMutex;

template<typename StringType>
void FindAndReplace(StringType& source,
        const StringType& find,
        const StringType& replace)
{
    size_t pos = 0;
    while ((pos = source.find(find, pos)) != StringType::npos) {
        source.replace(pos, find.length(), replace);
        pos += replace.length();
    }
}

DPL::String BCP47LanguageTagToLocale(const DPL::String& inLanguageTag)
{
    DPL::String languageTag(inLanguageTag);
    FindAndReplace(languageTag, DPL::String(L"-"), DPL::String(L"_"));
    return languageTag;
}

DPL::String LocaleToBCP47LanguageTag(const DPL::String& inLocaleString)
{
    DPL::String localeString = inLocaleString.substr(
            0,
            inLocaleString.
                find_first_of(L"."));
    FindAndReplace(localeString, DPL::String(L"_"), DPL::String(L"-"));
    return localeString;
}

void UpdateUserAgentLanguageTags()
{
    // WARNING!!!!!  This function shall be called
    // only when mutex is locked in readWriteMode!

    LanguageTagsList list = m_userLanguageTags;
    list.insert(list.begin(),
                m_systemLanguageTags.begin(),
                m_systemLanguageTags.end());
    m_languageTags.clear();

    FOREACH(i, list) {
        DPL::String tag = LocaleToBCP47LanguageTag(*i);
        while (true) { //W3C Packaging 9. Step 5. 2. D
            if (tag.empty()) { continue; }

            LogDebug("Adding user locale '" << tag << "'");
            m_languageTags.push_back(tag);

            size_t subtagPos = tag.find_last_of(L"-");
            if (subtagPos == DPL::String::npos) {
                break;
            }
            tag = tag.substr(0, subtagPos);
        }
    }

    m_languageTags.push_back(L"en");
    m_languageTags.push_back(L"");
}

void SetUserLanguageTags(const LanguageTagsList& tags)
{
    DPL::ReadWriteMutex::ScopedWriteLock lock(&m_readWriteMutex);
    if (m_userLanguageTags != tags) {
        m_userLanguageTags = tags;
        UpdateUserAgentLanguageTags();
    }
}

void SetSystemLanguageTags(const LanguageTagsList& tags)
{
    DPL::ReadWriteMutex::ScopedWriteLock lock(&m_readWriteMutex);
    if (m_systemLanguageTags != tags) {
        m_systemLanguageTags = tags;
        UpdateUserAgentLanguageTags();
    }
}

LanguageTagsList GetUserAgentLanguageTags()
{
    DPL::ReadWriteMutex::ScopedReadLock lock(&m_readWriteMutex);
    return m_languageTags;
}

void Initialize()
{
    appcore_set_event_callback(
        APPCORE_EVENT_LANG_CHANGE,
        &LanguageChanged,
        NULL);

    LanguageChanged(NULL); // updating language for the first time
}

}
