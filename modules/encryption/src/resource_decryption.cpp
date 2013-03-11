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
#include <stddef.h>
#include <dpl/encryption/resource_decryption.h>

#include <fcntl.h>
#include <string>
#include <dpl/log/log.h>
#include <dpl/exception.h>
#include <dukgen.h>

namespace {
#define BITS_SIZE 128
#define KEY_SIZE 16
}
namespace WRTDecryptor {
ResourceDecryptor::ResourceDecryptor()
{
    LogDebug("Started Decryption");
}

ResourceDecryptor::ResourceDecryptor(std::string userKey)
{
    LogDebug("Finished Decryption");
    SetDecryptionKey(userKey);
}

ResourceDecryptor::~ResourceDecryptor()
{}

void ResourceDecryptor::SetDecryptionKey(std::string userKey)
{
    if (userKey.empty()) {
        return;
    }

    char* pKey = GetDeviceUniqueKey(const_cast<char*>(userKey.c_str()),
            userKey.size(), KEY_SIZE);

    unsigned char *key = reinterpret_cast<unsigned char*>(pKey);

    if (0 > AES_set_decrypt_key(key, BITS_SIZE, &m_decKey)) {
        ThrowMsg(ResourceDecryptor::Exception::GetDecKeyFailed,
                 "Failed to create decryption key");
    }
}

AES_KEY* ResourceDecryptor::GetDecryptionKey()
{
    return &m_decKey;
}

void ResourceDecryptor::GetDecryptedChunk(unsigned char*
                                          inBuf,
                                          unsigned char* decBuf,
                                          size_t inBufSize)
{
    Assert(decBuf);
    if (decBuf == NULL) {
        ThrowMsg(ResourceDecryptor::Exception::EncryptionFailed,
                 "Failed to Get Decryption Chunk");
    }
    unsigned char ivec[16] = { 0, };

    AES_cbc_encrypt(inBuf, decBuf, inBufSize, &m_decKey, ivec, AES_DECRYPT);
    LogDebug("Success decryption");
}
} //namespace WRTDecryptor
