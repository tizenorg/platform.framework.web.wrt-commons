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

#include <wrt-commons/security-origin-dao/security_origin_database.h>
#include <wrt-commons/security-origin-dao/security_origin_dao.h>
#include <wrt-commons/security-origin-dao/security_origin_dao_types.h>
#include <orm_generator_security_origin.h>
#include <dpl/foreach.h>
#include <dpl/wrt-dao-ro/widget_dao_read_only.h>
#include <dpl/wrt-dao-ro/WrtDatabase.h>
#include <dpl/wrt-dao-ro/widget_config.h>
#include <dpl/wrt-dao-ro/global_config.h>
#include <sys/stat.h>
#include <fstream>

using namespace DPL::DB::ORM;
using namespace DPL::DB::ORM::security_origin;

namespace SecurityOriginDB {

#define SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN          Try

#define SQL_CONNECTION_EXCEPTION_HANDLER_END(message)   \
    Catch(DPL::DB::SqlConnection::Exception::Base) {       \
        LogError(message);                              \
        ReThrowMsg(SecurityOriginDAO::Exception::DatabaseError, \
                   message);                            \
    }

namespace {
DPL::DB::SqlConnection::Flag::Option SECURITY_ORIGIN_DB_OPTION =
    DPL::DB::SqlConnection::Flag::RW;
DPL::DB::SqlConnection::Flag::Type SECURITY_ORIGIN_DB_TYPE =
    DPL::DB::SqlConnection::Flag::UseLucene;
const char* const SECURITY_ORIGIN_DB_NAME = ".security_origin.db";
const char* const SECURITY_ORIGIN_DB_SQL_PATH =
    "/usr/share/wrt-engine/security_origin_db.sql";

std::string createDatabasePath(int widgetHandle)
{
    using namespace DPL::DB::ORM;
    using namespace WrtDB::WidgetConfig;
    using namespace WrtDB::GlobalConfig;

    SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
    {
        std::stringstream filename;
        WrtDB::WidgetDAOReadOnly widgetDAO(widgetHandle);
        DPL::Optional<DPL::String> pkgname = widgetDAO.getPkgname();

        filename << GetWidgetPersistentStoragePath(*pkgname)
                 << "/"
                 << SECURITY_ORIGIN_DB_NAME;
        return filename.str();
    }
    SQL_CONNECTION_EXCEPTION_HANDLER_END("Fail to get database Path")

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
            LogInfo("Creating database " << databasePath);

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
        }
    }
    SQL_CONNECTION_EXCEPTION_HANDLER_END("Fail to get database Path")
}
}

SecurityOriginDAO::SecurityOriginDAO(int handle) :
    m_securityOriginDBPath(createDatabasePath(handle)),
    m_securityOriginDBInterface(m_securityOriginDBPath, SECURITY_ORIGIN_DB_TYPE)
{
    checkDatabase(m_securityOriginDBPath);
    m_securityOriginDBInterface.AttachToThread(SECURITY_ORIGIN_DB_OPTION);
}

SecurityOriginDAO::~SecurityOriginDAO()
{
    m_securityOriginDBInterface.DetachFromThread();
}

SecurityOriginDataList SecurityOriginDAO::getSecurityOriginDataList(void)
{
    SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
    {
        SecurityOriginDataList list;
        SECURITY_ORIGIN_DB_SELECT(select, SecurityOriginInfo, &m_securityOriginDBInterface);
        typedef std::list<SecurityOriginInfo::Row> RowList;
        RowList rowList = select->GetRowList();

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

Result SecurityOriginDAO::getResult(const SecurityOriginData &securityOriginData)
{
    SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
    {
        SECURITY_ORIGIN_DB_SELECT(select, SecurityOriginInfo, &m_securityOriginDBInterface);
        select->Where(
            And(And(And(Equals<SecurityOriginInfo::feature>(securityOriginData.feature),
                        Equals<SecurityOriginInfo::scheme>(securityOriginData.origin.scheme)),
                    Equals<SecurityOriginInfo::host>(securityOriginData.origin.host)),
                Equals<SecurityOriginInfo::port>(securityOriginData.origin.port)));
        SecurityOriginInfo::Select::RowList rows = select->GetRowList();

        if (rows.empty()) {
            return RESULT_UNKNOWN;
        }
        SecurityOriginInfo::Row row = rows.front();
        return static_cast<Result>(row.Get_result());
    }
    SQL_CONNECTION_EXCEPTION_HANDLER_END("Failed to get result for security origin")
}

void SecurityOriginDAO::setSecurityOriginData(const SecurityOriginData &securityOriginData,
                                              const Result result)
{
    SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
    {
        ScopedTransaction transaction(&m_securityOriginDBInterface);
        SecurityOriginInfo::Row row;
        row.Set_feature(securityOriginData.feature);
        row.Set_scheme(securityOriginData.origin.scheme);
        row.Set_host(securityOriginData.origin.host);
        row.Set_port(securityOriginData.origin.port);
        row.Set_result(result);

        if (true == hasResult(securityOriginData)) {
            SECURITY_ORIGIN_DB_UPDATE(update,
                               SecurityOriginInfo,
                               &m_securityOriginDBInterface);
            update->Values(row);
            update->Execute();
        } else {
            SECURITY_ORIGIN_DB_INSERT(
                insert,
                SecurityOriginInfo,
                &m_securityOriginDBInterface);
            insert->Values(row);
            insert->Execute();
        }
        transaction.Commit();
    }
    SQL_CONNECTION_EXCEPTION_HANDLER_END("Fail to set security origin data")
}

void SecurityOriginDAO::removeSecurityOriginData(
    const SecurityOriginData &securityOriginData)
{
    SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
    {
        ScopedTransaction transaction(&m_securityOriginDBInterface);

        if (true == hasResult(securityOriginData)) {
            SECURITY_ORIGIN_DB_DELETE(del, SecurityOriginInfo, &m_securityOriginDBInterface)
            del->Where(
                And(And(And(Equals<SecurityOriginInfo::feature>(securityOriginData.feature),
                            Equals<SecurityOriginInfo::scheme>(securityOriginData.origin.scheme)),
                        Equals<SecurityOriginInfo::host>(securityOriginData.origin.host)),
                    Equals<SecurityOriginInfo::port>(securityOriginData.origin.port)));
            del->Execute();
            transaction.Commit();
        }
    }
    SQL_CONNECTION_EXCEPTION_HANDLER_END("Fail to set security origin data")
}

bool SecurityOriginDAO::hasResult(const SecurityOriginData &securityOriginData)
{
    SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
    {
        SECURITY_ORIGIN_DB_SELECT(select, SecurityOriginInfo, &m_securityOriginDBInterface);
        select->Where(
            And(And(And(Equals<SecurityOriginInfo::feature>(securityOriginData.feature),
                        Equals<SecurityOriginInfo::scheme>(securityOriginData.origin.scheme)),
                    Equals<SecurityOriginInfo::host>(securityOriginData.origin.host)),
                Equals<SecurityOriginInfo::port>(securityOriginData.origin.port)));

        SecurityOriginInfo::Select::RowList rows = select->GetRowList();

        return !rows.empty();
    }
    SQL_CONNECTION_EXCEPTION_HANDLER_END("Failed to get result for security origin")
}

#undef SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
#undef SQL_CONNECTION_EXCEPTION_HANDLER_END

} // namespace SecurityOriginDB
