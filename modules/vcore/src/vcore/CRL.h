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
 * @author      Piotr Marcinkiewicz(p.marcinkiew@samsung.com)
 * @version     0.4
 * @file        CRL.h
 * @brief       Routines for certificate validation over CRL
 */

#ifndef WRT_ENGINE_SRC_VALIDATION_CORE_ENGINE_CRL_H_
#define WRT_ENGINE_SRC_VALIDATION_CORE_ENGINE_CRL_H_

#include <dpl/exception.h>
#include <dpl/shared_ptr.h>
#include <dpl/noncopyable.h>
#include <dpl/log/log.h>

#include "Certificate.h"
#include "CertificateCollection.h"
#include "SoupMessageSendBase.h"
#include "VerificationStatus.h"

namespace ValidationCore {
namespace CRLException {
DECLARE_EXCEPTION_TYPE(DPL::Exception, CRLException)
DECLARE_EXCEPTION_TYPE(CRLException, StorageError)
DECLARE_EXCEPTION_TYPE(CRLException, DownloadFailed)
DECLARE_EXCEPTION_TYPE(CRLException, InternalError)
DECLARE_EXCEPTION_TYPE(CRLException, InvalidParameter)
}

class CachedCRL;  // forward declaration

class CRL
{
  protected:
    X509_STORE *m_store;
    X509_LOOKUP *m_lookup;

    class CRLData : DPL::Noncopyable
    {
      public:
        //TODO: change to SharedArray when available
        char *buffer;
        size_t length;
        std::string uri;

        CRLData(char* _buffer,
                size_t _length,
                const std::string &_uri) :
            buffer(_buffer),
            length(_length),
            uri(_uri)
        {
        }

        CRLData(const SoupWrapper::SoupMessageSendBase::MessageBuffer &mBuff,
                const std::string &mUri)
        : uri(mUri)
        {
            buffer = new char[mBuff.size()];
            length = mBuff.size();
            memcpy(buffer, &mBuff[0], mBuff.size());
        }

        ~CRLData()
        {
            LogInfo("Delete buffer");
            delete[] buffer;
        }
    };
    typedef DPL::SharedPtr<CRLData> CRLDataPtr;
    typedef std::list<std::string> StringList;

    CRLDataPtr getCRL(const std::string &uri) const;
    CRLDataPtr downloadCRL(const std::string &uri);
    X509_STORE_CTX *createContext(const CertificatePtr &argCert);
    void updateCRL(const CRLDataPtr &crl);
    X509_CRL *convertToInternal(const CRLDataPtr &crl) const;
    StringList getCrlUris(const CertificatePtr &argCert);
    bool isPEMFormat(const CRLDataPtr &crl) const;
    bool verifyCRL(X509_CRL *crl,
                   const CertificatePtr &cert);
    void addToStorage(const CertificatePtr &argCert);
    void cleanup();
    bool isOutOfDate(const CRLDataPtr &crl) const;

    friend class CachedCRL;
  public:
    enum UpdatePolicy
    {
        UPDATE_ON_EXPIRED,  /**< Download and update CRL only when next update
                                date has expired */
        UPDATE_ON_DEMAND    /**< Download and update CRL regardless next update
                                date */
    };

    struct RevocationStatus
    {
        bool isCRLValid;    /**< True when CRL was valid during
                                certificate validation */
        bool isRevoked;     /**< True when certificate is revoked */
    };

    CRL();
    ~CRL();

    /**
     * @brief Checks if given certificate is revoked.
     *
     * @details This function doesn't update CRL list. If related CRL
     * is out of date the #isCRLValid return parameter is set to false.
     *
     * @param[in] argCert The certificate to check against revocation.
     * @return RevocationStatus.isRevoked True when certificate is revoked,
     *          false otherwise.
     *         RevocationStatus.isCRLValid True if related CRL has not expired,
     *          false otherwise.
     */
    RevocationStatus checkCertificate(const CertificatePtr &argCert);

    /**
     * @brief Checks if any certificate from certificate chain is revoked.
     *
     * @details This function doesn't update CRL lists. If any of related
     * CRL is out of date the #isCRLValid parameter is set to true.
     * This function adds valid certificates from the chain to internal storage
     * map so they'll be available in further check operations for current
     * CRL object.
     *
     * @param[in] argCert The certificate chain to check against revocation.
     * @return RevocationStatus.isRevoked True when any from certificate chain
     *          is revoked, false otherwise.
     *         RevocationStatus.isCRLValid True if all of related CRLs has
     *          not expired, false otherwise.
     */
    RevocationStatus checkCertificateChain(CertificateCollection certChain);

    VerificationStatus checkEndEntity(CertificateCollection &chain);

    /**
     * @brief Updates CRL related with given certificate.
     *
     * @details This function updates CRL list related with given certificate.
     * If CRL related with given certificate is not stored in database
     * then this function will download CRL and store it in database.
     *
     * @param[in] argCert The certificate for which the CRL will be updated
     * @param[in] updatePolicy Determine when CRL will be downloaded and updated
     * @return True when CRL for given certificate was updated successfully,
     *          false otherwise.
     */
    bool updateList(const CertificatePtr &argCert,
                    const UpdatePolicy updatePolicy);

    /**
     * @brief Updates CRL related with given certificates.
     *
     * @details This function updates CRL lists related with given certificates.
     * If CRL related with given certificate is not stored in database
     * then this function will download CRL and store it in database.
     *
     * @param[in] collection The certificate collection for which the CRL will
     *            be updated
     * @param[in] updatePolicy Determine when CRL will be downloaded and updated
     * @return True when CRL for given certificate was updated successfully,
     *          false otherwise.
     */
    bool updateList(const CertificateCollection &collection,
                    const UpdatePolicy updatePolisy);

    /**
     * @brief Add certificate to known certificates store.
     *
     * @param[in] collection The certificate collection which will be
     *            added to known certificate store.
     */
    void addToStore(const CertificateCollection &collection);
};
} // ValidationCore

#endif //ifndef WRT_ENGINE_SRC_VALIDATION_CORE_ENGINE_CRL_H_
