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
 *
 *
 * @file       AceDAOReadOnly.h
 * @author     Krzysztof Jackiewicz (k.jackiewicz@samsung.com)
 * @author     Grzegorz Krawczyk (g.krawczyk@samsung.com)
 * @version    0.1
 * @brief
 */

#ifndef ACE_DAO_READ_ONLY_H_
#define ACE_DAO_READ_ONLY_H_

#include <openssl/md5.h>
#include <dpl/string.h>
#include <dpl/exception.h>
#include <dpl/ace-dao-ro/PreferenceTypes.h>
#include <dpl/ace-dao-ro/BaseAttribute.h>
#include <dpl/ace-dao-ro/BasePermission.h>
#include <dpl/ace-dao-ro/IRequest.h>
#include <dpl/ace/PolicyEffect.h>
#include <dpl/ace/PolicyResult.h>
#include <dpl/ace/PromptDecision.h>
#include <dpl/ace-dao-ro/wrt_db_types.h>

namespace AceDB {

class AceDAOReadOnly
{
  public:
    class Exception
    {
      public:
        DECLARE_EXCEPTION_TYPE(DPL::Exception, Base)
        DECLARE_EXCEPTION_TYPE(Base, DatabaseError)
    };

    AceDAOReadOnly() {}

    static void attachToThread();
    static void detachFromThread();

    // policy effect/decision
    static OptionalPolicyResult getPolicyResult(
            const BaseAttributeSet &attributes);

    static OptionalPolicyResult getPolicyResult(
        const DPL::String &attrHash);

    // prompt decision
    static OptionalCachedPromptDecision getPromptDecision(
            const DPL::String &hash,
            const std::string &userParam);
    static OptionalCachedPromptDecision getPromptDecision(
            const BaseAttributeSet &attribites,
            const std::string &userParam);

    // resource settings
    static PreferenceTypes getDevCapSetting(const std::string &resource);
    static void getDevCapSettings(PreferenceTypesMap *preferences);

    // user settings
    static void getWidgetDevCapSettings(BasePermissionList *permissions);
    static PreferenceTypes getWidgetDevCapSetting(
            const std::string &resource,
            WidgetHandle handler);

    static void getAttributes(BaseAttributeSet *attributes);

    // static dev cap permissions
    //
    // (For a given widget handle, a set of device caps is
    // granted "statically", i.e. it is determined at installation
    // time that the widget will always get (at launch) the SMACK
    // permissions needed to use those device caps).
    //
    // 'permissions' is an output parameter - it must point to
    // an existing set and the function will clear it and fill
    // with the device cap names as described.
    static void getStaticDevCapPermissions(
        int widgetHandle,
        std::set<DPL::String> *permissions);

  protected:
    static int promptDecisionToInt(PromptDecision decision);
    static PromptDecision intToPromptDecision(int decision);
};

}

#endif
