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
/*!
 * @author      Tomasz Morawski(t.morawski@samsung.com)
 * @author      Michal Ciepielski(m.ciepielski@samsung.com)
 * @author      Piotr Marcinkiewicz(p.marcinkiew@samsung.com)
 * @version     0.4
 * @file        OCPS.cpp
 * @brief       Routines for certificate validation over OCSP
 */

#include "OCSP.h"

#include <string.h>
#include <algorithm>

#include <openssl/ssl.h>
#include <openssl/crypto.h>
#include <openssl/err.h>
#include <openssl/x509v3.h>

#include <dpl/log/log.h>
#include <dpl/assert.h>
#include <dpl/foreach.h>
#include <dpl/scoped_array.h>
#include <dpl/scoped_free.h>

#include <libsoup/soup.h>

#include "Certificate.h"
#include "SoupMessageSendSync.h"

extern "C" {
// This function is needed to fix "Invalid conversion from void*
// to unsigned char*" C++ compiler error during calling
// i2d_OCSP_REQUEST_bio macro
    extern bool convertToBuffer(OCSP_REQUEST* req,
                                char** buf,
                                int* size);
}

namespace {
const int ConnectionTimeoutInSeconds = 6;
const int ConnectionRetryCount = 5;

//! Maximum leeway in validity period in seconds: default 1 day
//! (@see checkRevocationStatus function code)

//! Maximum validity time for revocation status (1 day)
const int MaxValidatyPeriodInSeconds = 24 * 60 * 60;

//! Max age (@see checkRevocationStatus function code)
const int MaxAge = -1;
}

