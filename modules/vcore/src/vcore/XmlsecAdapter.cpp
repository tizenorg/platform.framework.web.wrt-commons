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
 * @file        XmlsecAdapter.cpp
 * @author      Bartlomiej Grzelewski (b.grzelewski@samsung.com)
 * @version     1.0
 * @brief
 */

/* 
 * Copyright (C) 2002-2003 Aleksey Sanin.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */


#include <cstdlib>
#include <cstring>

#include <libxml/tree.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

#ifndef XMLSEC_NO_XSLT
#include <libxslt/xslt.h>
#endif /*   XMLSEC_NO_XSLT */

#include <xmlsec/xmlsec.h>
#include <xmlsec/xmltree.h>
#include <xmlsec/xmldsig.h>
#include <xmlsec/crypto.h>
#include <xmlsec/io.h>
#include <xmlsec/keyinfo.h>

#include <dpl/assert.h>
#include <dpl/log/log.h>

#include "XmlsecAdapter.h"
#include <dpl/singleton_impl.h>
IMPLEMENT_SINGLETON(ValidationCore::XmlSec)

namespace ValidationCore {
VC_DECLARE_DELETER(xmlSecKeysMngr, xmlSecKeysMngrDestroy)

static const char* DIGEST_MD5 = "md5";

std::string XmlSec::s_prefixPath;

int XmlSec::fileMatchCallback(const char *filename)
{
    std::string path = s_prefixPath + filename;
    return xmlFileMatch(path.c_str());
}

void* XmlSec::fileOpenCallback(const char *filename)
{
    std::string path = s_prefixPath + filename;
    LogDebug("Xmlsec opening: " << path);
    return xmlFileOpen(path.c_str());
}

int XmlSec::fileReadCallback(void *context,
        char *buffer,
        int len)
{
    return xmlFileRead(context, buffer, len);
}

int XmlSec::fileCloseCallback(void *context)
{
    return xmlFileClose(context);
}

void XmlSec::fileExtractPrefix(XmlSecContext *context)
{
    if (!(context->workingDirectory.empty())) {
        s_prefixPath = context->workingDirectory;
        return;
    }

    s_prefixPath = context->signatureFile;
    size_t pos = s_prefixPath.rfind('/');
    if (pos == std::string::npos) {
        s_prefixPath.clear();
    } else {
        s_prefixPath.erase(pos + 1, std::string::npos);
    }
}

XmlSec::XmlSec() :
    m_initialized(false)
{
    LIBXML_TEST_VERSION
        xmlLoadExtDtdDefaultValue = XML_DETECT_IDS | XML_COMPLETE_ATTRS;
    xmlSubstituteEntitiesDefault(1);
#ifndef XMLSEC_NO_XSLT
    xmlIndentTreeOutput = 1;
#endif

    if (xmlSecInit() < 0) {
        LogError("Xmlsec initialization failed.");
        ThrowMsg(Exception::InternalError, "Xmlsec initialization failed.");
    }

    if (xmlSecCheckVersion() != 1) {
        xmlSecShutdown();
        LogError("Loaded xmlsec library version is not compatible.");
        ThrowMsg(Exception::InternalError,
                 "Loaded xmlsec library version is not compatible.");
    }

#ifdef XMLSEC_CRYPTO_DYNAMIC_LOADING
    if (xmlSecCryptoDLLoadLibrary(BAD_CAST XMLSEC_CRYPTO) < 0) {
        xmlSecShutdown();
        LogError(
            "Error: unable to load default xmlsec-crypto library. Make sure "
            "that you have it installed and check shared libraries path "
            "(LD_LIBRARY_PATH) envornment variable.");
        ThrowMsg(Exception::InternalError,
                 "Unable to load default xmlsec-crypto library.");
    }
#endif

    if (xmlSecCryptoAppInit(NULL) < 0) {
        xmlSecShutdown();
        LogError("Crypto initialization failed.");
        ThrowMsg(Exception::InternalError, "Crypto initialization failed.");
    }

    if (xmlSecCryptoInit() < 0) {
        xmlSecCryptoAppShutdown();
        xmlSecShutdown();
        LogError("Xmlsec-crypto initialization failed.");
        ThrowMsg(Exception::InternalError,
                 "Xmlsec-crypto initialization failed.");
    }

    m_initialized = true;
}

void XmlSec::deinitialize(void)
{
    Assert(m_initialized);

    /*   Shutdown xmlsec-crypto library */
    xmlSecCryptoShutdown();

    /*   Shutdown crypto library */
    xmlSecCryptoAppShutdown();

    /*   Shutdown xmlsec library */
    xmlSecShutdown();

    /*   Shutdown libxslt/libxml */
#ifndef XMLSEC_NO_XSLT
    xsltCleanupGlobals();
#endif /*   XMLSEC_NO_XSLT */

    s_prefixPath.clear();
    m_initialized = false;
}

XmlSec::~XmlSec()
{
    if (m_initialized) {
        deinitialize();
    }
}

XmlSec::Result XmlSec::validateFile(XmlSecContext *context,
        xmlSecKeysMngrPtr mngr)
{
    xmlDocPtr doc = NULL;
    xmlNodePtr node = NULL;
    xmlSecDSigCtxPtr dsigCtx = NULL;
    int size, res = -1;

    fileExtractPrefix(context);
    LogDebug("Prefix path: " << s_prefixPath);

    xmlSecIOCleanupCallbacks();

    xmlSecIORegisterCallbacks(
        fileMatchCallback,
        fileOpenCallback,
        fileReadCallback,
        fileCloseCallback);

    /*   load file */
    doc = xmlParseFile(context->signatureFile.c_str());
    if ((doc == NULL) || (xmlDocGetRootElement(doc) == NULL)) {
        LogWarning("Unable to parse file " << context->signatureFile);
        goto done;
    }

    /*   find start node */
    node = xmlSecFindNode(xmlDocGetRootElement(
                              doc), xmlSecNodeSignature, xmlSecDSigNs);
    if (node == NULL) {
        LogWarning("Start node not found in " << context->signatureFile);
        goto done;
    }

    /*   create signature context */
    dsigCtx = xmlSecDSigCtxCreate(mngr);
    if (dsigCtx == NULL) {
        LogError("Failed to create signature context.");
        goto done;
    }

    if (context->allowBrokenChain) {
        dsigCtx->keyInfoReadCtx.flags |=
            XMLSEC_KEYINFO_FLAGS_ALLOW_BROKEN_CHAIN;
    }

    if (context->validationTime) {
        LogDebug("Setting validation time.");
        dsigCtx->keyInfoReadCtx.certsVerificationTime = context->validationTime;
    }

    /*   Verify signature */
    if (xmlSecDSigCtxVerify(dsigCtx, node) < 0) {
        LogWarning("Signature verify error.");
        goto done;
    }

    if (dsigCtx->keyInfoReadCtx.flags2 &
        XMLSEC_KEYINFO_ERROR_FLAGS_BROKEN_CHAIN) {
        LogWarning("XMLSEC_KEYINFO_FLAGS_ALLOW_BROKEN_CHAIN was set to true!");
        LogWarning("Signature contains broken chain!");
        context->errorBrokenChain = true;
    }

    /*   print verification result to stdout */
    if (dsigCtx->status == xmlSecDSigStatusSucceeded) {
        LogDebug("Signature is OK");
        res = 0;
    } else {
        LogDebug("Signature is INVALID");
        goto done;
    }

    if (dsigCtx->c14nMethod && dsigCtx->c14nMethod->id &&
        dsigCtx->c14nMethod->id->name) {
        LogInfo("Canonicalization method: " <<
                reinterpret_cast<const char *>(dsigCtx->c14nMethod->id->name));
    }

    size = xmlSecPtrListGetSize(&(dsigCtx->signedInfoReferences));
    for (int i = 0; i < size; ++i) {
        xmlSecDSigReferenceCtxPtr dsigRefCtx =
            (xmlSecDSigReferenceCtxPtr)xmlSecPtrListGetItem(&(dsigCtx->
                                                                  signedInfoReferences),
                                                            i);
        if (dsigRefCtx && dsigRefCtx->uri) {
            if (dsigRefCtx->digestMethod && dsigRefCtx->digestMethod->id &&
                dsigRefCtx->digestMethod->id->name) {
                const char* pDigest =
                    reinterpret_cast<const char *>(dsigRefCtx->digestMethod->id
                                                       ->name);
                std::string strDigest(pDigest);
                LogInfo("reference digest method: " <<
                        reinterpret_cast<const char *>(dsigRefCtx->digestMethod
                                                           ->id
                                                           ->name));
                if (strDigest == DIGEST_MD5) {
                    LogWarning("MD5 digest method used! Please use sha");
                    res = -1;
                    break;
                }
            }
            context->referenceSet.insert(std::string(reinterpret_cast<char *>(
                                                         dsigRefCtx->uri)));
        }
    }

done:
    /*   cleanup */
    if (dsigCtx != NULL) {
        xmlSecDSigCtxDestroy(dsigCtx);
    }

    if (doc != NULL) {
        xmlFreeDoc(doc);
    }

    if (res) {
        return ERROR_INVALID_SIGNATURE;
    }
    return NO_ERROR;
}

void XmlSec::loadDERCertificateMemory(XmlSecContext *context,
        xmlSecKeysMngrPtr mngr)
{
    unsigned char *derCertificate = NULL;
    int size = i2d_X509(context->certificatePtr->getX509(), &derCertificate);

    if (!derCertificate) {
        LogError("Failed during x509 conversion to der format.");
        ThrowMsg(Exception::InternalError,
                 "Failed during x509 conversion to der format.");
    }

    if (xmlSecCryptoAppKeysMngrCertLoadMemory(mngr,
                                              derCertificate,
                                              size,
                                              xmlSecKeyDataFormatDer,
                                              xmlSecKeyDataTypeTrusted) < 0) {
        OPENSSL_free(derCertificate);
        LogError("Failed to load der certificate from memory.");
        ThrowMsg(Exception::InternalError,
                 "Failed to load der certificate from memory.");
    }

    OPENSSL_free(derCertificate);
}

void XmlSec::loadPEMCertificateFile(XmlSecContext *context,
        xmlSecKeysMngrPtr mngr)
{
    if (xmlSecCryptoAppKeysMngrCertLoad(mngr,
                                        context->certificatePath.c_str(),
                                        xmlSecKeyDataFormatPem,
                                        xmlSecKeyDataTypeTrusted) < 0) {
        LogError("Failed to load PEM certificate from file.");
        ThrowMsg(Exception::InternalError,
                 "Failed to load PEM certificate from file.");
    }
}

XmlSec::Result XmlSec::validate(XmlSecContext *context)
{
    Assert(context);
    Assert(!(context->signatureFile.empty()));
    Assert(context->certificatePtr.Get() || !(context->certificatePath.empty()));

    if (!m_initialized) {
        LogError("XmlSec is not initialized.");
        ThrowMsg(Exception::InternalError, "XmlSec is not initialized");
    }

    AutoPtr<xmlSecKeysMngr> mngr(xmlSecKeysMngrCreate());

    if (!mngr.get()) {
        LogError("Failed to create keys manager.");
        ThrowMsg(Exception::InternalError, "Failed to create keys manager.");
    }

    if (xmlSecCryptoAppDefaultKeysMngrInit(mngr.get()) < 0) {
        LogError("Failed to initialize keys manager.");
        ThrowMsg(Exception::InternalError, "Failed to initialize keys manager.");
    }
    context->referenceSet.clear();

    if (context->certificatePtr.Get()) {
        loadDERCertificateMemory(context, mngr.get());
    }

    if (!context->certificatePath.empty()) {
        loadPEMCertificateFile(context, mngr.get());
    }

    return validateFile(context, mngr.get());
}
} // namespace ValidationCore
