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
 * @file    certificate_dao.cpp
 * @author  Leerang Song (leerang.song@samsung.com)
 * @version 1.0
 * @brief    This file contains the definition of certificate dao class.
 */

#include <wrt-commons/certificate-dao/certificate_dao.h>
#include <wrt-commons/certificate-dao/certificate_database.h>
#include <wrt-commons/certificate-dao/certificate_dao_types.h>
#include <orm_generator_certificate.h>
#include <dpl/foreach.h>
#include <dpl/wrt-dao-ro/widget_dao_read_only.h>
#include <dpl/wrt-dao-ro/WrtDatabase.h>
#include <dpl/wrt-dao-ro/widget_config.h>
#include <dpl/wrt-dao-ro/global_config.h>
#include <dpl/wrt-dao-ro/common_dao_types.h>
#include <sys/stat.h>
#include <fstream>
/* GCC versions 4.7 had changes to the C++ standard. It
 * no longer includes <unistd.h> to remove namespace pollution.
 */
#include <unistd.h>

using namespace DPL::DB::ORM;
using namespace DPL::DB::ORM::certificate;

namespace CertificateDB {
#define SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN          Try

#define SQL_CONNECTION_EXCEPTION_HANDLER_END(message)   \
    Catch(DPL::DB::SqlConnection::Exception::Base) {       \
        LogError(message);                              \
        ReThrowMsg(CertificateDAO::Exception::DatabaseError, \
                   message);                            \
    }

namespace {
DPL::DB::SqlConnection::Flag::Option CERTIFICATE_DB_OPTION =
    DPL::DB::SqlConnection::Flag::RW;
DPL::DB::SqlConnection::Flag::Type CERTIFICATE_DB_TYPE =
    DPL::DB::SqlConnection::Flag::UseLucene;
const char* const CERTIFICATE_DB_NAME = ".certificate.db";
const char* const CERTIFICATE_DB_SQL_PATH =
    "/usr/share/wrt-engine/certificate_db.sql";
const char* const CERTIFICATE_DATABASE_JOURNAL_FILENAME = "-journal";

const int WEB_APPLICATION_UID = 5000;
const int WEB_APPLICATION_GUID = 5000;

std::string createDatabasePath(const WrtDB::TizenPkgId &pkgName)
{
    std::stringstream filename;

    filename << WrtDB::WidgetConfig::GetWidgetPersistentStoragePath(pkgName)
             << "/"
             << CERTIFICATE_DB_NAME;
    return filename.str();
}

void checkDatabase(std::string databasePath)
{
    SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
    {
        if (databasePath.empty()) {
            ThrowMsg(CertificateDAO::Exception::DatabaseError,
                     "Wrong database Path is passed");
        }

        struct stat buffer;
        if (stat(databasePath.c_str(), &buffer) != 0) {
            //Create fresh database
            LogDebug("Creating database " << databasePath);

            std::fstream file;
            file.open(CERTIFICATE_DB_SQL_PATH, std::ios_base::in);
            if (!file) {
                ThrowMsg(CertificateDAO::Exception::DatabaseError,
                         "Fail to get database Path");
            }

            std::stringstream ssBuffer;
            ssBuffer << file.rdbuf();

            file.close();

            DPL::DB::SqlConnection con(databasePath,
                                       CERTIFICATE_DB_TYPE,
                                       CERTIFICATE_DB_OPTION);
            con.ExecCommand(ssBuffer.str().c_str());
        }

        if(chown(databasePath.c_str(),
                 WEB_APPLICATION_UID,
                 WEB_APPLICATION_GUID) != 0)
        {
            ThrowMsg(CertificateDAO::Exception::DatabaseError,
                 "Fail to change uid/guid");
        }
        std::string databaseJournal =
            databasePath + CERTIFICATE_DATABASE_JOURNAL_FILENAME;
        if(chown(databaseJournal.c_str(),
                 WEB_APPLICATION_UID,
                 WEB_APPLICATION_GUID) != 0)
        {
            ThrowMsg(CertificateDAO::Exception::DatabaseError,
                 "Fail to change uid/guid");
        }
    }
    SQL_CONNECTION_EXCEPTION_HANDLER_END("Fail to get database Path")
}
}

CertificateDAO::CertificateDAO(const WrtDB::TizenPkgId &pkgName) :
    m_certificateDBPath(createDatabasePath(pkgName)),
    m_certificateDBInterface(m_certificateDBPath, CERTIFICATE_DB_TYPE)
{
    checkDatabase(m_certificateDBPath);
    m_certificateDBInterface.AttachToThread(CERTIFICATE_DB_OPTION);
}

CertificateDAO::~CertificateDAO()
{
    m_certificateDBInterface.DetachFromThread();
}

CertificateDataList CertificateDAO::getCertificateDataList(void)
{
    SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
    {
        CertificateDataList list;
        CERTIFICATE_DB_SELECT(select,
            CertificateInfo,
            &m_certificateDBInterface);
        typedef std::list<CertificateInfo::Row> RowList;
        RowList rowList = select->GetRowList();

        FOREACH(it, rowList) {
            list.push_back(
                CertificateDataPtr(
                    new CertificateData(it->Get_certificate())));
        }
        return list;
    }
    SQL_CONNECTION_EXCEPTION_HANDLER_END("Failed to get data  list")
}

Result CertificateDAO::getResult(
    const CertificateData &certificateData)
{
    SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
    {
        CERTIFICATE_DB_SELECT(select,
            CertificateInfo,
            &m_certificateDBInterface);
        select->Where(
            Equals<CertificateInfo::certificate>(certificateData.certificate));
        CertificateInfo::Select::RowList rows = select->GetRowList();

        if (rows.empty()) {
            return RESULT_UNKNOWN;
        }
        CertificateInfo::Row row = rows.front();
        return static_cast<Result>(row.Get_result());
    }
    SQL_CONNECTION_EXCEPTION_HANDLER_END(
        "Failed to get result for security certiInfo")
}

void CertificateDAO::setCertificateData(const CertificateData &certificateData,
    const Result result)
{
    SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
    {
        ScopedTransaction transaction(&m_certificateDBInterface);
        CertificateInfo::Row row;
        row.Set_certificate(certificateData.certificate);
        row.Set_result(result);

        if (true == hasResult(certificateData)) {
            CERTIFICATE_DB_UPDATE(update,
                CertificateInfo,
                &m_certificateDBInterface);
            update->Values(row);
            update->Execute();
        } else {
            CERTIFICATE_DB_INSERT(insert,
                CertificateInfo,
                &m_certificateDBInterface);
            insert->Values(row);
            insert->Execute();
        }
        transaction.Commit();
    }
    SQL_CONNECTION_EXCEPTION_HANDLER_END("Fail to set security certiInfo data")
}

void CertificateDAO::removeCertificateData(
    const CertificateData &certificateData)
{
    SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
    {
        ScopedTransaction transaction(&m_certificateDBInterface);

        if (true == hasResult(certificateData)) {
            CERTIFICATE_DB_DELETE(del,
                CertificateInfo,
                &m_certificateDBInterface)
            del->Where(
                Equals<CertificateInfo::certificate>(certificateData.certificate));
            del->Execute();
            transaction.Commit();
        }
    }
    SQL_CONNECTION_EXCEPTION_HANDLER_END("Fail to set certiInfo data")
}

void CertificateDAO::removeCertificateData(const Result result)
{
    SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
    {
        ScopedTransaction transaction(&m_certificateDBInterface);
        CERTIFICATE_DB_DELETE(del,
                                  CertificateInfo,
                                  &m_certificateDBInterface)
        del->Where(Equals<CertificateInfo::result>(result));
        del->Execute();
        transaction.Commit();
    }
    SQL_CONNECTION_EXCEPTION_HANDLER_END("Fail to remove data by result")
}

bool CertificateDAO::hasResult(const CertificateData &certificateData)
{
    Result res = getResult(certificateData);
    return (res != RESULT_UNKNOWN);
}

#undef SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
#undef SQL_CONNECTION_EXCEPTION_HANDLER_END
} // namespace CertificateDB
