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
#include <dpl/static_block.h>
#include <dpl/wrt-dao-ro/widget_dao_read_only.h>
#include <dpl/localization/w3c_file_localization.h>
#include <LanguageTagsProvider.h>

namespace {

STATIC_BLOCK
{
    WrtDB::LanguageTagList tags;
    tags.push_back(L"pl-pl");
    tags.push_back(L"en-en");
    tags.push_back(L"pl-en");
    LanguageTagsProviderSingleton::Instance().setLanguageTags(tags);
}

static const DPL::String widget1Path =
    L"/opt/share/widget/tests/localization/widget1/";
static const DPL::String widget2Path =
    L"/opt/share/widget/tests/localization/widget2/";
} // anonymous namespace

RUNNER_TEST(test01_getFilePathInWidgetPackageFromUrl){
    WrtDB::TizenAppId name = L"tizenid201"; //no difference if it is valid or invalid appId/pkgId, we fill database which has no intergrity constrainst
    WrtDB::WidgetDAOReadOnly dao(name);

    DPL::Optional<DPL::String> result = W3CFileLocalization::getFilePathInWidgetPackageFromUrl(
            name,
            DPL::String(L"widget://one"));

    RUNNER_ASSERT_MSG(!!result, "No result");
    RUNNER_ASSERT(
        *result ==
        L"/opt/share/widget/tests/localization/widget1/res/wgt/locales/pl-en/one");
}

RUNNER_TEST(test02_getFilePathInWidgetPackageFromUrl){
    WrtDB::TizenAppId name = L"tizenid202";
    WrtDB::WidgetDAOReadOnly dao(name);

    DPL::Optional<DPL::String> result = W3CFileLocalization::getFilePathInWidgetPackageFromUrl(
            name,
            DPL::String(L"widget://one"));

    RUNNER_ASSERT_MSG(!!result, "No result");
    RUNNER_ASSERT(
        *result ==
        L"/opt/share/widget/tests/localization/widget2/res/wgt/locales/pl-en/one");
}

RUNNER_TEST(test03_getFilePathInWidgetPackageFromUrl){
    WrtDB::TizenAppId name = L"tizenid202";
    WrtDB::WidgetDAOReadOnly dao(name);

    DPL::Optional<DPL::String> result = W3CFileLocalization::getFilePathInWidgetPackageFromUrl(
            name,
            DPL::String(L"widget://two"));

    RUNNER_ASSERT_MSG(!!result, "No result");
    RUNNER_ASSERT(
        *result ==
        L"/opt/share/widget/tests/localization/widget2/res/wgt/locales/en-en/two");
}

RUNNER_TEST(test04_getFilePathInWidgetPackage){
    WrtDB::TizenAppId name = L"tizenid201";
    WrtDB::WidgetDAOReadOnly dao(name);

    DPL::Optional<DPL::String> result = W3CFileLocalization::getFilePathInWidgetPackage(
            name,
            DPL::String(L"one"));

    RUNNER_ASSERT_MSG(!!result, "No result");
    RUNNER_ASSERT(*result == L"locales/pl-en/one");
}

RUNNER_TEST(test05_getFilePathInWidgetPackage){
    WrtDB::TizenAppId name = L"tizenid202";
    WrtDB::WidgetDAOReadOnly dao(name);

    DPL::Optional<DPL::String> result = W3CFileLocalization::getFilePathInWidgetPackage(
            name,
            DPL::String(L"two"));

    RUNNER_ASSERT_MSG(!!result, "No result");
    RUNNER_ASSERT(*result == L"locales/en-en/two");
}

