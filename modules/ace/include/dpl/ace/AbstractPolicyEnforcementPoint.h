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

#ifndef WRT_SRC_ACCESS_CONTROL_LOGIC_ABSTRACT_POLICY_ENFORCEMENT_POINTS_H
#define WRT_SRC_ACCESS_CONTROL_LOGIC_ABSTRACT_POLICY_ENFORCEMENT_POINTS_H

#include <dpl/ace/WRT_INTERFACE.h>
#include <dpl/event/inter_context_delegate.h>
#include <dpl/ace/PolicyResult.h>

class AbstractPolicyEnforcementPoint
{
  public:
    typedef DPL::Event::ICDelegate<PolicyResult> ResponseReceiver;
    virtual PolicyResult check(Request &request) = 0;
};

#endif /* WRT_SRC_ACCESS_CONTROL_LOGIC_ABSTRACT_POLICY_ENFORCEMENT_POINTS_H */