namespace ValidationCore {

const char* OCSP::DEFAULT_RESPONDER_URI_ENV = "OCSP_DEFAULT_RESPONDER_URI";

OCSP::DigestAlgorithmMap createDigestAlgMap()
{
    OCSP::DigestAlgorithmMap mDigestAlg = OCSP::DigestAlgorithmMap();

    mDigestAlg.insert(std::make_pair(OCSP::SHA1, EVP_sha1()));
    mDigestAlg.insert(std::make_pair(OCSP::SHA224, EVP_sha224()));
    mDigestAlg.insert(std::make_pair(OCSP::SHA256, EVP_sha256()));
    mDigestAlg.insert(std::make_pair(OCSP::SHA384, EVP_sha384()));
    mDigestAlg.insert(std::make_pair(OCSP::SHA512, EVP_sha512()));

    return mDigestAlg;
}

OCSP::DigestAlgorithmMap OCSP::m_sDigestAlgMap = createDigestAlgMap();

OCSP::OCSP() :
    /* Upgrade of openssl is required to support sha256 */
    //    m_pCertIdDigestAlg(EVP_sha256()),
    //    m_pRequestDigestAlg(EVP_sha256()),
    m_pCertIdDigestAlg(EVP_sha1()),
    m_pRequestDigestAlg(EVP_sha1()),
    m_bUseNonce(false),
    m_bUseDefResponder(false),
    m_bSignRequest(false),
    m_pSignKey(0)
{
}

SoupWrapper::SoupMessageSendBase::RequestStatus OCSP::sendOcspRequest(
        OCSP_REQUEST* argRequest,
        const DPL::OptionalString& argUri)
{
    using namespace SoupWrapper;
    // convert OCSP_REQUEST to memory buffer
    std::string url = DPL::ToUTF8String(*argUri);
    char* requestBuffer;
    int requestSizeInt;
    if (!convertToBuffer(argRequest, &requestBuffer, &requestSizeInt)) {
        ThrowMsg(OCSP::Exception::VerificationError,
                 "OCSP: failed to convert OCSP_REQUEST to mem buffer");
    }

    Assert(requestSizeInt >= 0);

    SoupMessageSendBase::MessageBuffer buffer;
    buffer.resize(requestSizeInt);
    memcpy(&buffer[0], requestBuffer, requestSizeInt);
    free(requestBuffer);

    char *cport = 0,*chost = 0,*cpath = 0;
    int use_ssl = 0;

    if (!OCSP_parse_url(const_cast<char*>(url.c_str()),
                        &chost,
                        &cport,
                        &cpath,
                        &use_ssl))
    {
        LogWarning("Error in OCSP_parse_url");
        return SoupMessageSendBase::REQUEST_STATUS_CONNECTION_ERROR;
    }

    std::string host = chost;

    if (cport) {
        host += ":";
        host += cport;
    }

    free(cport);
    free(chost);
    free(cpath);

    m_soupMessage.setHost(url);
    m_soupMessage.setHeader("Host", host);
    m_soupMessage.setRequest(std::string("application/ocsp-request"),
                             buffer);

    return m_soupMessage.sendSync();
}

ValidationCore::VerificationStatusSet OCSP::validateCertificateList(
        const CertificateList &certs)
{
    VerificationStatusSet statusSet;

    if (certs.size() < 2) {
        // no certificates to verify, just return a error
        LogWarning("No validation will be proceed. OCSP require at"
                   " least 2 certificates in chain. Found only " <<
                   certs.size());
        statusSet.add(VERIFICATION_STATUS_ERROR);
        return statusSet;
    }

    CertificateList::const_iterator iter = certs.begin();
    CertificateList::const_iterator parent = iter;

    time_t minValidity = 0;
    for (++parent; parent != certs.end(); ++iter, ++parent) {
        LogDebug("Certificate validation (CN:" <<
                 (*iter)->getCommonName() << ")");
        statusSet.add(validateCertificate(*iter, *parent));
        if ((0 == minValidity || minValidity > m_responseValidity) &&
                m_responseValidity > 0)
        {
            minValidity = m_responseValidity;
        }
    }
    m_responseValidity = minValidity;

    return statusSet;
}

VerificationStatus OCSP::checkEndEntity(
        const CertificateCollection &chain)
{
    const char *defResponderURI = getenv(OCSP::DEFAULT_RESPONDER_URI_ENV);

    VerificationStatusSet verSet;
    if (defResponderURI) {
        setUseDefaultResponder(true);
        setDefaultResponder(defResponderURI);
    }

    // this is temporary fix. it must be rewriten
    CertificateList clst;
    if (chain.isChain() && chain.size() >= 2) {
        CertificateList::const_iterator icert = chain.begin();
        clst.push_back(*icert);
        ++icert;
        clst.push_back(*icert);
    }
    verSet += validateCertificateList(clst);

    return verSet.convertToStatus();
}

VerificationStatus OCSP::validateCertificate(CertificatePtr argCert,
                                             CertificatePtr argIssuer)
{
    using namespace SoupWrapper;

    Assert(!!argCert);
    Assert(!!argIssuer);

    Try {
        DPL::OptionalString uri;

        if (!m_bUseDefResponder) {
            uri = argCert->getOCSPURL();
            if (!uri) {
                return VERIFICATION_STATUS_NOT_SUPPORT;
            }
        } else {
            if (m_strResponderURI.empty()) {
                ThrowMsg(Exception::VerificationError,
                         "Default responder is not set");
            }
            LogWarning("Default responder will be used");

            uri = m_strResponderURI;
        }

        // creates a request
        CreateRequestResult newRequest = createRequest(argCert, argIssuer);
        if (!newRequest.success) {
            ThrowMsg(Exception::VerificationError, "Request creation failed");
        }

        // SSLSmartContainer <OCSP_CERTID> certIdCont(certId);
        // this smart ptr is commented out in purpose. request
        // manages certIdmemory (which was done in createRequest above)
        SSLSmartContainer <OCSP_REQUEST> requestCont(newRequest.ocspRequest);

        SoupMessageSendBase::RequestStatus requestStatus;
        requestStatus = sendOcspRequest(requestCont, uri);

        if (requestStatus != SoupMessageSendBase::REQUEST_STATUS_OK) {
            return VERIFICATION_STATUS_CONNECTION_FAILED;
        }

        // Response is m_soupMessage, convert it to OCSP_RESPONSE
        OcspResponse response  = convertToResponse();

        if (!response.first) {
            ThrowMsg(OCSP::Exception::VerificationError,
                     "OCSP: failed to convert mem buffer to OCSP_RESPONSE");
        }

        SSLSmartContainer <OCSP_RESPONSE> responseCont(response.second);
        // verify response eg. check response status,
        // validate responder certificate
        validateResponse(requestCont,
                         responseCont,
                         newRequest.ocspCertId);
    } Catch(Exception::ConnectionError) {
        LogWarning("OCSP: ConnectionError");
        return VERIFICATION_STATUS_CONNECTION_FAILED;
    } Catch(Exception::CertificateRevoked) {
        LogWarning("OCSP: Revoked");
        return VERIFICATION_STATUS_REVOKED;
    } Catch(Exception::CertificateUnknown) {
        LogWarning("OCSP: Unknown");
        return VERIFICATION_STATUS_UNKNOWN;
    } Catch(Exception::VerificationError) {
        LogWarning("OCSP: Verification error");
        return VERIFICATION_STATUS_VERIFICATION_ERROR;
    } Catch(Exception::Base) {
        LogWarning("OCSP: Error");
        return VERIFICATION_STATUS_ERROR;
    }
    LogWarning("OCSP: Good");
    return VERIFICATION_STATUS_GOOD;
}

OCSP::CreateRequestResult OCSP::createRequest(CertificatePtr argCert,
                                              CertificatePtr argIssuer)
{
    OCSP_REQUEST* newRequest = OCSP_REQUEST_new();

    if (!newRequest) {
        LogWarning("OCSP: Failed to create a request");
        return CreateRequestResult();
    }

    SSLSmartContainer <OCSP_REQUEST> requestCont(newRequest);

    OCSP_CERTID* certId = addSerial(argCert, argIssuer);

    if (!certId) {
        LogWarning("OCSP: Unable to create a serial id");
        return CreateRequestResult();
    }
    SSLSmartContainer <OCSP_CERTID> certIdCont(certId);

    // Inserting certificate ID to request
    if (!OCSP_request_add0_id(requestCont, certIdCont)) {
        LogWarning("OCSP: Unable to create a certificate id");
        return CreateRequestResult();
    }

    if (m_bUseNonce) {
        OCSP_request_add1_nonce(requestCont, 0, -1);
    }

    if (m_bSignRequest) {
        if (!m_pSignCert || !m_pSignKey) {
            LogWarning("OCSP: Unable to sign request if "
                       "SignCert or SignKey was not set");
            return CreateRequestResult();
        }

        if (!OCSP_request_sign(requestCont,
                               m_pSignCert->getX509(),
                               m_pSignKey,
                               m_pRequestDigestAlg,
                               0,
                               0))
        {
            LogWarning("OCSP: Unable to sign request");
            return CreateRequestResult();
        }
    }
    return CreateRequestResult(true,
                               requestCont.DetachPtr(),
                               certIdCont.DetachPtr());
}

OCSP_CERTID* OCSP::addSerial(CertificatePtr argCert,
                             CertificatePtr argIssuer)
{
    X509_NAME* iname = X509_get_subject_name(argIssuer->getX509());
    ASN1_BIT_STRING* ikey = X509_get0_pubkey_bitstr(argIssuer->getX509());
    ASN1_INTEGER* serial = X509_get_serialNumber(argCert->getX509());

    return OCSP_cert_id_new(m_pCertIdDigestAlg, iname, ikey, serial);
}

void OCSP::setDigestAlgorithmForCertId(DigestAlgorithm alg)
{
    DigestAlgorithmMap::const_iterator cit = m_sDigestAlgMap.find(alg);

    if (cit != m_sDigestAlgMap.end()) {
        m_pCertIdDigestAlg = cit->second;
    } else {
        LogDebug("Request for unsupported CertId digest algorithm"
                 "ignored!");
    }
}

void OCSP::setDigestAlgorithmForRequest(DigestAlgorithm alg)
{
    DigestAlgorithmMap::const_iterator cit = m_sDigestAlgMap.find(alg);

    if (cit != m_sDigestAlgMap.end()) {
        m_pRequestDigestAlg = cit->second;
    } else {
        LogDebug("Request for unsupported OCSP request digest algorithm"
                 "ignored!");
    }
}

void OCSP::setTrustedStore(const CertificateList& certs)
{
    X509_STORE *store = X509_STORE_new();
    m_pTrustedStore = store;
    // create a trusted store basing on certificate chain from a signature
    FOREACH(iter, certs) {
        X509_STORE_add_cert(store, (*iter)->getX509());
    }
}

void OCSP::validateResponse(OCSP_REQUEST* argRequest,
                            OCSP_RESPONSE* argResponse,
                            OCSP_CERTID* argCertId)
{
    int result = OCSP_response_status(argResponse);

    if (result != OCSP_RESPONSE_STATUS_SUCCESSFUL) {
        handleInvalidResponse(result);
        ThrowMsg(Exception::VerificationError, "OCSP_response_status failed");
    }

    // get response object
    OCSP_BASICRESP* basic = OCSP_response_get1_basic(argResponse);
    if (!basic) {
        ThrowMsg(Exception::VerificationError,
                 "OCSP: Unable to get a BASICRESP object.");
    }

    SSLSmartContainer <OCSP_BASICRESP> basicRespCont(basic);
    if (m_bUseNonce && OCSP_check_nonce(argRequest, basicRespCont) <= 0) {
        ThrowMsg(Exception::VerificationError, "OCSP: Invalid nonce");
    }

    if (!verifyResponse(basic)) {
        ThrowMsg(Exception::VerificationError,
                 "Unable to verify the OCSP responder's certificate");
    }

    checkRevocationStatus(basicRespCont, argCertId);
}

bool OCSP::verifyResponse(OCSP_BASICRESP* basic)
{
    Assert(m_pTrustedStore);
    // verify ocsp response
    int response = OCSP_basic_verify(basic, NULL, m_pTrustedStore, 0);
    if (response <= 0) {
        LogWarning("OCSP verification failed");
    }

    return response > 0;
}

void OCSP::checkRevocationStatus(OCSP_BASICRESP* basic,
                                 OCSP_CERTID* id)
{
    ASN1_GENERALIZEDTIME* producedAt;
    ASN1_GENERALIZEDTIME* thisUpdate;
    ASN1_GENERALIZEDTIME* nextUpdate;
    int reason;
    int status;

    m_responseValidity = 0;

    if (!OCSP_resp_find_status(basic,
                               id,
                               &status,
                               &reason,
                               &producedAt,
                               &thisUpdate,
                               &nextUpdate))
    {
        ThrowMsg(Exception::VerificationError,
                 "OCSP: Failed to find certificate status.");
    }

    if (!OCSP_check_validity(thisUpdate,
                             nextUpdate,
                             MaxValidatyPeriodInSeconds,
                             MaxAge))
    {
        ThrowMsg(Exception::VerificationError,
                 "OCSP: Failed to check certificate validate.");
    }

    if (nextUpdate) {
        asn1GeneralizedTimeToTimeT(nextUpdate,&m_responseValidity);
        time_t now;
        time(&now);
        LogDebug("Time of next OCSP update got from server: " <<
                 m_responseValidity);
        LogDebug("Expires in: " << (m_responseValidity - now));
        LogDebug("Original: " << nextUpdate->data);
    }

    switch (status) {
    case V_OCSP_CERTSTATUS_GOOD:
        return;
    case V_OCSP_CERTSTATUS_REVOKED:
        ThrowMsg(Exception::CertificateRevoked, "Certificate is Revoked");
    case V_OCSP_CERTSTATUS_UNKNOWN:
        ThrowMsg(Exception::CertificateUnknown, "Certificate is Unknown");
    default:
        Assert(false && "Invalid status");
    }
}

OCSP::OcspResponse OCSP::convertToResponse()
{
    using namespace SoupWrapper;

    // convert memory buffer to ocsp response object
    BUF_MEM res_bmem;
    OCSP_RESPONSE* response;

    SoupMessageSendBase::MessageBuffer buffer = m_soupMessage.getResponse();

    res_bmem.length = buffer.size();
    res_bmem.data = &buffer[0];
    res_bmem.max = buffer.size();

    BIO* res_mem_bio = BIO_new(BIO_s_mem());
    BIO_set_mem_buf(res_mem_bio, &res_bmem, BIO_NOCLOSE);

    response = d2i_OCSP_RESPONSE_bio(res_mem_bio, NULL);
    BIO_free_all(res_mem_bio);

    if (!response) {
        LogWarning("OCSP: Failed to convert OCSP Response to DER format");
        return std::make_pair(false, static_cast<OCSP_RESPONSE*>(NULL));
    }

    return std::make_pair(true, response);
}

void OCSP::handleInvalidResponse(int result)
{
    switch (result) {
    case OCSP_RESPONSE_STATUS_MALFORMEDREQUEST:
        LogWarning("OCSP: Server returns "
                   "OCSP_RESPONSE_STATUS_MALFORMEDREQUEST status");
        break;
    case OCSP_RESPONSE_STATUS_INTERNALERROR:
        LogWarning("OCSP: Server returns "
                   "OCSP_RESPONSE_STATUS_INTERNALERROR status");
        break;
    case OCSP_RESPONSE_STATUS_TRYLATER:
        LogWarning("OCSP: Server returns "
                   "OCSP_RESPONSE_STATUS_TRYLATER status");
        break;
    case OCSP_RESPONSE_STATUS_SIGREQUIRED:
        LogWarning("OCSP: Server returns "
                   "OCSP_RESPONSE_STATUS_SIGREQUIRED status");
        break;
    case OCSP_RESPONSE_STATUS_UNAUTHORIZED:
        LogWarning("OCSP: Server returns "
                   "OCSP_RESPONSE_STATUS_UNAUTHORIZED status");
        break;
    default:
        Assert(false && "Invalid result value");
    }
}
} // namespace ValidationCore
