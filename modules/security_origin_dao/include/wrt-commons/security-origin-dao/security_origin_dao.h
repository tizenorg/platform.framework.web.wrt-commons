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
 * @file    securoty_origin_dao.h
 * @author  Jihoon Chung (jihoon.chung@samsung.com)
 * @version 1.0
 * @brief   This file contains the declaration of security origin dao
 */
#ifndef _SECURITY_ORIGIN_DAO_H_
#define _SECURITY_ORIGIN_DAO_H_

#include <dpl/db/thread_database_support.h>
#include <wrt-commons/security-origin-dao/security_origin_dao_types.h>

namespace SecurityOriginDB {

class SecurityOriginDAO
{
  public:
    class Exception
    {
      public:
        DECLARE_EXCEPTION_TYPE(DPL::Exception, Base)
        DECLARE_EXCEPTION_TYPE(Base, DatabaseError)
        DECLARE_EXCEPTION_TYPE(Base, DataNotExist)
    };

    explicit SecurityOriginDAO(int handle);
    virtual ~SecurityOriginDAO();
    Result getResult(const SecurityOriginData &securityOriginData);
    void setSecurityOriginData(const SecurityOriginData &securityOriginData,
                               const Result result);
  private:
    std::string m_securityOriginDBPath;
    DPL::DB::ThreadDatabaseSupport m_securityOriginDBInterface;
    bool hasResult(const SecurityOriginData &securityOriginData);
};

} // namespace SecurityOriginDB

#endif // _SECURITY_ORIGIN_DAO_H_
