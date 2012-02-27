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
 * @file
 * @author      Bartlomiej Grzelewski (b.grzelewski@samsung.com)
 * @version     1.0
 * @brief
 */
#ifndef _WRT_ENGINE_SRC_INSTALLER_CORE_VALIDATION_CORE_CERTIFICATE_H_
#define _WRT_ENGINE_SRC_INSTALLER_CORE_VALIDATION_CORE_CERTIFICATE_H_

#include <list>
#include <string>
#include <set>
#include <vector>
#include <ctime>

#include <openssl/x509.h>

#include <dpl/exception.h>
#include <dpl/noncopyable.h>
#include <dpl/shared_ptr.h>
#include <dpl/enable_shared_from_this.h>
#include <dpl/optional.h>
#include <dpl/optional_typedefs.h>
#include <dpl/string.h>

#include <cert-service.h>

namespace ValidationCore {

// from OpenSSL asn1/a_utctm.c code
int asn1TimeToTimeT(ASN1_TIME *t,
                    time_t *res);


int asn1GeneralizedTimeToTimeT(ASN1_GENERALIZEDTIME *tm,
                               time_t *res);

class Certificate;

typedef DPL::SharedPtr<Certificate> CertificatePtr;
typedef std::list<CertificatePtr> CertificateList;

class Certificate : public DPL::EnableSharedFromThis<Certificate>
{
  public:
    typedef std::vector<unsigned char> Fingerprint;
    typedef DPL::String AltName;
    typedef std::set<AltName> AltNameSet;

    enum FingerprintType
    {
        FINGERPRINT_MD5,
        FINGERPRINT_SHA1
    };
    enum FieldType
    {
        FIELD_ISSUER,
        FIELD_SUBJECT
    };

    enum FormType
    {
        FORM_DER,
        FORM_BASE64
    };

    class Exception
    {
      public:
        DECLARE_EXCEPTION_TYPE(DPL::Exception, Base)
        DECLARE_EXCEPTION_TYPE(Base, OpensslInternalError)
    };

    explicit Certificate(X509 *cert);

    explicit Certificate(cert_svc_mem_buff &buffer);

    explicit Certificate(const std::string &der,
                         FormType form = FORM_DER);

    ~Certificate();

    // It returns pointer to internal structure!
    // Do not free this pointer!
    X509 *getX509(void) const;

    std::string getDER(void) const;

    std::string getBase64(void) const;

    // This const is cheating here because you have no
    // guarantee that X509_get_subject_name will not
    // change X509 object.
    bool isSignedBy(const CertificatePtr &parent) const;

    Fingerprint getFingerprint(FingerprintType type) const;

    DPL::OptionalString getCommonName(FieldType type = FIELD_SUBJECT) const;
    DPL::OptionalString getCountryName(FieldType type = FIELD_SUBJECT) const;
    DPL::OptionalString getStateOrProvinceName(
            FieldType type = FIELD_SUBJECT) const;
    DPL::OptionalString getLocalityName(FieldType type = FIELD_SUBJECT) const;
    DPL::OptionalString getOrganizationName(
            FieldType type = FIELD_SUBJECT) const;
    DPL::OptionalString getOrganizationalUnitName(
            FieldType type = FIELD_SUBJECT) const;
    DPL::OptionalString getOCSPURL() const;

    // Openssl supports 9 types of alternative name filed.
    // 4 of them are "string similar" types so it is possible
    // to create more generic function.
    AltNameSet getAlternativeNameDNS() const;

    time_t getNotAfter() const;

    /**
     * @brief This is convenient function.
     *
     * @details It can't be const function (however it doesn't change internal
     * object). For details see #isSignedBy() function description.
     */
    bool isRootCert();

    /**
     * @brief Gets list of CRL distribution's points URIs
     */
    std::list<std::string> getCrlUris() const;

  protected:
    DPL::OptionalString getField(FieldType type,
                            int fieldNid) const;

    X509 *m_x509;
};
} // namespace ValidationCore

#endif
