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
 * @file   global_dao.cpp
 * @author  Pawel Sikorski (p.sikorski@samsung.com)
 * @version 1.0
 * @brief   This file contains the definition of global dao class.
 */

#include <dpl/wrt-dao-rw/global_dao.h>
#include <dpl/log/log.h>
#include <dpl/string.h>
#include <dpl/db/orm.h>
#include <orm_generator_wrt.h>
#include <dpl/wrt-dao-ro/webruntime_database.h>
#include <dpl/wrt-dao-ro/WrtDatabase.h>

namespace WrtDB {

void GlobalDAO::AddAdultBlackListElement(const DPL::String &url)
{
    Try {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;

        ScopedTransaction transaction(&WrtDatabase::interface());
        if (IsElementOnAdultBlackList(url)) {
            transaction.Commit();
            return;
        }

        WRT_DB_INSERT(insert, ChildProtectionBlacklist, &WrtDatabase::interface())
        ChildProtectionBlacklist::Row row;
        row.Set_url(url);

        insert->Values(row);
        insert->Execute();
        transaction.Commit();
    } Catch(DPL::DB::SqlConnection::Exception::Base) {
        ReThrowMsg(GlobalDAO::Exception::DatabaseError,
                   "Failed to add element to AdultBlackList");
    }
}

void GlobalDAO::RemoveAdultBlackListElement(const DPL::String &url)
{
    Try {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;
        WRT_DB_DELETE(deleteFrom, ChildProtectionBlacklist, &WrtDatabase::interface())

        deleteFrom->Where(Equals<ChildProtectionBlacklist::url>(url));
        deleteFrom->Execute();
    } Catch(DPL::DB::SqlConnection::Exception::Base) {
        ReThrowMsg(GlobalDAO::Exception::DatabaseError,
                   "Failed to removed element from AdultBlackList");
    }
}

void GlobalDAO::UpdateAdultBlackList(const DPL::String &oldUrl,
                                     const DPL::String &newUrl)
{
    Try {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;

        ScopedTransaction transaction(&WrtDatabase::interface());
        if (IsElementOnAdultBlackList(newUrl)) {
            transaction.Commit();
            return;
        }
        WRT_DB_UPDATE(update, ChildProtectionBlacklist, &WrtDatabase::interface())
        ChildProtectionBlacklist::Row row;
        row.Set_url(newUrl);

        update->Where(Equals<ChildProtectionBlacklist::url>(oldUrl));
        update->Values(row);
        update->Execute();
        transaction.Commit();
    } Catch(DPL::DB::SqlConnection::Exception::Base) {
        ReThrowMsg(GlobalDAO::Exception::DatabaseError,
                   "Failed to update an element in AdultBlackList");
    }
}

void GlobalDAO::SetDeveloperMode(bool mode)
{
    LogDebug("updating Developer mode to:" << mode);
    Try {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;
        GlobalProperties::Row row;
        row.Set_developer_mode(mode);

        WRT_DB_UPDATE(update, GlobalProperties, &WrtDatabase::interface())
        update->Values(row);
        update->Execute();
    }
    Catch(DPL::DB::SqlConnection::Exception::Base){
        ReThrowMsg(GlobalDAO::Exception::DatabaseError,
                   "Failed to update Developer Mode");
    }
}

void GlobalDAO::AddDefferedWidgetPackageInstallation(const DPL::String &path)
{
    LogDebug("Adding widget package as defered. Path: " << path);
    Try {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;
        DefferedWidgetPackageInstallation::Row row;
        row.Set_path(path);

        WRT_DB_INSERT(insert, DefferedWidgetPackageInstallation, &WrtDatabase::interface())
        insert->Values(row);
        insert->Execute();
    }
    Catch(DPL::DB::SqlConnection::Exception::Base){
        ReThrowMsg(GlobalDAO::Exception::DatabaseError,
                   "Failed to add defered widget package");
    }
}

void GlobalDAO::RemoveDefferedWidgetPackageInstallation(const DPL::String &path)
{
    LogDebug("Remove widget package from differed list. Path: " << path);
    Try {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;
        WRT_DB_DELETE(del, DefferedWidgetPackageInstallation, &WrtDatabase::interface())
        del->Where(Equals<DefferedWidgetPackageInstallation::path>(path));
        del->Execute();
    }
    Catch(DPL::DB::SqlConnection::Exception::Base){
        ReThrowMsg(GlobalDAO::Exception::DatabaseError,
                   "Failed to remove defered widget package");
    }
}

void GlobalDAO::SetParentalMode(bool parental_status)
{
    Try {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;
        GlobalProperties::Row row;
        row.Set_parental_mode(parental_status);
        WRT_DB_UPDATE(update, GlobalProperties, &WrtDatabase::interface())

        update->Values(row);
        update->Execute();
    } Catch(DPL::DB::SqlConnection::Exception::Base) {
        ReThrowMsg(GlobalDAO::Exception::DatabaseError,
                   "Failed to update Parental Mode");
    }
}

void GlobalDAO::SetParentalAllowedAge(const DPL::OptionalInt& age)
{
    using namespace DPL::DB::ORM;
    using namespace DPL::DB::ORM::wrt;
    Try {
        typedef wrt::GlobalProperties::Row ParentalAllowedAgeRow;

        ParentalAllowedAgeRow row;
        row.Set_parental_allowed_age(age);
        WRT_DB_UPDATE(update, GlobalProperties, &WrtDatabase::interface())
        update->Values(row);
        update->Execute();
    } Catch(DPL::DB::SqlConnection::Exception::Base) {
        ReThrowMsg(GlobalDAO::Exception::DatabaseError,
                   "Failed to update Parental Mode");
    }
}

void GlobalDAO::SetSecureByDefault(bool secure)
{
    //If secure == true -> widget does not need to be signed
    Try {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;
        GlobalProperties::Row row;
        row.Set_secure_by_default(secure);
        WRT_DB_UPDATE(update, GlobalProperties, &WrtDatabase::interface())
        update->Values(row);
        update->Execute();
    }
    Catch(DPL::DB::SqlConnection::Exception::Base) {
        ReThrowMsg(GlobalDAO::Exception::DatabaseError,
                   "Failed to update secureByDefault");
    }
}

void GlobalDAO::setComplianceMode(bool mode)
{
    LogDebug("Updating compliance mode to:" << mode);
    Try{
        using namespace DPL::DB::ORM; using namespace DPL::DB::ORM::wrt;
        GlobalProperties::Row row;
        row.Set_compliance_mode(mode);

        WRT_DB_UPDATE(update, GlobalProperties, &WrtDatabase::interface())
        update->Values(row);
        update->Execute();
    }
    Catch (DPL::DB::SqlConnection::Exception::Base){
        ReThrowMsg(GlobalDAO::Exception::DatabaseError,
                   "Failed to update compliance mode");
    }
}

void GlobalDAO::setComplianceFakeImei(const std::string &imei)
{
    LogDebug("Setting compliance fake IMEI: " << imei);
    Try{
        using namespace DPL::DB::ORM; using namespace DPL::DB::ORM::wrt;
        GlobalProperties::Row row;
        row.Set_compliance_fake_imei(DPL::FromASCIIString(imei));

        WRT_DB_UPDATE(update, GlobalProperties, &WrtDatabase::interface())
        update->Values(row);
        update->Execute();
    }
    Catch (DPL::DB::SqlConnection::Exception::Base){
        ReThrowMsg(GlobalDAO::Exception::DatabaseError,
                   "Failed to update compliance fake IMEI");
    }
}

void GlobalDAO::setComplianceFakeMeid(const std::string &meid)
{
    LogDebug("Setting compliance fake MEID: " << meid);
    Try{
        using namespace DPL::DB::ORM; using namespace DPL::DB::ORM::wrt;
        GlobalProperties::Row row;
        row.Set_compliance_fake_meid(DPL::FromASCIIString(meid));

        WRT_DB_UPDATE(update, GlobalProperties, &WrtDatabase::interface())
        update->Values(row);
        update->Execute();
    }
    Catch (DPL::DB::SqlConnection::Exception::Base){
        ReThrowMsg(GlobalDAO::Exception::DatabaseError,
                   "Failed to update compliance fake MEID");
    }
}

void GlobalDAO::AddCategoryRule(const ChildProtection::PowderRules::
                                CategoryRule& powder)
{
    Try {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;
        PowderRules::Row row;
        row.Set_category(powder.category);
        row.Set_level(powder.level);
        row.Set_context(powder.context);

        wrt::ScopedTransaction transaction(&WrtDatabase::interface());
        if (IsPowderRulePresent(powder)) {
            transaction.Commit();
            return;
        }
        WRT_DB_INSERT(insert, PowderRules, &WrtDatabase::interface())

        insert->Values(row);
        insert->Execute();
        transaction.Commit();
    } Catch(DPL::DB::SqlConnection::Exception::Base) {
        ReThrowMsg(GlobalDAO::Exception::DatabaseError,
                   "Failed to add POWDER rules");
    }
}

void GlobalDAO::RemoveCategoryRule(const ChildProtection::PowderRules::
                                   CategoryRule& powder)
{
    Try {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;

        if (!powder.context.IsNull()) {
            WRT_DB_DELETE(delWithContext, PowderRules, &WrtDatabase::interface())

            delWithContext->Where(
                And(Equals<PowderRules::category>(powder.category),
                    And(Equals<PowderRules::level>(powder.level),
                        Equals<PowderRules::context>(powder.context))));
            delWithContext->Execute();
        } else {
            WRT_DB_DELETE(delWithoutContext, PowderRules, &WrtDatabase::interface())
            delWithoutContext->Where(
                And(Equals<PowderRules::category>(powder.category),
                    And(Equals<PowderRules::level>(powder.level),
                        Is<PowderRules::context>(powder.context))));
            delWithoutContext->Execute();
        }
    } Catch(DPL::DB::SqlConnection::Exception::Base) {
        ReThrowMsg(GlobalDAO::Exception::DatabaseError,
                   "Failed to remove POWDER rules");
    }
}

void GlobalDAO::UpdateCategoryRule(
        const ChildProtection::PowderRules::CategoryRule& oldRule,
        const ChildProtection::PowderRules::CategoryRule& newRule)
{
    Try {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;
        PowderRules::Row row;
        row.Set_category(newRule.category);
        row.Set_level(newRule.level);
        row.Set_context(newRule.context);

        wrt::ScopedTransaction transaction(&WrtDatabase::interface());

        if (IsPowderRulePresent(newRule)) {
            transaction.Commit();
            return;
        }

        if (!oldRule.context.IsNull()) {
            WRT_DB_UPDATE(updateWithContext, PowderRules, &WrtDatabase::interface())
            updateWithContext->Where(
                And(Equals<PowderRules::category>(oldRule.category),
                    And(Equals<PowderRules::level>(oldRule.level),
                        Equals<PowderRules::context>(oldRule.context))));
            updateWithContext->Values(row);
            updateWithContext->Execute();
        } else {
            WRT_DB_UPDATE(updateWithoutContext, PowderRules, &WrtDatabase::interface())
            updateWithoutContext->Where(
                And(Equals<PowderRules::category>(oldRule.category),
                    And(Equals<PowderRules::level>(oldRule.level),
                        Is<PowderRules::context>(oldRule.context))));
            updateWithoutContext->Values(row);
            updateWithoutContext->Execute();
        }
        transaction.Commit();
    } Catch(DPL::DB::SqlConnection::Exception::Base) {
        ReThrowMsg(GlobalDAO::Exception::DatabaseError,
                   "Failed to update POWDER rules");
    }
}

void GlobalDAO::SetHomeNetworkDataUsage(GlobalDAO::NetworkAccessMode newMode)
{
    LogDebug("updating home network data usage to:" << newMode);
    Try {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;
        GlobalProperties::Row row;
        row.Set_home_network_data_usage(static_cast<int>(newMode));

        WRT_DB_UPDATE(update, GlobalProperties, &WrtDatabase::interface())
        update->Values(row);
        update->Execute();
    }
    Catch(DPL::DB::SqlConnection::Exception::Base){
        ReThrowMsg(GlobalDAO::Exception::DatabaseError,
                   "Failed to update home network data usage");
    }
}

void GlobalDAO::SetRoamingDataUsage(GlobalDAO::NetworkAccessMode newMode)
{
    LogDebug("updating roaming network data usage to:" << newMode);
    Try {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;
        GlobalProperties::Row row;
        row.Set_roaming_data_usage(static_cast<int>(newMode));

        WRT_DB_UPDATE(update, GlobalProperties, &WrtDatabase::interface())
        update->Values(row);
        update->Execute();
    }
    Catch(DPL::DB::SqlConnection::Exception::Base){
        ReThrowMsg(GlobalDAO::Exception::DatabaseError,
                   "Failed to update roaming network data usage");
    }
}

void GlobalDAO::SetAutoSaveIdPasswd(const DPL::String &url,
                                    const AutoSaveData &saveData)
{
    Try {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;
        ScopedTransaction transaction(&WrtDatabase::interface());
        AutoSaveIdPasswd::Row row;

        row.Set_address(url);
        row.Set_userId(saveData.userId);
        row.Set_passwd(saveData.passwd);

        DPL::Optional<GlobalDAO::AutoSaveData> savedData =
            GetAutoSaveIdPasswd(url);

        if (!savedData.IsNull()) {
            WRT_DB_UPDATE(update, AutoSaveIdPasswd, &WrtDatabase::interface())

            update->Where(Equals<AutoSaveIdPasswd::address>(url));
            update->Values(row);
            update->Execute();
        } else {
            WRT_DB_INSERT(insert, AutoSaveIdPasswd, &WrtDatabase::interface());
            insert->Values(row);
            insert->Execute();
        }

        transaction.Commit();
    } Catch(DPL::DB::SqlConnection::Exception::Base) {
        ReThrowMsg(GlobalDAO::Exception::DatabaseError,
                   "Fail to register id, passwd for autosave");
    }
}

void GlobalDAO::AddWhiteURI(const std::string &uri, bool subDomain)
{
    LogDebug("Add White URI : " << uri);
    Try {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;
        WidgetWhiteURIList::Row row;
        row.Set_uri(DPL::FromASCIIString(uri));
        row.Set_subdomain_access(static_cast<int>(subDomain));
        wrt::ScopedTransaction transaction(&WrtDatabase::interface());

        WRT_DB_INSERT(insert, WidgetWhiteURIList, &WrtDatabase::interface())

        insert->Values(row);
        insert->Execute();
        transaction.Commit();
    }
    Catch(DPL::DB::SqlConnection::Exception::Base){
        ReThrowMsg(GlobalDAO::Exception::DatabaseError,
                   "Failed to add white URI");
    }
}

void GlobalDAO::RemoveWhiteURI(const std::string &uri)
{
    LogDebug("Remove White URI : " << uri);
    Try {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;

        WRT_DB_DELETE(deleteFrom, WidgetWhiteURIList, &WrtDatabase::interface())
        deleteFrom->Where(Equals<WidgetWhiteURIList::uri>(DPL::FromASCIIString(uri)));
        deleteFrom->Execute();
    } Catch(DPL::DB::SqlConnection::Exception::Base) {
        ReThrowMsg(GlobalDAO::Exception::DatabaseError,
                   "Failed to removed white URI from AdultBlackList");
    }
}

} // namespace WrtDB
