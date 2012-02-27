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
 * @file       AceDAOReadOnlyReadOnly.cpp
 * @author     Krzysztof Jackiewicz (k.jackiewicz@samsung.com)
 * @author     Grzegorz Krawczyk (g.krawczyk@samsung.com)
 * @version    0.1
 * @brief
 */

#include <dpl/ace-dao-ro/AceDAOReadOnly.h>
#include <dpl/ace-dao-ro/AceDAOUtilities.h>
#include <dpl/ace-dao-ro/AceDAOConversions.h>
#include <dpl/ace-dao-ro/AceDatabase.h>

#include <dpl/foreach.h>

using namespace DPL::DB::ORM;
using namespace DPL::DB::ORM::ace;
using namespace AceDB::AceDaoUtilities;
using namespace AceDB::AceDaoConversions;

namespace AceDB {

static const int DB_ALLOW_ALWAYS = 0;
static const int DB_ALLOW_FOR_SESSION = 1;
static const int DB_ALLOW_THIS_TIME = 2;
static const int DB_DENY_ALWAYS = 3;
static const int DB_DENY_FOR_SESSION = 4;
static const int DB_DENY_THIS_TIME = 5;

int AceDAOReadOnly::promptDecisionToInt(PromptDecision decision)
{
    if (PromptDecision::ALLOW_ALWAYS == decision) {
        return DB_ALLOW_ALWAYS;
    } else if (PromptDecision::DENY_ALWAYS == decision) {
        return DB_DENY_ALWAYS;
    } else if (PromptDecision::ALLOW_THIS_TIME == decision) {
        return DB_ALLOW_THIS_TIME;
    } else if (PromptDecision::DENY_THIS_TIME == decision) {
        return DB_DENY_THIS_TIME;
    } else if (PromptDecision::ALLOW_FOR_SESSION == decision) {
        return DB_ALLOW_FOR_SESSION;
    }
    // DENY_FOR_SESSION
    return DB_DENY_FOR_SESSION;
}

PromptDecision AceDAOReadOnly::intToPromptDecision(int dec) {
    if (dec == DB_ALLOW_ALWAYS) {
        return PromptDecision::ALLOW_ALWAYS;
    } else if (dec == DB_DENY_ALWAYS) {
        return PromptDecision::DENY_ALWAYS;
    } else if (dec == DB_ALLOW_THIS_TIME) {
        return PromptDecision::ALLOW_THIS_TIME;
    } else if (dec == DB_DENY_THIS_TIME) {
        return PromptDecision::DENY_THIS_TIME;
    } else if (dec == DB_ALLOW_FOR_SESSION) {
        return PromptDecision::ALLOW_FOR_SESSION;
    }
    // DB_DENY_FOR_SESSION
    return PromptDecision::DENY_FOR_SESSION;
}

void AceDAOReadOnly::attachToThread()
{
    AceDaoUtilities::m_databaseInterface.AttachToThread();
}

void AceDAOReadOnly::detachFromThread()
{
    AceDaoUtilities::m_databaseInterface.DetachFromThread();
}

OptionalCachedPromptDecision AceDAOReadOnly::getPromptDecision(
    const DPL::String &hash,
    const std::string &userParam)
{
    Try {
        ScopedTransaction transaction(&AceDaoUtilities::m_databaseInterface);

        // get matching subject verdict
        ACE_DB_SELECT(select, AcePromptDecision, &AceDaoUtilities::m_databaseInterface);
        DPL::String DPLParam = DPL::FromUTF8String(userParam);

        select->Where(
            And(
                Equals<AcePromptDecision::hash>(hash),
                Equals<AcePromptDecision::user_param>(DPLParam)));

        std::list<AcePromptDecision::Row> rows = select->GetRowList();
        if (rows.empty()) {
            transaction.Commit();
            return OptionalCachedPromptDecision();
        }

        AcePromptDecision::Row row = rows.front();
        CachedPromptDecision decision;
        decision.decision = intToPromptDecision(row.Get_decision());
        decision.session = row.Get_session();

        return OptionalCachedPromptDecision(decision);
    }
    Catch(DPL::DB::SqlConnection::Exception::Base) {
        ReThrowMsg(Exception::DatabaseError, "Failed to getPromptDecision");
    }
}

OptionalCachedPromptDecision AceDAOReadOnly::getPromptDecision(
    const BaseAttributeSet &attributes,
    const std::string &userParam)
{
    return getPromptDecision(convertToHash(attributes), userParam);
}

void AceDAOReadOnly::getAttributes(BaseAttributeSet *attributes)
{
    if (NULL == attributes) {
        LogError("NULL pointer");
        return;
    }
    attributes->clear();
    std::string aname;
    int type;
    Try {
        ScopedTransaction transaction(&AceDaoUtilities::m_databaseInterface);

        ACE_DB_SELECT(select, AceAttribute, &AceDaoUtilities::m_databaseInterface);
        typedef std::list<AceAttribute::Row> RowList;
        RowList list = select->GetRowList();

        FOREACH(i, list) {
            BaseAttributePtr attribute(new BaseAttribute());
            DPL::String name = i->Get_name();
            aname = DPL::ToUTF8String(name);
            type = i->Get_type();

            attribute->setName(&aname);
            attribute->setType(intToAttributeType(type));
            attributes->insert(attribute);
        }
    }
    Catch(DPL::DB::SqlConnection::Exception::Base) {
        ReThrowMsg(Exception::DatabaseError, "Failed to getAttributes");
    }
}

OptionalPolicyResult AceDAOReadOnly::getPolicyResult(
        const BaseAttributeSet &attributes)
{

    auto attrHash = convertToHash(attributes);
    return getPolicyResult(attrHash);
}

OptionalPolicyResult AceDAOReadOnly::getPolicyResult(
    const DPL::String &attrHash)
{
    Try {
        ScopedTransaction transaction(&AceDaoUtilities::m_databaseInterface);

        // get matching subject verdict
        ACE_DB_SELECT(select, AcePolicyResult, &AceDaoUtilities::m_databaseInterface);
        Equals<AcePolicyResult::hash> e1(attrHash);
        select->Where(e1);

        std::list<AcePolicyResult::Row> rows = select->GetRowList();
        if (rows.empty()) {
            transaction.Commit();
            return OptionalPolicyResult();
        }

        AcePolicyResult::Row row = rows.front();
        int decision = row.Get_decision();
        PolicyResult res = PolicyResult::deserialize(decision);
        transaction.Commit();
        return OptionalPolicyResult(res);
    }
    Catch(DPL::DB::SqlConnection::Exception::Base) {
        ReThrowMsg(Exception::DatabaseError, "Failed to getVerdict");
    }
}

PreferenceTypes AceDAOReadOnly::getDevCapSetting(const std::string &resource)
{
    Try {
        AceDevCap::Row row;
        if (!getResourceByUri(resource, row)) {
            return PreferenceTypes::PREFERENCE_DEFAULT;
        }
        return intToPreference(row.Get_general_setting());
    }
    Catch(DPL::DB::SqlConnection::Exception::Base) {
        ReThrowMsg(Exception::DatabaseError, "Failed to getResourceSetting");
    }
}

void AceDAOReadOnly::getDevCapSettings(PreferenceTypesMap *globalSettingsMap)
{
    if (NULL == globalSettingsMap) {
        LogError("Null pointer");
        return;
    }
    globalSettingsMap->clear();
    Try {
        ACE_DB_SELECT(select, AceDevCap, &AceDaoUtilities::m_databaseInterface);
        typedef std::list<AceDevCap::Row> RowList;
        RowList list = select->GetRowList();

        FOREACH(i, list) {
            PreferenceTypes p = intToPreference(i->Get_general_setting());
            globalSettingsMap->insert(make_pair(DPL::ToUTF8String(
                i->Get_id_uri()), p));
        }
    }
    Catch(DPL::DB::SqlConnection::Exception::Base) {
        ReThrowMsg(Exception::DatabaseError, "Failed to getResourceSettings");
    }
}



void AceDAOReadOnly::getWidgetDevCapSettings(BasePermissionList *outputList)
{
    if (NULL == outputList) {
        LogError("NULL pointer");
        return;
    }
    outputList->clear();
    Try {
        ScopedTransaction transaction(&AceDaoUtilities::m_databaseInterface);

        std::string resourceName;
        PreferenceTypes allowAccess;

        ACE_DB_SELECT(select,
                      AceWidgetDevCapSetting,
                      &AceDaoUtilities::m_databaseInterface);

        typedef std::list<AceWidgetDevCapSetting::Row> RowList;
        RowList list = select->GetRowList();

        // TODO JOIN
        FOREACH(i, list) {
            int app_id = i->Get_app_id();
            int res_id = i->Get_resource_id();

            ACE_DB_SELECT(resourceSelect, AceDevCap, &AceDaoUtilities::m_databaseInterface);
            resourceSelect->Where(Equals<AceDevCap::resource_id>(res_id));
            AceDevCap::Row rrow = resourceSelect->GetSingleRow();

            resourceName = DPL::ToUTF8String(rrow.Get_id_uri());

            if (!resourceName.empty()) {
                allowAccess = intToPreference(i->Get_access_value());
                outputList->push_back(
                    BasePermission(app_id,
                    resourceName,
                    allowAccess));
            }
        }
    }
    Catch(DPL::DB::SqlConnection::Exception::Base) {
        ReThrowMsg(Exception::DatabaseError, "Failed to findUserSettings");
    }
}

PreferenceTypes AceDAOReadOnly::getWidgetDevCapSetting(
        const std::string &resource,
        WidgetHandle handler)
{
    Try {
        ScopedTransaction transaction(&AceDaoUtilities::m_databaseInterface);

        AceDevCap::Row rrow;
        if (!getResourceByUri(resource, rrow)) {
            return PreferenceTypes::PREFERENCE_DEFAULT;
        }
        int resourceId = rrow.Get_resource_id();

        // get matching user setting
        ACE_DB_SELECT(select, AceWidgetDevCapSetting, &AceDaoUtilities::m_databaseInterface);

        select->Where(And(Equals<AceWidgetDevCapSetting::resource_id>(resourceId),
                Equals<AceWidgetDevCapSetting::app_id>(handler)));

        std::list<int> values =
            select->GetValueList<AceWidgetDevCapSetting::access_value>();
        if (values.empty()) {
            return PreferenceTypes::PREFERENCE_DEFAULT;
        }
        return intToPreference(values.front());
    }
    Catch(DPL::DB::SqlConnection::Exception::Base) {
        ReThrowMsg(Exception::DatabaseError, "Failed in getUserSetting");
    }
}

void AceDAOReadOnly::getStaticDevCapPermissions(
    int widgetHandle,
    std::set<DPL::String> *permissions)
{
    if (NULL == permissions) {
        LogError("NULL pointer");
        return;
    }
    permissions->clear();
    Try {
        ScopedTransaction transaction(&AceDaoUtilities::m_databaseInterface);

        ACE_DB_SELECT(select, AceStaticDevCapPermission,
                      &AceDaoUtilities::m_databaseInterface);
        select->Where(
            Equals<AceStaticDevCapPermission::app_id>(widgetHandle));
        std::list<DPL::String> devCapNames =
            select->GetValueList<AceStaticDevCapPermission::dev_cap>();
        permissions->insert(devCapNames.begin(), devCapNames.end());
        transaction.Commit();
    } Catch(DPL::DB::SqlConnection::Exception::Base) {
        ReThrowMsg(Exception::DatabaseError, "Failed to getStaticDevCapPermissions");
    }
}
}
