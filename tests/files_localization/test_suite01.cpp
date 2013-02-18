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
 * @file    test_suite01.cpp
 * @author  Krzysztof Jackiewicz (k.jackiewicz@samsung.com)
 * @version 1.0
 * @brief
 */

#include <dpl/log/log.h>
#include <dpl/test/test_runner.h>

//#include "mockup_include/dpl/wrt-dao-rw/widget_dao.h"
#include <dpl/wrt-dao-rw/widget_dao.h>
#include <dpl/localization/w3c_file_localization.h>

namespace {
WrtDB::LanguageTagList generateLanguageTags()
{
    WrtDB::LanguageTagList tags;
    tags.push_back(L"pl-pl");
    tags.push_back(L"en-en");
    tags.push_back(L"pl-en");
    return tags;
}

static const WrtDB::LanguageTagList languageTags = generateLanguageTags();
static const DPL::String widget1Path =
    L"/opt/share/widget/tests/localization/widget1/";
static const DPL::String widget2Path =
    L"/opt/share/widget/tests/localization/widget2/";
} // anonymous namespace

RUNNER_TEST(test01_getFilePathInWidgetPackageFromUrl){
    const int widgetHandle = 1;
    WrtDB::WidgetDAO dao(widgetHandle);
    //dao.setPath(widget1Path);

    auto result = W3CFileLocalization::getFilePathInWidgetPackageFromUrl(
            widgetHandle,
            languageTags,
            L"widget://one");

    RUNNER_ASSERT(
        *result ==
        L"/opt/share/widget/tests/localization/widget1/locales/pl-en/one");
}

RUNNER_TEST(test02_getFilePathInWidgetPackageFromUrl){
    const int widgetHandle = 2;
    WrtDB::WidgetDAO dao(widgetHandle);
    //dao.setPath(widget2Path);

    auto result = W3CFileLocalization::getFilePathInWidgetPackageFromUrl(
            widgetHandle,
            languageTags,
            L"widget://one");

    RUNNER_ASSERT(
        *result ==
        L"/opt/share/widget/tests/localization/widget2/locales/pl-en/one");
}

RUNNER_TEST(test03_getFilePathInWidgetPackageFromUrl){
    const int widgetHandle = 2;
    WrtDB::WidgetDAO dao(widgetHandle);
    //dao.setPath(widget2Path);

    auto result = W3CFileLocalization::getFilePathInWidgetPackageFromUrl(
            widgetHandle,
            languageTags,
            L"widget://two");

    RUNNER_ASSERT(
        *result ==
        L"/opt/share/widget/tests/localization/widget2/locales/en-en/two");
}

RUNNER_TEST(test04_getFilePathInWidgetPackage){
    const int widgetHandle = 1;
    WrtDB::WidgetDAO dao(widgetHandle);
    //dao.setPath(widget1Path);

    auto result = W3CFileLocalization::getFilePathInWidgetPackage(
            widgetHandle,
            languageTags,
            L"one");

    RUNNER_ASSERT(*result == L"locales/pl-en/one");
}

RUNNER_TEST(test05_getFilePathInWidgetPackage){
    const int widgetHandle = 2;
    WrtDB::WidgetDAO dao(widgetHandle);
    //dao.setPath(widget2Path);

    auto result = W3CFileLocalization::getFilePathInWidgetPackage(
            widgetHandle,
            languageTags,
            L"two");

    RUNNER_ASSERT(*result == L"locales/en-en/two");
}

