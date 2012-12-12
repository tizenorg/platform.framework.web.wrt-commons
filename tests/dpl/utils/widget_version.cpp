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
 * @file    widget_version.cpp
 * @author  Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version 1.0
 * @brief   Implementation file for test cases for engine internal tests
 */
#include <dpl/test/test_runner.h>
#include <dpl/utils/widget_version.h>

RUNNER_TEST_GROUP_INIT(DPL_WIDGET_VERSION)

RUNNER_TEST(WidgetVersion_M2_O0)
{
    DPL::String raw(L"1.2");
    WidgetVersion version(raw);

    RUNNER_ASSERT(version.IsWac() == true);
    RUNNER_ASSERT(version.Major() == DPL::String(L"1"));
    RUNNER_ASSERT(version.Minor() == DPL::String(L"2"));
    RUNNER_ASSERT(version.Micro() == DPL::Optional<DPL::String>());
    RUNNER_ASSERT(version.Optional() == DPL::Optional<DPL::String>());
    RUNNER_ASSERT(version.Raw() == raw);
}

RUNNER_TEST(WidgetVersion_M2_O0_nonwac_1)
{
    DPL::String raw(L"a1.2");
    WidgetVersion version(raw);

    RUNNER_ASSERT(version.IsWac() == false);
    RUNNER_ASSERT(version.Raw() == raw);
}

RUNNER_TEST(WidgetVersion_M2_O0_nonwac_2)
{
    DPL::String raw(L"1.2a");
    WidgetVersion version(raw);

    RUNNER_ASSERT(version.IsWac() == false);
    RUNNER_ASSERT(version.Raw() == raw);
}

RUNNER_TEST(WidgetVersion_M2_O0_nonwac_3)
{
    DPL::String raw(L"aaa1.2bbb");
    WidgetVersion version(raw);

    RUNNER_ASSERT(version.IsWac() == false);
    RUNNER_ASSERT(version.Raw() == raw);
}

RUNNER_TEST(WidgetVersion_M2_O0_nonwac_4)
{
    DPL::String raw(L"1a.a2");
    WidgetVersion version(raw);

    RUNNER_ASSERT(version.IsWac() == false);
    RUNNER_ASSERT(version.Raw() == raw);
}

RUNNER_TEST(WidgetVersion_M2_O0_long)
{
    DPL::String raw(L"123456789012345678901234567890.98765432109876543210987654321");
    WidgetVersion version(raw);

    RUNNER_ASSERT(version.IsWac() == true);
    RUNNER_ASSERT(version.Major() == DPL::String(L"123456789012345678901234567890"));
    RUNNER_ASSERT(version.Minor() == DPL::String(L"98765432109876543210987654321"));
    RUNNER_ASSERT(version.Micro() == DPL::Optional<DPL::String>());
    RUNNER_ASSERT(version.Optional() == DPL::Optional<DPL::String>());
    RUNNER_ASSERT(version.Raw() == raw);
}

RUNNER_TEST(WidgetVersion_M3_O0)
{
    DPL::String raw(L"1.2.3");
    WidgetVersion version(raw);

    RUNNER_ASSERT(version.IsWac() == true);
    RUNNER_ASSERT(version.Major() == DPL::String(L"1"));
    RUNNER_ASSERT(version.Minor() == DPL::String(L"2"));
    RUNNER_ASSERT(version.Micro() == DPL::Optional<DPL::String>(L"3"));
    RUNNER_ASSERT(version.Optional() == DPL::Optional<DPL::String>());
    RUNNER_ASSERT(version.Raw() == raw);
}

RUNNER_TEST(WidgetVersion_M3_O0_nonwac_1)
{
    DPL::String raw(L"a1a.2.3");
    WidgetVersion version(raw);

    RUNNER_ASSERT(version.IsWac() == false);
    RUNNER_ASSERT(version.Raw() == raw);
}

RUNNER_TEST(WidgetVersion_M3_O0_nonwac_2)
{
    DPL::String raw(L"1.b2.3");
    WidgetVersion version(raw);

    RUNNER_ASSERT(version.IsWac() == false);
    RUNNER_ASSERT(version.Raw() == raw);
}

RUNNER_TEST(WidgetVersion_M3_O0_nonwac_3)
{
    DPL::String raw(L"1.2.3c");
    WidgetVersion version(raw);

    RUNNER_ASSERT(version.IsWac() == false);
    RUNNER_ASSERT(version.Raw() == raw);
}

RUNNER_TEST(WidgetVersion_M3_O0_nonwac_4)
{
    DPL::String raw(L"1.2.3a");
    WidgetVersion version(raw);

    RUNNER_ASSERT(version.IsWac() == false);
    RUNNER_ASSERT(version.Raw() == raw);
}

RUNNER_TEST(WidgetVersion_M3_O1_1)
{
    DPL::String raw(L"1.2.3 test111");
    WidgetVersion version(raw);

    RUNNER_ASSERT(version.IsWac() == true);
    RUNNER_ASSERT(version.Major() == DPL::String(L"1"));
    RUNNER_ASSERT(version.Minor() == DPL::String(L"2"));
    RUNNER_ASSERT(version.Micro() == DPL::Optional<DPL::String>(L"3"));
    RUNNER_ASSERT(version.Optional() == DPL::Optional<DPL::String>(L"test111"));
    RUNNER_ASSERT(version.Raw() == raw);
}

