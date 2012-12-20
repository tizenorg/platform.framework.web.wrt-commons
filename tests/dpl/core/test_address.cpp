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
 * @file        test_address.cpp
 * @author      Tomasz Swierczek (t.swierczek@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of test address
 */
#include <dpl/test/test_runner.h>
#include <dpl/address.h>

RUNNER_TEST_GROUP_INIT(DPL)

RUNNER_TEST(Address_InitialEmpty)
{
    DPL::Address address;
    RUNNER_ASSERT(address.ToString() == ":0");
}

RUNNER_TEST(Address_InitialAddress)
{
    DPL::Address address("www.sample.com");
    RUNNER_ASSERT(address.ToString() == "www.sample.com:0");
}

RUNNER_TEST(Address_InitialAddressPort)
{
    DPL::Address address("www.somewhere.com", 8080);
    RUNNER_ASSERT(address.ToString() == "www.somewhere.com:8080");
}

RUNNER_TEST(Address_Getters)
{
    DPL::Address address("www.somewhere.com", 8080);
    RUNNER_ASSERT(address.GetAddress() == "www.somewhere.com");
    RUNNER_ASSERT(address.GetPort() == 8080);
}
