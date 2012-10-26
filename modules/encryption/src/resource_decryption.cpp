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
 * @file    resource_decryption.cpp
 * @author  Soyoung Kim (sy037.kim@samsung.com)
 * @version 1.0
 * @brief   Implementation file for resource decryption
 */
#include <dpl/encryption/resource_decryption.h>

#include <fcntl.h>
#include <string>
#include <dpl/log/log.h>
#include <dpl/exception.h>

namespace {
inline std::string GetDefaultEncryptKeyPath() {
    return "/opt/share/widget/data/";
}
}
namespace WRTDecryptor{
ResourceDecryptor::ResourceDecryptor() :
    m_decKey(NULL)
{
    LogDebug("Started Decryption");
}

ResourceDecryptor::ResourceDecryptor(std::string userKey) :
    m_decKey(NULL)
{
    LogDebug("Finished Decryption");
    SetDecryptionKey(userKey);
}

ResourceDecryptor::~ResourceDecryptor()
{
    delete m_decKey;
}

void ResourceDecryptor::SetDecryptionKey(std::string userKey)
{
    /* TODO : get key from secure storage */
    std::string keyPath = GetDefaultEncryptKeyPath() + userKey + "_dec";
    LogDebug("Description Key path : " << keyPath);

    FILE* fp = fopen(keyPath.c_str(), "rb");
    if (fp == NULL) {
        ThrowMsg(ResourceDecryptor::Exception::GetDecKeyFailed,
                "Failed to get decryption key");
    }

    m_decKey = new AES_KEY;
    size_t resultSize =fread(m_decKey, 1, sizeof(AES_KEY),fp);
    if (resultSize!= sizeof(AES_KEY))
        ThrowMsg(ResourceDecryptor::Exception::GetDecKeyFailed,
                "Failed to get AES key");

    fclose(fp);
}

AES_KEY* ResourceDecryptor::GetDecryptionKey()
{
    return m_decKey;
}

void ResourceDecryptor::GetDecryptedChunk(unsigned char*
        inBuf, unsigned char* decBuf, size_t inBufSize)
{
    Assert(decBuf);
    Assert(m_decKey);
    if (decBuf == NULL || m_decKey == NULL) {
        ThrowMsg(ResourceDecryptor::Exception::EncryptionFailed,
                "Failed to Get Decryption Chunk");
    }
    unsigned char ivec[16] = {0, };

    AES_cbc_encrypt(inBuf, decBuf, inBufSize, m_decKey, ivec, AES_DECRYPT);
    LogDebug("Success decryption");
}

} //namespace WRTDecryptor
