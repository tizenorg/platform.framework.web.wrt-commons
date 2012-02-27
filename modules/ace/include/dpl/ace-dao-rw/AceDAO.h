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
 * @file       AceDAO.h
 * @author     Krzysztof Jackiewicz (k.jackiewicz@samsung.com)
 * @version    0.1
 * @brief
 */

#ifndef ACEDAO_H_
#define ACEDAO_H_

#include <set>
#include <list>
#include <map>
#include <string>

#include <dpl/optional_typedefs.h>
#include <dpl/string.h>
#include <dpl/ace-dao-ro/AceDAOReadOnly.h>
#include <dpl/ace-dao-ro/ValidityTypes.h>

namespace AceDB {
/*
 *
 */
class AceDAO : public AceDAOReadOnly
{
  public:

    AceDAO() {}

    // Policy Decisions
    static void setPolicyResult(
            const BaseAttributeSet &attributes,
            const PolicyResult &policyResult);

    static void removePolicyResult(
            const BaseAttributeSet &attributes);

    // PromptDecision
    static void setPromptDecision(
            const DPL::String &hash,
            const DPL::String &userParam,
            const DPL::OptionalString &session,
            PromptDecision decision);
    static void setPromptDecision(
            const BaseAttributeSet &attributes,
            const DPL::String &userParam,
            const DPL::OptionalString &session,
            PromptDecision decision);

    static void clearPromptDecisions(void);

    // reseting database
    static void clearWidgetDevCapSettings(void);
    static void clearDevCapSettings(void);
    static void clearAllSettings(void);
    static void resetDatabase(void);

    // resource settings
    static void setDevCapSetting(const std::string &resource,
            PreferenceTypes preference);
    static void removeDevCapSetting(const std::string &resource);

    // user settings
    static void setWidgetDevCapSetting(
            const std::string &resource,
            WidgetHandle handler,
            PreferenceTypes);
    static void removeWidgetDevCapSetting(
            const std::string &resource,
            WidgetHandle handler);

    // resource and subject management
    static int addResource(const std::string &request);

    // utilities
    static void addAttributes(const BaseAttributeSet &attributes);

    // set static dev cap permissions
    //
    // (For a given widget handle, a set of device caps is
    // granted "statically", i.e. it is determined at installation
    // time that the widget will always get (at launch) the SMACK
    // permissions needed to use those device caps).
    //
    // 'permissions' is the set of device cap names to be assigned
    // as "statically permitted" to the given widget handle
    static void setStaticDevCapPermissions(
        int widgetHandle,
        const std::set<DPL::String> &permissions);

};
}
#endif /* ACEDAO_H_ */
