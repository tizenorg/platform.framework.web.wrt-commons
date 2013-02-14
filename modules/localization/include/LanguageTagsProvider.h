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
 * @file    LanguageTagsProvider.h
 * @author  Marcin Kaminski (marcin.ka@samsung.com)
 * @version 1.0
 */

#ifndef LANGUAGETAGSPROVIDER_H
#define LANGUAGETAGSPROVIDER_H

#include <dpl/optional.h>
#include <dpl/string.h>
#include <dpl/singleton.h>
#include <list>

typedef std::list<DPL::String> LanguageTags;

class LanguageTagsProvider
{
  public:
    /*
     * Get list of currently set language tags
     */
    const LanguageTags getLanguageTags() const;

    /*
     * Set new language tags (other than based on system locales)
     */
    void setLanguageTags(const LanguageTags& taglist);

    /*
     * Set language tags from given locales.
     * Supported format is: xx[-yy[-zz]][.encoding]
     */
    void setLanguageTagsFromLocales(const char* locales);

    /*
     * Set language tags based on system language settings
     */
    void resetLanguageTags();

    /*
     * Adds default widget locales to language tags if
     * it doesn't exist within actual tags.
     * Default locales i added:
     * - at the beginning if less then 2 tags exists on list
     * - just before empty ("") locales - pre-last position
     * - at the end if last position is not empty locale
     */
    void addWidgetDefaultLocales(const DPL::String&);

    /*
     * Function converts language tag string (i.e. en-US)
     * into locales string (en_US).
     */
    static DPL::String BCP47LanguageTagToLocale(const DPL::String&);

    /*
     * Function converts locales string (i.e. en_US.UTF-8) into language tag
     * (i.e. en-US)
     */
    static DPL::String LocaleToBCP47LanguageTag(const DPL::String&);

  private:
    friend class DPL::Singleton<LanguageTagsProvider>;

    LanguageTags m_languageTagsList;

    LanguageTagsProvider();
    virtual ~LanguageTagsProvider();

    void loadSystemTags();
    void createTagsFromLocales(const char* language);
};

typedef DPL::Singleton<LanguageTagsProvider> LanguageTagsProviderSingleton;

#endif /* LANGUAGETAGSPROVIDER_H */
