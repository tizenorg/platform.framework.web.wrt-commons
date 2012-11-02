/*
 * Copyright (c) 2012 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file    LanguageTagsProvider.cpp
 * @author  Marcin Kaminski (marcin.ka@samsung.com)
 * @version 1.0
 */
#include <stddef.h>
#include "LanguageTagsProvider.h"

#include <dpl/log/log.h>
#include <vconf.h>
#include <algorithm>
#include <iostream>
#include <cassert>
#include <dpl/singleton_impl.h>
IMPLEMENT_SINGLETON(LanguageTagsProvider)

/* ========== public ========== */
const LanguageTags LanguageTagsProvider::getLanguageTags() const
{
    return m_languageTagsList;
}

void LanguageTagsProvider::setLanguageTags(const LanguageTags& taglist)
{
    m_languageTagsList = taglist;
    /* If given list does not contain default values ("en" and empty string)
     * than append them to the list.
     * In case of empty list given as parameter only two default values
     * will exist on m_languageTagsList. */
    DPL::String tofind = L"en";
    if(std::find(m_languageTagsList.begin(), m_languageTagsList.end(),
            tofind) == m_languageTagsList.end()) {
        m_languageTagsList.push_back(L"en");
    }
    tofind = L"";
    if(std::find(m_languageTagsList.begin(), m_languageTagsList.end(),
            tofind) == m_languageTagsList.end()) {
        m_languageTagsList.push_back(L"");
    }
}

void LanguageTagsProvider::setLanguageTagsFromLocales(const char* locales)
{
    LogDebug("Setting new language tags for locales " << locales);
    this->createTagsFromLocales(locales);
}

void LanguageTagsProvider::resetLanguageTags()
{
    this->loadSystemTags();
}

DPL::String LanguageTagsProvider::BCP47LanguageTagToLocale(const DPL::String& inLanguageTag)
{
    DPL::String languageTag(inLanguageTag);
    /* Replace all */
    std::replace(languageTag.begin(), languageTag.end(), '-', '_');
    return languageTag;
}

DPL::String LanguageTagsProvider::LocaleToBCP47LanguageTag(const DPL::String& inLocaleString)
{
    /* Cut off codepage information from given string (if any exists)
     * i.e. change en_US.UTF-8 into en_US */
    DPL::String localeString = inLocaleString.substr(
            0, inLocaleString.find_first_of(L"."));
    /* Replace all '_' with '-' */
    std::replace(localeString.begin(), localeString.end(), '_', '-');
    return localeString;
}

/* ========== private ========== */
LanguageTagsProvider::LanguageTagsProvider()
{
    LogDebug("Creating LanguageTagsProvider instance");
    this->loadSystemTags();
}

LanguageTagsProvider::~LanguageTagsProvider()
{
}

void LanguageTagsProvider::loadSystemTags()
{
    char* language = vconf_get_str(VCONFKEY_LANGSET);
    if(!language) {
        LogError("Failed to get language from vconf");
    }
    else {
        LogDebug("Language fetched from vconf: " << language);
    }
    createTagsFromLocales(language);
    free(language);
}

void LanguageTagsProvider::createTagsFromLocales(const char* language)
{
    m_languageTagsList.clear();
    if(!language) {
        LogDebug("Setting default language tags");
        /* If NULL language given than set default language tags
         * and return. */
        m_languageTagsList.push_back(L"en");
        m_languageTagsList.push_back(L"");
        return;
    }

    LogDebug("Setting tags for language: " << language);
    DPL::String langdescr = LocaleToBCP47LanguageTag(DPL::FromUTF8String(language));

    size_t position;
    if(langdescr.empty()) {
        LogError("Empty language description while correct value needed");
    }
    else {
        /* Language tags list should not be cleared before this place to
         * avoid losing current data when new data are invalid */
        while (true) {
            LogDebug("Processing language description: " << langdescr);
            m_languageTagsList.push_back(langdescr);

            position = langdescr.find_last_of(L"-");
            if (position == DPL::String::npos) {
                break;
            }
            langdescr = langdescr.substr(0, position);
        }
    }
    /* If current content of langdescr (thus last added added tag)
     * is not "en" then add english as default locales.
     * "en" and empty tag are added even when langdescr is not valid. */
    if (langdescr.compare(L"en") != 0){
        m_languageTagsList.push_back(L"en");
    }
    /* Add empty tag for non-localized content */
    m_languageTagsList.push_back(L"");
}
