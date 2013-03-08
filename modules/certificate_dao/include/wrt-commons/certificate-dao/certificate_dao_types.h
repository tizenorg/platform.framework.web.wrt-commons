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
 * @file    certificate_dao_types.h
 * @author  Leerang Song (leerang.song@samsung.com)
 * @version 1.0
 * @brief This file contains the declaration of
 *           common data types for certificate database.
 */
#ifndef _CERTIFICATE_DAO_TYPES_H_
#define _CERTIFICATE_DAO_TYPES_H_

#include <list>
#include <memory>
#include <dpl/string.h>

namespace CertificateDB {

enum Result
{
    RESULT_UNKNOWN = 0,
    RESULT_ALLOW_ONCE,
    RESULT_DENY_ONCE,
    RESULT_ALLOW_ALWAYS,
    RESULT_DENY_ALWAYS
};

struct CertificateData
{
    DPL::String certificate;

    CertificateData(const DPL::String& certi) :
        certificate(certi)
    {}

    bool operator== (const CertificateData& other) const
    {
        return !(certificate == other.certificate);
    }

    bool operator!= (const CertificateData& other) const
    {
        return !(*this == other);
    }
};

typedef std::shared_ptr<CertificateData> CertificateDataPtr;
typedef std::list<CertificateDataPtr> CertificateDataList;
} // namespace CertificateDB

#endif // _CERTIFICATE_DAO_TYPES_H_