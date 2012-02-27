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
 * @file        Interfaces.h
 * @author      Bartlomiej Grzelewski (b.grzelewski@samsung.com)
 * @version     1.0
 * @brief       Instantion of PEP used in tests.
 */
#ifndef _TEST_ACE_TEST_PEPSINGLETON_H_
#define _TEST_ACE_TEST_PEPSINGLETON_H_

#include <dpl/singleton.h>
#include <dpl/ace/PolicyEnforcementPoint.h>

typedef DPL::Singleton<PolicyEnforcementPoint> PEPSingleton;

#endif // _TEST_ACE_TEST_PEPSINGLETON_H_

