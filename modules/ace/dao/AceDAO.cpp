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
 * @file       AceDAO.cpp
 * @author     Krzysztof Jackiewicz (k.jackiewicz@samsung.com)
 * @version    0.1
 * @brief
 */

#include <dpl/ace-dao-rw/AceDAO.h>

#include <openssl/md5.h>
#include <dpl/foreach.h>
#include <dpl/string.h>
#include <dpl/log/log.h>
#include <dpl/db/orm.h>
#include <dpl/ace-dao-ro/AceDAOUtilities.h>
#include <dpl/ace-dao-ro/AceDAOConversions.h>
#include <dpl/ace-dao-ro/AceDatabase.h>

using namespace DPL::DB::ORM;
using namespace DPL::DB::ORM::ace;
using namespace AceDB::AceDaoUtilities;
using namespace AceDB::AceDaoConversions;

namespace {
char const * const EMPTY_SESSION = "";
} // namespace

namespace AceDB{

void AceDAO::setPromptDecision(
    const DPL::String &hash,
    const DPL::String &userParam,
    const DPL::OptionalString &session,
    PromptDecision decision)
{
    Try {
        ScopedTransaction transaction(&AceDaoUtilities::m_databaseInterface);

        ACE_DB_DELETE(del, AcePromptDecision, &AceDaoUtilities::m_databaseInterface);
        del->Where(
            And(
                Equals<AcePromptDecision::user_param>(userParam),
                Equals<AcePromptDecision::hash>(hash)));
        del->Execute();

        AcePromptDecision::Row row;
        row.Set_hash(hash);
        row.Set_decision(promptDecisionToInt(decision));
        row.Set_user_param(userParam);
        row.Set_session(session);
        ACE_DB_INSERT(insert, AcePromptDecision, &AceDaoUtilities::m_databaseInterface);
        insert->Values(row);
        insert->Execute();

        transaction.Commit();
    }
    Catch(DPL::DB::SqlConnection::Exception::Base) {
        ReThrowMsg(Exception::DatabaseError, "Failed to setUserSetting");
    }
}

void AceDAO::setPromptDecision(
    const BaseAttributeSet &attributes,
    const DPL::String &userParam,
    const DPL::OptionalString &session,
    PromptDecision decision)
{
    DPL::String DPLHash = convertToHash(attributes);

    setPromptDecision(
        DPLHash,
        userParam,
        session,
        decision);
}

void AceDAO::removePolicyResult(
        const BaseAttributeSet &attributes)
{
    Try {
        ScopedTransaction transaction(&AceDaoUtilities::m_databaseInterface);

        auto attrHash =  convertToHash(attributes);

        ACE_DB_DELETE(del,
                      AcePolicyResult,
                      &AceDaoUtilities::m_databaseInterface);
        del->Where(Equals<AcePolicyResult::hash>(attrHash));
        del->Execute();
        transaction.Commit();
    }
    Catch(DPL::DB::SqlConnection::Exception::Base) {
        ReThrowMsg(Exception::DatabaseError, "Failed to removeVerdict");
    }
}

void AceDAO::clearAllSettings(void)
{
    clearWidgetDevCapSettings();
    clearDevCapSettings();
}

void AceDAO::setDevCapSetting(const std::string &resource,
                              PreferenceTypes preference)
{
    Try {
        ACE_DB_UPDATE(update, AceDevCap, &AceDaoUtilities::m_databaseInterface);
        AceDevCap::Row row;
        row.Set_general_setting(preferenceToInt(preference));
        update->Values(row);
        update->Where(
            Equals<AceDevCap::id_uri>(DPL::FromUTF8String(resource)));
        update->Execute();
    }
    Catch(DPL::DB::SqlConnection::Exception::Base) {
        ReThrowMsg(Exception::DatabaseError, "Failed to SetResourceSetting");
    }
}

void AceDAO::removeDevCapSetting(const std::string &resource)
{
    Try {
        ACE_DB_UPDATE(update, AceDevCap, &AceDaoUtilities::m_databaseInterface);
        AceDevCap::Row row;
        row.Set_general_setting(preferenceToInt(PreferenceTypes::PREFERENCE_DEFAULT));
        update->Values(row);
        update->Where(
            Equals<AceDevCap::id_uri>(DPL::FromUTF8String(resource)));
        update->Execute();
    }
    Catch(DPL::DB::SqlConnection::Exception::Base) {
        ReThrowMsg(Exception::DatabaseError, "Failed to removeResourceSetting");
    }
}


void AceDAO::setWidgetDevCapSetting(const std::string &resource,
                                    WidgetHandle handler,
                                    PreferenceTypes preference)
{
    Try {
        ScopedTransaction transaction(&AceDaoUtilities::m_databaseInterface);
        // TODO JOIN
        AceDevCap::Row rrow;
        if (!getResourceByUri(resource, rrow)) {
            ThrowMsg(Exception::DatabaseError, "Resource not found");
        }

        ACE_DB_INSERT(insert,
                      AceWidgetDevCapSetting,
                      &AceDaoUtilities::m_databaseInterface);

        AceWidgetDevCapSetting::Row row;
        row.Set_app_id(handler);
        int rid = rrow.Get_resource_id();
        row.Set_resource_id(rid);
        row.Set_access_value(preferenceToInt(preference));
        insert->Values(row);
        insert->Execute();

        transaction.Commit();
    }
    Catch(DPL::DB::SqlConnection::Exception::Base) {
        ReThrowMsg(Exception::DatabaseError, "Failed to setUserSetting");
    }
}

void AceDAO::removeWidgetDevCapSetting(const std::string &resource,
                                       WidgetHandle handler)
{
    Try {
        ScopedTransaction transaction(&AceDaoUtilities::m_databaseInterface);
        AceDevCap::Row rrow;
        if (!getResourceByUri(resource, rrow)) {
            ThrowMsg(Exception::DatabaseError, "resource not found");
        }

        ACE_DB_DELETE(del,
                      AceWidgetDevCapSetting,
                      &AceDaoUtilities::m_databaseInterface);

        Equals<AceWidgetDevCapSetting::app_id> e1(handler);
        Equals<AceWidgetDevCapSetting::resource_id> e2(rrow.Get_resource_id());
        del->Where(And(e1, e2));
        del->Execute();
        transaction.Commit();
    }
    Catch(DPL::DB::SqlConnection::Exception::Base) {
        ReThrowMsg(Exception::DatabaseError, "Failed to clearUserSettings");
    }
}


void AceDAO::setPolicyResult(const BaseAttributeSet &attributes,
                             const PolicyResult &policyResult)
{
    Try {
        ScopedTransaction transaction(&AceDaoUtilities::m_databaseInterface);

        // TODO: this call is connected with logic.
        // It should be moved to PolicyEvaluator
        addAttributes(attributes);

        auto attrHash = convertToHash(attributes);

        ACE_DB_DELETE(del, AcePolicyResult, &AceDaoUtilities::m_databaseInterface)
        del->Where(Equals<AcePolicyResult::hash>(attrHash));
        del->Execute();

        ACE_DB_INSERT(insert, AcePolicyResult, &AceDaoUtilities::m_databaseInterface);
        AcePolicyResult::Row row;
        row.Set_decision(PolicyResult::serialize(policyResult));
        row.Set_hash(attrHash);
        insert->Values(row);
        insert->Execute();

        transaction.Commit();
    }
    Catch(DPL::DB::SqlConnection::Exception::Base) {
        ReThrowMsg(Exception::DatabaseError, "Failed to addVerdict");
    }
}

void AceDAO::resetDatabase(void)
{
    Try {
        ScopedTransaction transaction(&AceDaoUtilities::m_databaseInterface);
        ACE_DB_DELETE(del1, AcePolicyResult, &AceDaoUtilities::m_databaseInterface);
        del1->Execute();
        ACE_DB_DELETE(del2, AceWidgetDevCapSetting, &AceDaoUtilities::m_databaseInterface);
        del2->Execute();
        ACE_DB_DELETE(del3, AceDevCap, &AceDaoUtilities::m_databaseInterface);
        del3->Execute();
        ACE_DB_DELETE(del4, AceSubject, &AceDaoUtilities::m_databaseInterface);
        del4->Execute();
        ACE_DB_DELETE(del5, AceAttribute, &AceDaoUtilities::m_databaseInterface);
        del5->Execute();
        ACE_DB_DELETE(del6, AcePromptDecision, &AceDaoUtilities::m_databaseInterface);
        del6->Execute();

        transaction.Commit();

        // TODO there is no such query yet in ORM.
        //        GlobalConnection::DataCommandAutoPtr command =
        //                GlobalConnectionSingleton::Instance().PrepareDataCommand(
        //                        "VACUUM");
        //        command->Step();
    }
    Catch(DPL::DB::SqlConnection::Exception::Base) {
        ReThrowMsg(Exception::DatabaseError, "Failed to resetDatabase");
    }
}

void AceDAO::clearDevCapSettings()
{
    Try {
        ACE_DB_UPDATE(update, AceDevCap, &AceDaoUtilities::m_databaseInterface);
        AceDevCap::Row row;
        row.Set_general_setting(-1);
        update->Values(row);
        update->Execute();
    }
    Catch(DPL::DB::SqlConnection::Exception::Base) {
        ReThrowMsg(Exception::DatabaseError, "Failed to clearResourceSettings");
    }
}

void AceDAO::clearWidgetDevCapSettings()
{
    Try {
        ACE_DB_DELETE(del, AceWidgetDevCapSetting, &AceDaoUtilities::m_databaseInterface);
        del->Execute();
    }
    Catch(DPL::DB::SqlConnection::Exception::Base) {
        ReThrowMsg(Exception::DatabaseError, "Failed to clearUserSettings");
    }
}

int AceDAO::addResource(const std::string &request)
{
    LogDebug("addResource: " << request);
    Try {
        ScopedTransaction transaction(&AceDaoUtilities::m_databaseInterface);
        AceDevCap::Row rrow;
        if (getResourceByUri(request, rrow)) {
            transaction.Commit();
            return rrow.Get_resource_id();
        }

        ACE_DB_INSERT(insert, AceDevCap, &AceDaoUtilities::m_databaseInterface);
        AceDevCap::Row row;
        row.Set_id_uri(DPL::FromUTF8String(request));
        row.Set_general_setting(-1);
        insert->Values(row);
        int id = insert->Execute();
        transaction.Commit();
        return id;
    }
    Catch(DPL::DB::SqlConnection::Exception::Base) {
        ReThrowMsg(Exception::DatabaseError, "Failed in addResource");
    }
}

void AceDAO::addAttributes(const BaseAttributeSet &attributes)
{
    Try {
        BaseAttributeSet::const_iterator iter;

        for (iter = attributes.begin(); iter != attributes.end(); ++iter) {
            ACE_DB_SELECT(select, AceAttribute, &AceDaoUtilities::m_databaseInterface);
            select->Where(Equals<AceAttribute::name>(DPL::FromUTF8String(
                *(*iter)->getName())));
            std::list<AceAttribute::Row> rows = select->GetRowList();
            if (!rows.empty()) {
                continue;
            }

            ACE_DB_INSERT(insert, AceAttribute, &AceDaoUtilities::m_databaseInterface);
            AceAttribute::Row row;
            row.Set_name(DPL::FromUTF8String(*(*iter)->getName()));
            row.Set_type(attributeTypeToInt((*iter)->getType()));
            insert->Values(row);
            insert->Execute();
        }
    }
    Catch(DPL::DB::SqlConnection::Exception::Base) {
        ReThrowMsg(Exception::DatabaseError, "Failed in addAttributes");
    }
}

void AceDAO::setStaticDevCapPermissions(
    int widgetHandle,
    const std::set<DPL::String> &permissions)
{
    Try {
        FOREACH(it, permissions) {
          ACE_DB_INSERT(insert, AceStaticDevCapPermission,
                        &AceDaoUtilities::m_databaseInterface);
          AceStaticDevCapPermission::Row row;
          row.Set_app_id(widgetHandle);
          row.Set_dev_cap(*it);
          insert->Values(row);
          insert->Execute();
        }
    } Catch(DPL::DB::SqlConnection::Exception::Base) {
        ReThrowMsg(Exception::DatabaseError, "Failed in setStaticDevCapPermissions");
    }
}


}
