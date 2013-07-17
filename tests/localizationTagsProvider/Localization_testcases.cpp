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
/**
 * @file    Localization_testcases.cpp
 * @author  Marcin Kaminski (marcin.ka@samsung.com)
 * @version 1.0
 * @brief   This file contains tests for localization related code.
 */

#include <dpl/log/log.h>
#include <dpl/test/test_runner.h>
#include <vconf.h>

#include <LanguageTagsProvider.h>

RUNNER_TEST_GROUP_INIT(LanguageTagsProvider)

RUNNER_TEST(tagsFromSystemLocales)
{
    LogDebug("Generating tags from system locales");

    char* currlocals = vconf_get_str(VCONFKEY_LANGSET);
    LogDebug("Locales fetched from system settings: " << currlocals);
    RUNNER_ASSERT_MSG(!!currlocals, "NULL locales received from system");
    int result = vconf_set_str(VCONFKEY_LANGSET, "en_US.UTF-8");
    LogDebug("Returned vconf set execution status: " << result);
    RUNNER_ASSERT_MSG(result == 0, "Invalid value returned by vconf_set_str on setting locales");

    /* Ensure that system locales where fetched */
    LanguageTagsProviderSingleton::Instance().resetLanguageTags();
    LogDebug("Language tags set based on current system locales");

    LanguageTags ltlist = LanguageTagsProviderSingleton::Instance().getLanguageTags();
    RUNNER_ASSERT_MSG(!ltlist.empty(), "Empty tag list returned");

    /* Correct list generated from given locales should contain: "en-US", "en" and "" */
    LanguageTags correct;
    correct.push_back(L"en-US");
    correct.push_back(L"en");
    correct.push_back(L"");

    RUNNER_ASSERT_MSG( correct==ltlist, "Received and expected language tags lists differ");

    /* Restore system locales */
    result = vconf_set_str(VCONFKEY_LANGSET, currlocals);
    RUNNER_ASSERT_MSG(result == 0, "Invalid value returned by vconf_set_str on restoring locales");
    LogDebug("System locales restored");
}

RUNNER_TEST(tagsFromGivenLocales)
{
    LogDebug("Generating tags from given locales");

    const char *locales1 = "it_IT.UTF-8", *locales2="en_GB";

    LogDebug("Using locales with codepage: " << locales1);
    LanguageTagsProviderSingleton::Instance().setLanguageTagsFromLocales(locales1);
    LanguageTags ltlist = LanguageTagsProviderSingleton::Instance().getLanguageTags();
    /* Correct list generated from given locales should contain: "it-IT", "it" and
     * two default values: "en" and "" */
    LanguageTags correct;
    correct.push_back(L"it-IT");
    correct.push_back(L"it");
    correct.push_back(L"");
    RUNNER_ASSERT_MSG(correct==ltlist, "Received and expected language tags lists differ");

    LogDebug("Using locales without codepage: " << locales2);
    LanguageTagsProviderSingleton::Instance().setLanguageTagsFromLocales(locales2);
    ltlist = LanguageTagsProviderSingleton::Instance().getLanguageTags();
    correct.clear();
    correct.push_back(L"en-GB");
    correct.push_back(L"en");
    correct.push_back(L"");
    RUNNER_ASSERT_MSG(correct==ltlist, "Received and expected language tags lists differ");
}

RUNNER_TEST(tagsFromNullLocales)
{
    LogDebug("Generating tags when NULL locales given");

    LanguageTagsProviderSingleton::Instance().setLanguageTagsFromLocales(NULL);
    LanguageTags ltlist = LanguageTagsProviderSingleton::Instance().getLanguageTags();
    /* List with two values "en" and "" should be returned */
    LanguageTags correct;
    correct.push_back(L"");
    RUNNER_ASSERT_MSG(correct==ltlist, "Received and expected language tags lists differ");
}


RUNNER_TEST(tagsFromGivenTagList)
{
    LogDebug("Copying given tags list");

    LogDebug("Correct full list (with default values)");
    LanguageTags correct;
    correct.push_back(L"de-DE");
    correct.push_back(L"de");
    correct.push_back(L"");
    LanguageTagsProviderSingleton::Instance().setLanguageTags(correct);
    LanguageTags result = LanguageTagsProviderSingleton::Instance().getLanguageTags();
    RUNNER_ASSERT_MSG(correct==result, "Received and expected language tags lists differ");

    LogDebug("Tags list without default values)");
    LanguageTags nondef;
    nondef.push_back(L"de-DE");
    nondef.push_back(L"de");
    LanguageTagsProviderSingleton::Instance().setLanguageTags(correct);
    result = LanguageTagsProviderSingleton::Instance().getLanguageTags();

    /* Received list should contains elements from input list with default
     * values added (as "correct" list has) */
    RUNNER_ASSERT_MSG(!result.empty(), "Empty tags list should never be returned");
    RUNNER_ASSERT_MSG(nondef!=result, "Received list is same as given incomplete one");
    RUNNER_ASSERT_MSG(correct==result, "Received and expected language tags lists differ");
}

RUNNER_TEST(tagsFromEmptyList)
{
    LogDebug("Generating tags when empty tag list given");

    LanguageTags input;
    LanguageTagsProviderSingleton::Instance().setLanguageTags(input);
    LanguageTags result = LanguageTagsProviderSingleton::Instance().getLanguageTags();
    RUNNER_ASSERT_MSG(!result.empty(), "Empty tags list should never be returned");
}

RUNNER_TEST(defaultWidgetLocale)
{
    LogDebug("Adding default widget locales to language tags list");

    LanguageTags input;
    input.push_back(L"de-DE");
    input.push_back(L"de");
    input.push_back(L"");
    LanguageTagsProviderSingleton::Instance().setLanguageTags(input);
    LanguageTagsProviderSingleton::Instance().addWidgetDefaultLocales(L"it");
    LanguageTags result = LanguageTagsProviderSingleton::Instance().getLanguageTags();
    RUNNER_ASSERT_MSG(result.size() == 4, "4 different language tags expected");
    LanguageTags reference;
    reference.push_back(L"de-DE");
    reference.push_back(L"de");
    reference.push_back(L"it");
    reference.push_back(L"");
    RUNNER_ASSERT_MSG(result == reference, "Received and expected language tags lists differ");
    LanguageTagsProviderSingleton::Instance().addWidgetDefaultLocales(L"it");
    LanguageTagsProviderSingleton::Instance().addWidgetDefaultLocales(L"de-DE");
    result = LanguageTagsProviderSingleton::Instance().getLanguageTags();
    RUNNER_ASSERT_MSG(result == reference, "Adding already included tag should not change tags list");
}
