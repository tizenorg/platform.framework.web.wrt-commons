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
#include <vcore/CertificateCollection.h>

#include <algorithm>

#include <dpl/binary_queue.h>
#include <dpl/foreach.h>
#include <dpl/log/log.h>

#include <vcore/Base64.h>

namespace {
using namespace ValidationCore;

inline std::string toBinaryString(int data)
{
    char buffer[sizeof(int)];
    memcpy(buffer, &data, sizeof(int));
    return std::string(buffer, buffer + sizeof(int));
}

// helper functor for finding certificate basing on subject field
class SubjectElementFinder
{
  public:
    SubjectElementFinder(CertificatePtr &cert) : m_searchFor(cert)
    {
    }
    bool operator()(const CertificatePtr &cert) const
    {
        return cert->isSignedBy(m_searchFor);
    }
    void setCert(CertificatePtr &cert)
    {
        m_searchFor = cert;
    }

  private:
    CertificatePtr m_searchFor;
};

// helper functor for finding certificate basing on issuer field
class IssuerElementFinder
{
  public:
    IssuerElementFinder(CertificatePtr &cert) : m_searchFor(cert)
    {
    }
    bool operator()(const CertificatePtr &cert) const
    {
        return m_searchFor->isSignedBy(cert);
    }
    void setCert(CertificatePtr &cert)
    {
        m_searchFor = cert;
    }

  private:
    CertificatePtr m_searchFor;
};

//! Helper function
// pops an element from list, finder is responsible for match certificates
template <typename T>
static CertificatePtr popAnElementFromList(CertificateList& lCertificates,
        const T& finder)
{
    CertificatePtr pOutCertificate;
    CertificateList::iterator it =
        std::find_if(lCertificates.begin(), lCertificates.end(), finder);

    if (it != lCertificates.end()) {
        pOutCertificate = *it;
        lCertificates.erase(it);
    }

    return pOutCertificate;
}
} // namespace

namespace ValidationCore {
CertificateCollection::CertificateCollection() :
    m_collectionStatus(COLLECTION_UNSORTED)
{
}

void CertificateCollection::clear(void)
{
    m_collectionStatus = COLLECTION_UNSORTED;
    m_certList.clear();
}

void CertificateCollection::load(const CertificateList &certList)
{
    m_collectionStatus = COLLECTION_UNSORTED;
    std::copy(certList.begin(),
              certList.end(),
              std::back_inserter(m_certList));
}

bool CertificateCollection::load(const std::string &buffer)
{
    Base64Decoder base64;
    base64.reset();
    base64.append(buffer);
    if (!base64.finalize()) {
        LogWarning("Error during chain decoding");
        return false;
    }
    std::string binaryData = base64.get();

    DPL::BinaryQueue queue;
    queue.AppendCopy(binaryData.c_str(), binaryData.size());

    int certNum;
    queue.FlattenConsume(&certNum, sizeof(int));

    CertificateList list;

    for (int i = 0; i < certNum; ++i) {
        int certSize;
        queue.FlattenConsume(&certSize, sizeof(int));
        std::vector<char> rawDERCert;
        rawDERCert.resize(certSize);
        queue.FlattenConsume(&rawDERCert[0], certSize);
        Try {
            list.push_back(CertificatePtr(
                               new Certificate(std::string(rawDERCert.begin(),
                                                           rawDERCert.end()))));
        } Catch(Certificate::Exception::Base) {
            LogWarning("Error during certificate creation.");
            return false;
        }
        LogDebug("Read certificate from database. Certificate common name: " <<
                 list.back()->getCommonName());
    }
    load(list);
    return true;
}

std::string CertificateCollection::toBase64String() const
{
    std::ostringstream output;
    int certNum = m_certList.size();
    output << toBinaryString(certNum);
    FOREACH(i, m_certList){
        std::string derCert = (*i)->getDER();
        output << toBinaryString(derCert.size());
        output << derCert;
    }
    Base64Encoder base64;
    base64.reset();
    base64.append(output.str());
    base64.finalize();
    return base64.get();
}

CertificateList CertificateCollection::getCertificateList() const
{
    return m_certList;
}

bool CertificateCollection::isChain() const
{
    if (COLLECTION_SORTED != m_collectionStatus) {
        LogError("You must sort certificates first");
        ThrowMsg(Exception::WrongUsage,
                 "You must sort certificates first");
    }
    return (COLLECTION_SORTED == m_collectionStatus) ? true : false;
}

bool CertificateCollection::sort()
{
    if (COLLECTION_UNSORTED == m_collectionStatus) {
        sortCollection();
    }
    return (COLLECTION_SORTED == m_collectionStatus) ? true : false;
}

CertificateList CertificateCollection::getChain() const
{
    if (COLLECTION_SORTED != m_collectionStatus) {
        LogError("You must sort certificates first");
        ThrowMsg(Exception::WrongUsage,
                 "You must sort certificates first");
    }
    return m_certList;
}

void CertificateCollection::sortCollection()
{
    CertificateList lCertificates = m_certList;
    // sorting is not necessary
    if (lCertificates.empty()) {
        m_collectionStatus = COLLECTION_SORTED;
        return;
    }

    // backup input list
    CertificateList lBackup = lCertificates;

    // create a new output list
    CertificateList lSortedCertificates;

    CertificatePtr pCertificate = lCertificates.front();

    lCertificates.pop_front();
    lSortedCertificates.push_back(pCertificate);

    // indicates whether any matching certificate was found
    bool bFound = false;

    CertificatePtr pSubjectCert = pCertificate;

    // lCertificates is very short (3 elements) so
    // O(n^2) algorithm is good enough.
    if (!lCertificates.empty()) {
        // First, find all predecessors for, next all successors
        // for pCertificate
        SubjectElementFinder subjectFinder(pCertificate);
        while ((pSubjectCert = popAnElementFromList(lCertificates,
                                                    subjectFinder)) != NULL) {
            lSortedCertificates.push_front(pSubjectCert);
            subjectFinder.setCert(pSubjectCert);
            bFound = true;
        }

        CertificatePtr pIssuerCert = pCertificate;

        IssuerElementFinder issuerFinder(pCertificate);
        while ((pIssuerCert = popAnElementFromList(lCertificates,
                                                   issuerFinder)) != NULL) {
            lSortedCertificates.push_back(pIssuerCert);
            issuerFinder.setCert(pIssuerCert);
            bFound = true;
        }

        if (!bFound || !lCertificates.empty()) {
            // well, those certificates don't form a valid chain
            LogWarning("Certificates don't form a valid chain");
            lCertificates = lBackup;
            m_collectionStatus = COLLECTION_CHAIN_BROKEN;
            return;
        }
    }

    m_certList = lSortedCertificates;
    m_collectionStatus = COLLECTION_SORTED;
}
} // namespace ValidationCore

