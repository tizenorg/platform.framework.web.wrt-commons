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
#include "Certificate.h"

#include <openssl/x509v3.h>

#include <dpl/assert.h>
#include <dpl/log/log.h>

#include <Base64.h>

namespace ValidationCore {

int asn1TimeToTimeT(ASN1_TIME *t,
                    time_t *res)
{
    struct tm tm;
    int offset;

    (*res) = 0;
    if (!ASN1_TIME_check(t)) {
        return -1;
    }

    memset(&tm, 0, sizeof(tm));

#define g2(p) (((p)[0] - '0') * 10 + (p)[1] - '0')
    if (t->type == V_ASN1_UTCTIME) {
        Assert(t->length > 12);

        /*   this code is copied from OpenSSL asn1/a_utctm.c file */
        tm.tm_year = g2(t->data);
        if (tm.tm_year < 50) {
            tm.tm_year += 100;
        }
        tm.tm_mon = g2(t->data + 2) - 1;
        tm.tm_mday = g2(t->data + 4);
        tm.tm_hour = g2(t->data + 6);
        tm.tm_min = g2(t->data + 8);
        tm.tm_sec = g2(t->data + 10);
        if (t->data[12] == 'Z') {
            offset = 0;
        } else {
            Assert(t->length > 16);

            offset = g2(t->data + 13) * 60 + g2(t->data + 15);
            if (t->data[12] == '-') {
                offset = -offset;
            }
        }
        tm.tm_isdst = -1;
    } else {
        Assert(t->length > 14);

        tm.tm_year = g2(t->data) * 100 + g2(t->data + 2);
        tm.tm_mon = g2(t->data + 4) - 1;
        tm.tm_mday = g2(t->data + 6);
        tm.tm_hour = g2(t->data + 8);
        tm.tm_min = g2(t->data + 10);
        tm.tm_sec = g2(t->data + 12);
        if (t->data[14] == 'Z') {
            offset = 0;
        } else {
            Assert(t->length > 18);

            offset = g2(t->data + 15) * 60 + g2(t->data + 17);
            if (t->data[14] == '-') {
                offset = -offset;
            }
        }
        tm.tm_isdst = -1;
    }
#undef g2
    (*res) = timegm(&tm) - offset * 60;
    return 0;
}

int asn1GeneralizedTimeToTimeT(ASN1_GENERALIZEDTIME *tm,
                               time_t *res)
{
    /*
     * This code is based on following assumption:
     * from openssl/a_gentm.c:
     * GENERALIZEDTIME is similar to UTCTIME except the year is
     * represented as YYYY. This stuff treats everything as a two digit
     * field so make first two fields 00 to 99
     */
    const int DATE_BUFFER_LENGTH = 15; // YYYYMMDDHHMMSSZ

    if (NULL == res || NULL == tm) {
        LogError("NULL pointer");
        return -1;
    }

    if (DATE_BUFFER_LENGTH != tm->length || NULL == tm->data) {
        LogError("Invalid ASN1_GENERALIZEDTIME");
        return -1;
    }

    struct tm time_s;
    if (sscanf ((char*)tm->data,
                "%4d%2d%2d%2d%2d%2d",
                &time_s.tm_year,
                &time_s.tm_mon,
                &time_s.tm_mday,
                &time_s.tm_hour,
                &time_s.tm_min,
                &time_s.tm_sec) < 6)
    {
        LogError("Could not extract time data from ASN1_GENERALIZEDTIME");
        return -1;
    }

    time_s.tm_year -= 1900;
    time_s.tm_mon -= 1;
    time_s.tm_isdst = 0;   // UTC
    time_s.tm_gmtoff = 0;  // UTC
    time_s.tm_zone = NULL; // UTC

    *res = mktime(&time_s);

    return 0;
}

Certificate::Certificate(X509 *cert)
{
    Assert(cert);
    m_x509 = X509_dup(cert);
    if (!m_x509) {
        LogWarning("Internal Openssl error in d2i_X509 function.");
        ThrowMsg(Exception::OpensslInternalError,
                 "Internal Openssl error in d2i_X509 function.");
    }
}

Certificate::Certificate(cert_svc_mem_buff &buffer)
{
    Assert(buffer.data);
    const unsigned char *ptr = buffer.data;
    m_x509 = d2i_X509(NULL, &ptr, buffer.size);
    if (!m_x509) {
        LogWarning("Internal Openssl error in d2i_X509 function.");
        ThrowMsg(Exception::OpensslInternalError,
                 "Internal Openssl error in d2i_X509 function.");
    }
}

Certificate::Certificate(const std::string &der,
                         Certificate::FormType form)
{
    Assert(der.size());

    int size;
    const unsigned char *ptr;
    std::string tmp;

    if (FORM_BASE64 == form) {
        Base64Decoder base64;
        base64.reset();
        base64.append(der);
        base64.finalize();
        tmp = base64.get();
        ptr = reinterpret_cast<const unsigned char*>(tmp.c_str());
        size = static_cast<int>(tmp.size());
    } else {
        ptr = reinterpret_cast<const unsigned char*>(der.c_str());
        size = static_cast<int>(der.size());
    }

    m_x509 = d2i_X509(NULL, &ptr, size);
    if (!m_x509) {
        LogError("Internal Openssl error in d2i_X509 function.");
        ThrowMsg(Exception::OpensslInternalError,
                 "Internal Openssl error in d2i_X509 function.");
    }
}

Certificate::~Certificate()
{
    X509_free(m_x509);
}

X509* Certificate::getX509(void) const
{
    return m_x509;
}

std::string Certificate::getDER(void) const
{
    unsigned char *rawDer = NULL;
    int size = i2d_X509(m_x509, &rawDer);
    if (!rawDer || size <= 0) {
        LogError("i2d_X509 failed");
        ThrowMsg(Exception::OpensslInternalError,
                 "i2d_X509 failed");
    }

    std::string output(reinterpret_cast<char*>(rawDer), size);
    OPENSSL_free(rawDer);
    return output;
}

std::string Certificate::getBase64(void) const
{
    Base64Encoder base64;
    base64.reset();
    base64.append(getDER());
    base64.finalize();
    return base64.get();
}

bool Certificate::isSignedBy(const CertificatePtr &parent) const
{
    if (!parent) {
        LogDebug("Invalid certificate parameter.");
        return false;
    }
    return 0 == X509_NAME_cmp(X509_get_subject_name(parent->m_x509),
                              X509_get_issuer_name(m_x509));
}

Certificate::Fingerprint Certificate::getFingerprint(
        Certificate::FingerprintType type) const
{
    size_t fingerprintlength = EVP_MAX_MD_SIZE;
    unsigned char fingerprint[EVP_MAX_MD_SIZE];
    Fingerprint raw;

    if (type == FINGERPRINT_MD5) {
        if (!X509_digest(m_x509, EVP_md5(), fingerprint, &fingerprintlength)) {
            LogError("MD5 digest counting failed!");
            ThrowMsg(Exception::OpensslInternalError,
                     "MD5 digest counting failed!");
        }
    }

    if (type == FINGERPRINT_SHA1) {
        if (!X509_digest(m_x509, EVP_sha1(), fingerprint,
                         &fingerprintlength))
        {
            LogError("SHA1 digest counting failed");
            ThrowMsg(Exception::OpensslInternalError,
                     "SHA1 digest counting failed!");
        }
    }

    raw.resize(fingerprintlength); // improve performance
    std::copy(fingerprint, fingerprint + fingerprintlength, raw.begin());

    return raw;
}

DPL::OptionalString Certificate::getField(FieldType type,
                                     int fieldNid) const
{
    X509_NAME *subjectName = NULL;
    switch (type) {
    case FIELD_ISSUER:
        subjectName = X509_get_issuer_name(m_x509);
        break;
    case FIELD_SUBJECT:
        subjectName = X509_get_subject_name(m_x509);
        break;
    default:
        Assert("Invalid type");
    }

    if (!subjectName) {
        LogError("Error during subject name extraction.");
        ThrowMsg(Exception::OpensslInternalError,
                 "Error during subject name extraction.");
    }

    X509_NAME_ENTRY *subjectEntry = NULL;

    DPL::Optional < DPL::String > output;

    int entryCount = X509_NAME_entry_count(subjectName);

    for (int i = 0; i < entryCount; ++i) {
        subjectEntry = X509_NAME_get_entry(subjectName,
                                           i);

        if (!subjectEntry) {
            continue;
        }

        int nid = OBJ_obj2nid(
            static_cast<ASN1_OBJECT*>(
                    X509_NAME_ENTRY_get_object(subjectEntry)));

        if (nid != fieldNid) {
            continue;
        }

        ASN1_STRING* pASN1Str = subjectEntry->value;

        unsigned char* pData = NULL;
        int nLength = ASN1_STRING_to_UTF8(&pData,
                                          pASN1Str);

        if (nLength < 0) {
            LogError("Reading field error.");
            ThrowMsg(Exception::OpensslInternalError,
                     "Reading field error.");
        }

        std::string strEntry(reinterpret_cast<char*>(pData),
                             nLength);
        output = DPL::FromUTF8String(strEntry);
        OPENSSL_free(pData);
    }
    return output;
}

DPL::OptionalString Certificate::getCommonName(FieldType type) const
{
    return getField(type, NID_commonName);
}

DPL::OptionalString Certificate::getCountryName(FieldType type) const
{
    return getField(type, NID_countryName);
}

DPL::OptionalString Certificate::getStateOrProvinceName(FieldType type) const
{
    return getField(type, NID_stateOrProvinceName);
}

DPL::OptionalString Certificate::getLocalityName(FieldType type) const
{
    return getField(type, NID_localityName);
}

DPL::OptionalString Certificate::getOrganizationName(FieldType type) const
{
    return getField(type, NID_organizationName);
}

DPL::OptionalString Certificate::getOrganizationalUnitName(FieldType type) const
{
    return getField(type, NID_organizationalUnitName);
}

DPL::OptionalString Certificate::getOCSPURL() const
{
    // TODO verify this code
    DPL::OptionalString retValue;
    AUTHORITY_INFO_ACCESS *aia = static_cast<AUTHORITY_INFO_ACCESS*>(
            X509_get_ext_d2i(m_x509,
                             NID_info_access,
                             NULL,
                             NULL));

    // no AIA extension in the cert
    if (NULL == aia) {
        return retValue;
    }

    int count = sk_ACCESS_DESCRIPTION_num(aia);

    for (int i = 0; i < count; ++i) {
        ACCESS_DESCRIPTION* ad = sk_ACCESS_DESCRIPTION_value(aia, i);

        if (OBJ_obj2nid(ad->method) == NID_ad_OCSP &&
            ad->location->type == GEN_URI)
        {
            void* data = ASN1_STRING_data(ad->location->d.ia5);
            retValue = DPL::OptionalString(DPL::FromUTF8String(
                    static_cast<char*>(data)));

            break;
        }
    }
    sk_ACCESS_DESCRIPTION_free(aia);
    return retValue;
}

Certificate::AltNameSet Certificate::getAlternativeNameDNS() const
{
    AltNameSet set;

    GENERAL_NAME *namePart = NULL;

    STACK_OF(GENERAL_NAME)* san =
        static_cast<STACK_OF(GENERAL_NAME)*>(
            X509_get_ext_d2i(m_x509,NID_subject_alt_name,NULL,NULL));

    while (sk_GENERAL_NAME_num(san) > 0) {
        namePart = sk_GENERAL_NAME_pop(san);
        if (GEN_DNS == namePart->type) {
            std::string temp =
                reinterpret_cast<char*>(ASN1_STRING_data(namePart->d.dNSName));
            DPL::String altDNSName = DPL::FromASCIIString(temp);
            set.insert(altDNSName);
            LogDebug("FOUND GEN_DNS: " << temp);
        } else {
            LogDebug("FOUND GEN TYPE ID: " << namePart->type);
        }
    }
    return set;
}

time_t Certificate::getNotAfter() const
{
    ASN1_TIME *time = X509_get_notAfter(m_x509);
    if (!time) {
        LogError("Reading Not After error.");
        ThrowMsg(Exception::OpensslInternalError, "Reading Not After error.");
    }
    time_t output;
    if (asn1TimeToTimeT(time, &output)) {
        LogError("Converting ASN1_time to time_t error.");
        ThrowMsg(Exception::OpensslInternalError,
                 "Converting ASN1_time to time_t error.");
    }
    return output;
}

bool Certificate::isRootCert()
{
    // based on that root certificate has the same subject as issuer name
    return isSignedBy(this->SharedFromThis());
}

std::list<std::string>
Certificate::getCrlUris() const
{
    std::list<std::string> result;

    STACK_OF(DIST_POINT)* distPoints =
        static_cast<STACK_OF(DIST_POINT)*>(
            X509_get_ext_d2i(
                getX509(),
                NID_crl_distribution_points,
                NULL,
                NULL));
    if (!distPoints) {
        LogDebug("No distribution points in certificate.");
        return result;
    }

    int count = sk_DIST_POINT_num(distPoints);
    for (int i = 0; i < count; ++i) {
        DIST_POINT* point = sk_DIST_POINT_value(distPoints, i);
        if (!point) {
            LogError("Failed to get distribution point.");
            continue;
        }
        if (point->distpoint != NULL &&
            point->distpoint->name.fullname != NULL)
        {
            int countName =
                sk_GENERAL_NAME_num(point->distpoint->name.fullname);
            for (int j = 0; j < countName; ++j) {
                GENERAL_NAME* name = sk_GENERAL_NAME_value(
                        point->distpoint->name.fullname, j);
                if (name != NULL && GEN_URI == name->type) {
                    char *crlUri =
                    reinterpret_cast<char*>(name->d.ia5->data);
                    if (!crlUri) {
                        LogError("Failed to get URI.");
                        continue;
                    }
                    result.push_back(crlUri);
                }
            }
        }
    }
    sk_DIST_POINT_pop_free(distPoints, DIST_POINT_free);
    return result;
}
} //  namespace ValidationCore