RUNNER_TEST(WidgetVersion_M3_O1_2)
{
    DPL::String raw(L"1.2.3 111");
    WidgetVersion version(raw);

    RUNNER_ASSERT(version.IsWac() == true);
    RUNNER_ASSERT(version.Major() == DPL::String(L"1"));
    RUNNER_ASSERT(version.Minor() == DPL::String(L"2"));
    RUNNER_ASSERT(version.Micro() == DPL::Optional<DPL::String>(L"3"));
    RUNNER_ASSERT(version.Optional() == DPL::Optional<DPL::String>(L"111"));
    RUNNER_ASSERT(version.Raw() == raw);
}

RUNNER_TEST(WidgetVersion_M3_O1_3)
{
    DPL::String raw(L"1.2.3 ");
    WidgetVersion version(raw);

    RUNNER_ASSERT(version.IsWac() == false);
    RUNNER_ASSERT(version.Raw() == raw);
}

RUNNER_TEST(WidgetVersion_M2_O1_1)
{
    DPL::String raw(L"1.2 t");
    WidgetVersion version(raw);

    RUNNER_ASSERT(version.IsWac() == true);
    RUNNER_ASSERT(version.Major() == DPL::String(L"1"));
    RUNNER_ASSERT(version.Minor() == DPL::String(L"2"));
    RUNNER_ASSERT(version.Micro() == DPL::Optional<DPL::String>());
    RUNNER_ASSERT(version.Optional() == DPL::Optional<DPL::String>(L"t"));
    RUNNER_ASSERT(version.Raw() == raw);
}

RUNNER_TEST(WidgetVersion_Strange_0)
{
    DPL::String raw(L"1");
    WidgetVersion version(raw);

    RUNNER_ASSERT(version.IsWac() == false);
    RUNNER_ASSERT(version.Raw() == raw);
}

RUNNER_TEST(WidgetVersion_Strange_1)
{
    DPL::String raw(L".1");
    WidgetVersion version(raw);

    RUNNER_ASSERT(version.IsWac() == false);
    RUNNER_ASSERT(version.Raw() == raw);
}

RUNNER_TEST(WidgetVersion_Strange_2)
{
    DPL::String raw(L"..1");
    WidgetVersion version(raw);

    RUNNER_ASSERT(version.IsWac() == false);
    RUNNER_ASSERT(version.Raw() == raw);
}

RUNNER_TEST(WidgetVersion_Strange_3)
{
    DPL::String raw(L"...1");
    WidgetVersion version(raw);

    RUNNER_ASSERT(version.IsWac() == false);
    RUNNER_ASSERT(version.Raw() == raw);
}

RUNNER_TEST(WidgetVersion_Strange_4)
{
    DPL::String raw(L"qwerty");
    WidgetVersion version(raw);

    RUNNER_ASSERT(version.IsWac() == false);
    RUNNER_ASSERT(version.Raw() == raw);
}

RUNNER_TEST(WidgetVersion_Strange_5)
{
    DPL::String raw(L"!@#$%^&*()_+   ^&%^*&%$^*&%*()&   JHKJLHKJLH   685685687");
    WidgetVersion version(raw);

    RUNNER_ASSERT(version.IsWac() == false);
    RUNNER_ASSERT(version.Raw() == raw);
}

RUNNER_TEST(WidgetVersion_Compare_0)
{
    RUNNER_ASSERT(WidgetVersion(L"1.1") < WidgetVersion(L"1.2"));
}

RUNNER_TEST(WidgetVersion_Compare_1)
{
    RUNNER_ASSERT(WidgetVersion(L"01.001") < WidgetVersion(L"0001.002"));
}

RUNNER_TEST(WidgetVersion_Compare_2)
{
    RUNNER_ASSERT(WidgetVersion(L"0.1") == WidgetVersion(L"00.1"));
}

RUNNER_TEST(WidgetVersion_Compare_3)
{
    RUNNER_ASSERT(WidgetVersion(L"1.00000000000000") >= WidgetVersion(L"1.0 test"));
}

RUNNER_TEST(WidgetVersion_Compare_4)
{
    RUNNER_ASSERT(WidgetVersion(L"19647963733338932479072098437089778943732432.00000000000000004324324324324321") > WidgetVersion(L"4324324324324324324321.000432"));
}

RUNNER_TEST(WidgetVersion_Compare_5)
{
    RUNNER_ASSERT(WidgetVersion(L"12345.1") == WidgetVersion(L"12345.1"));
}

RUNNER_TEST(WidgetVersion_Compare_6)
{
    RUNNER_ASSERT(WidgetVersion(L"1.1") != WidgetVersion(L"1.11"));
}

RUNNER_TEST(WidgetVersion_Compare_7)
{
    RUNNER_ASSERT(WidgetVersion(L"000123000.0 notatest") == WidgetVersion(L"00123000.0 testtesttest"));
}
