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
 * @file       certificate_dao.h
 * @author  Leerang Song (leerang.song@samsung.com)
 * @version 1.0
 * @brief    This file contains the declaration of certificate dao
 */
#ifndef _CERTIFICATE_DAO_H_
#define _CERTIFICATE_DAO_H_

#include <dpl/db/thread_database_support.h>
#include <wrt-commons/certificate-dao/certificate_dao_types.h>
#include <dpl/wrt-dao-ro/common_dao_types.h>

namespace CertificateDB {
class CertificateDAO
{
  public:
    class Exception
    {
      public:
        DECLARE_EXCEPTION_TYPE(DPL::Exception, Base)
        DECLARE_EXCEPTION_TYPE(Base, DatabaseError)
        DECLARE_EXCEPTION_TYPE(Base, DataNotExist)
    };

    explicit CertificateDAO(const WrtDB::TizenPkgId &pkgName);
    virtual ~CertificateDAO();
    CertificateDataList getCertificateDataList();
    Result getResult(const CertificateData &certificateData);
    void setCertificateData(const CertificateData &certificateData,
        const Result result);
    void removeCertificateData(const CertificateData &certificateData);
    void removeCertificateData(const Result result);

  private:
    std::string m_certificateDBPath;
    DPL::DB::ThreadDatabaseSupport m_certificateDBInterface;
    bool hasResult(const CertificateData &certificateData);
};

typedef std::shared_ptr<CertificateDAO> CertificateDAOPtr;
} // namespace CertificateDB

#endif // _CERTIFICATE_DAO_H_