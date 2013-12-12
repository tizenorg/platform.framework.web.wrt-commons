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
 * @file    security_origin_dao.cpp
 * @author  Jihoon Chung (jihoon.chung@samsung.com)
 * @version 1.0
 * @brief    This file contains the definition of security origin dao class.
 */

#include <wrt-commons/security-origin-dao/security_origin_dao.h>
#include <wrt-commons/security-origin-dao/security_origin_dao_types.h>
#include <orm_generator_security_origin.h>
#include <dpl/foreach.h>
#include <dpl/wrt-dao-ro/widget_dao_read_only.h>
#include <dpl/wrt-dao-ro/WrtDatabase.h>
#include <dpl/wrt-dao-ro/widget_config.h>
#include <dpl/wrt-dao-ro/global_config.h>
#include <dpl/wrt-dao-ro/common_dao_types.h>
#include <sys/stat.h>
#include <fstream>
#include <unistd.h>
#include <tzplatform_config.h>

using namespace DPL::DB::ORM;
using namespace DPL::DB::ORM::security_origin;

namespace SecurityOriginDB {
using namespace WrtDB;
#define SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN          Try

#define SQL_CONNECTION_EXCEPTION_HANDLER_END(message)   \
    Catch(DPL::DB::SqlConnection::Exception::Base) {       \
        LogError(message);                              \
        ReThrowMsg(SecurityOriginDAO::Exception::DatabaseError, \
                   message);                            \
    }

// database connection
#define SECURITY_ORIGIN_DB_INTERNAL(tlsCommand, InternalType, interface) \
    InternalType tlsCommand(interface);
#define SECURITY_ORIGIN_DB_SELECT(name, type, interface) \
    SECURITY_ORIGIN_DB_INTERNAL(name, type::Select, interface)
#define SECURITY_ORIGIN_DB_INSERT(name, type, interface) \
    SECURITY_ORIGIN_DB_INTERNAL(name, type::Insert, interface)
#define SECURITY_ORIGIN_DB_UPDATE(name, type, interface) \
    SECURITY_ORIGIN_DB_INTERNAL(name, type::Update, interface)
#define SECURITY_ORIGIN_DB_DELETE(name, type, interface) \
    SECURITY_ORIGIN_DB_INTERNAL(name, type::Delete, interface)

typedef DPL::DB::ORM::security_origin::SecurityOriginInfo::Row
    SecurityOriginInfoRow;
typedef DPL::DB::ORM::security_origin::SecurityOriginInfo::Select::RowList
    SecurityOriginInfoRowList;

namespace {
DPL::DB::SqlConnection::Flag::Option SECURITY_ORIGIN_DB_OPTION =
    DPL::DB::SqlConnection::Flag::RW;
DPL::DB::SqlConnection::Flag::Type SECURITY_ORIGIN_DB_TYPE =
    DPL::DB::SqlConnection::Flag::UseLucene;
const char* const SECURITY_ORIGIN_DB_NAME = ".security_origin.db";
const char* const SECURITY_ORIGIN_DB_SQL_PATH =
    "/usr/share/wrt-engine/security_origin_db.sql";
const char* const SECURITY_DATABASE_JOURNAL_FILENAME = "-journal";

const int WEB_APPLICATION_UID = tzplatform_getuid(TZ_USER_NAME);
const int WEB_APPLICATION_GUID = tzplatform_getgid(TZ_SYS_USER_GROUP);

std::string createDatabasePath(const WrtDB::TizenPkgId &pkgName)
{
    std::stringstream filename;

    filename << WrtDB::WidgetConfig::GetWidgetPersistentStoragePath(pkgName)
             << "/"
             << SECURITY_ORIGIN_DB_NAME;
    return filename.str();
}

void checkDatabase(std::string databasePath)
{
    SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
    {
        if (databasePath.empty()) {
            ThrowMsg(SecurityOriginDAO::Exception::DatabaseError,
                     "Wrong database Path is passed");
        }

        struct stat buffer;
        if (stat(databasePath.c_str(), &buffer) != 0) {
            //Create fresh database
            LogDebug("Creating database " << databasePath);

            std::fstream file;
            file.open(SECURITY_ORIGIN_DB_SQL_PATH, std::ios_base::in);
            if (!file) {
                ThrowMsg(SecurityOriginDAO::Exception::DatabaseError,
                         "Fail to get database Path");
            }

            std::stringstream ssBuffer;
            ssBuffer << file.rdbuf();

            file.close();

            DPL::DB::SqlConnection con(databasePath,
                                       SECURITY_ORIGIN_DB_TYPE,
                                       SECURITY_ORIGIN_DB_OPTION);
            con.ExecCommand(ssBuffer.str().c_str());

            if(chown(databasePath.c_str(),
                     WEB_APPLICATION_UID,
                     WEB_APPLICATION_GUID) != 0)
            {
                ThrowMsg(SecurityOriginDAO::Exception::DatabaseError,
                     "Fail to change uid/guid");
            }
            std::string databaseJournal =
                databasePath + SECURITY_DATABASE_JOURNAL_FILENAME;
            if(chown(databaseJournal.c_str(),
                     WEB_APPLICATION_UID,
                     WEB_APPLICATION_GUID) != 0)
            {
                ThrowMsg(SecurityOriginDAO::Exception::DatabaseError,
                     "Fail to change uid/guid");
            }
        }
    }
    SQL_CONNECTION_EXCEPTION_HANDLER_END("Fail to get database Path")
}
} // namespace SecurityOriginDB

SecurityOriginDAO::SecurityOriginDAO(const WrtDB::TizenPkgId &pkgName) :
    m_dbPath(createDatabasePath(pkgName)),
    m_dbInterface(m_dbPath, SECURITY_ORIGIN_DB_TYPE)
{
    checkDatabase(m_dbPath);
    m_dbInterface.AttachToThread(SECURITY_ORIGIN_DB_OPTION);
}

SecurityOriginDAO::~SecurityOriginDAO()
{
    m_dbInterface.DetachFromThread();
}

SecurityOriginDataList SecurityOriginDAO::getSecurityOriginDataList(void)
{
    SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
    {
        SecurityOriginDataList list;
        SECURITY_ORIGIN_DB_SELECT(select, SecurityOriginInfo, &m_dbInterface);
        typedef std::list<SecurityOriginInfo::Row> RowList;
        RowList rowList = select.GetRowList();

        FOREACH(it, rowList) {
            Origin origin(it->Get_scheme(), it->Get_host(), it->Get_port());
            list.push_back(
                SecurityOriginDataPtr(
                    new SecurityOriginData(
                        static_cast<Feature>(it->Get_feature()), origin)));
        }
        return list;
    }
    SQL_CONNECTION_EXCEPTION_HANDLER_END("Failed to get data  list")
}

Result SecurityOriginDAO::getResult(const SecurityOriginData &data)
{
    SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
    {
        SECURITY_ORIGIN_DB_SELECT(select, SecurityOriginInfo, &m_dbInterface);
        Equals<SecurityOriginInfo::feature> eFeature(data.feature);
        Equals<SecurityOriginInfo::scheme> eScheme(data.origin.scheme);
        Equals<SecurityOriginInfo::host> eHost(data.origin.host);
        Equals<SecurityOriginInfo::port> ePort(data.origin.port);
        select.Where(And(And(And(eFeature, eScheme), eHost), ePort));
        SecurityOriginInfoRowList rows = select.GetRowList();
        if (rows.empty()) {
            return RESULT_UNKNOWN;
        }
        SecurityOriginInfoRow row = rows.front();
        return static_cast<Result>(row.Get_result());
    }
    SQL_CONNECTION_EXCEPTION_HANDLER_END("getResult error")
}

bool SecurityOriginDAO::isReadOnly(const SecurityOriginData &data)
{
    SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
    {
        SECURITY_ORIGIN_DB_SELECT(select, SecurityOriginInfo, &m_dbInterface);
        Equals<SecurityOriginInfo::feature> eFeature(data.feature);
        Equals<SecurityOriginInfo::scheme> eScheme(data.origin.scheme);
        Equals<SecurityOriginInfo::host> eHost(data.origin.host);
        Equals<SecurityOriginInfo::port> ePort(data.origin.port);
        select.Where(And(And(And(eFeature, eScheme), eHost), ePort));
        SecurityOriginInfoRowList rows = select.GetRowList();
        if (rows.empty()) {
            return RESULT_UNKNOWN;
        }
        SecurityOriginInfoRow row = rows.front();
        return row.Get_readonly() ? true : false;
    }
    SQL_CONNECTION_EXCEPTION_HANDLER_END("isReadOnly error")
}

void SecurityOriginDAO::setSecurityOriginData(const SecurityOriginData &data,
                                              const Result result,
                                              const bool readOnly)
{
    if (true == hasResult(data)) {
        updateData(data, result, readOnly);
    } else {
        insertData(data, result, readOnly);
    }
}

void SecurityOriginDAO::setPrivilegeSecurityOriginData(
    const Feature feature,
    bool isOnlyAllowedLocalOrigin)
{
    //TODO: this breaks app:// scheme code -> no case for app scheme
    Origin origin(DPL::FromUTF8String("file"),
                  DPL::FromUTF8String(""),
                  0);
    if (!isOnlyAllowedLocalOrigin) {
        origin.scheme = DPL::FromUTF8String("");
    }
    SecurityOriginData data(feature, origin);
    setSecurityOriginData(data, RESULT_ALLOW_ALWAYS, true);
}

void SecurityOriginDAO::removeSecurityOriginData(
    const SecurityOriginData &data)
{
    if (false == hasResult(data)) {
        // There is no data
        return;
    }

    SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
    {
        ScopedTransaction transaction(&m_dbInterface);
        SECURITY_ORIGIN_DB_DELETE(del, SecurityOriginInfo, &m_dbInterface)
        Equals<SecurityOriginInfo::feature> eFeature(data.feature);
        Equals<SecurityOriginInfo::scheme> eScheme(data.origin.scheme);
        Equals<SecurityOriginInfo::host> eHost(data.origin.host);
        Equals<SecurityOriginInfo::port> ePort(data.origin.port);
        del.Where(And(And(And(eFeature, eScheme), eHost), ePort));
        del.Execute();
        transaction.Commit();
    }
    SQL_CONNECTION_EXCEPTION_HANDLER_END("Fail to remove security origin data")
}

void SecurityOriginDAO::removeSecurityOriginData(const Result result)
{
    SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
    {
        ScopedTransaction transaction(&m_dbInterface);
        SECURITY_ORIGIN_DB_DELETE(del, SecurityOriginInfo, &m_dbInterface)
        del.Where(Equals<SecurityOriginInfo::result>(result));
        del.Execute();
        transaction.Commit();
    }
    SQL_CONNECTION_EXCEPTION_HANDLER_END("Fail to remove data by result")
}

bool SecurityOriginDAO::hasResult(const SecurityOriginData &data)
{
    Result ret = getResult(data);
    return (ret != RESULT_UNKNOWN);
}

void SecurityOriginDAO::insertData(const SecurityOriginData &data,
                                   const Result result,
                                   const bool readOnly)
{
    SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
    {
        SecurityOriginInfoRow row;
        row.Set_feature(data.feature);
        row.Set_scheme(data.origin.scheme);
        row.Set_host(data.origin.host);
        row.Set_port(data.origin.port);
        row.Set_result(result);
        row.Set_readonly(readOnly ? 1 : 0);

        ScopedTransaction transaction(&m_dbInterface);
        SECURITY_ORIGIN_DB_INSERT(insert, SecurityOriginInfo, &m_dbInterface);
        insert.Values(row);
        insert.Execute();
        transaction.Commit();
    }
    SQL_CONNECTION_EXCEPTION_HANDLER_END("Fail to insert")
}

void SecurityOriginDAO::updateData(const SecurityOriginData &data,
                                   const Result result,
                                   const bool readOnly)
{
    SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
    {
        SecurityOriginInfoRow row;
        row.Set_feature(data.feature);
        row.Set_scheme(data.origin.scheme);
        row.Set_host(data.origin.host);
        row.Set_port(data.origin.port);
        row.Set_result(result);
        row.Set_readonly(readOnly ? 1 : 0);

        ScopedTransaction transaction(&m_dbInterface);
        SECURITY_ORIGIN_DB_UPDATE(update, SecurityOriginInfo, &m_dbInterface);
        Equals<SecurityOriginInfo::feature> eFeature(data.feature);
        Equals<SecurityOriginInfo::scheme> eScheme(data.origin.scheme);
        Equals<SecurityOriginInfo::host> eHost(data.origin.host);
        Equals<SecurityOriginInfo::port> ePort(data.origin.port);
        update.Where(And(And(And(eFeature, eScheme), eHost), ePort));
        update.Values(row);
        update.Execute();
        transaction.Commit();
    }
    SQL_CONNECTION_EXCEPTION_HANDLER_END("Fail to update")
}
#undef SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
#undef SQL_CONNECTION_EXCEPTION_HANDLER_END
} // namespace SecurityOriginDB
