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
 * @file    resource_encryption.cpp
 * @author  Soyoung Kim (sy037.kim@samsung.com)
 * @version 1.0
 * @brief   Implementation file for resource encryption
 */
#include <dpl/encryption/resource_encryption.h>

#include <fcntl.h>
#include <dpl/log/log.h>

namespace {
#define BITS_SIZE 128
const char* ENCRYPTION_FILE = "_enc";
const char* DECRYPTION_FILE = "_dec";

inline std::string GetDefaultEncryptKeyPath() {
    return "/opt/share/widget/data";
}
}
namespace WRTEncryptor{
ResourceEncryptor::ResourceEncryptor()
{
    LogDebug("Started Encrytion");
}

ResourceEncryptor::~ResourceEncryptor()
{
    LogDebug("Finished Encrytion");
}

int ResourceEncryptor::GetBlockSize(int inSize)
{
    if ((inSize % AES_BLOCK_SIZE) != 0) {
       return (( inSize/ AES_BLOCK_SIZE) +1) * AES_BLOCK_SIZE;
    }
    return inSize;
}

void ResourceEncryptor::CreateEncryptionKey(std::string userKey)
{
    if (userKey.empty()) {
        return;
    }

    AES_KEY decKey;
    const unsigned char* key = reinterpret_cast<unsigned char*>(
                                    const_cast<char*>(userKey.c_str()));

    if ( 0 > AES_set_encrypt_key(key, BITS_SIZE, &m_encKey)) {
        ThrowMsg(ResourceEncryptor::Exception::CreateEncKeyFailed,
                "Failed to create encryption key");
    }
    if ( 0 > AES_set_decrypt_key(key, BITS_SIZE, &decKey)) {
        ThrowMsg(ResourceEncryptor::Exception::CreateDecKeyFailed,
                "Failed to create decryption key");
    }

    std::string encPath, decPath;

    encPath = GetDefaultEncryptKeyPath() + "/" + userKey + ENCRYPTION_FILE;
    decPath = GetDefaultEncryptKeyPath() + "/" + userKey + DECRYPTION_FILE;

    /* TODO : save keys to secure storage */
    LogDebug("Encryption Key path " << encPath);
    LogDebug("Decryption Key path " << decPath);

    FILE* encFp = fopen(encPath.c_str(), "wb");
    if (encFp == NULL) {
        ThrowMsg(ResourceEncryptor::Exception::CreateEncKeyFileFailed,
                "Failed to save encryption key");
    }
    fwrite(&m_encKey, 1, sizeof(m_encKey), encFp);
    fclose(encFp);

    FILE* decFp = fopen(decPath.c_str(), "wb");
    if (decFp == NULL) {
        ThrowMsg(ResourceEncryptor::Exception::CreateDecKeyFileFailed,
                "Failed to save decryption key");
    }

    fwrite(&decKey, 1, sizeof(decKey), decFp);
    fclose(decFp);
    LogDebug("Success to create ecryption and decryption key");
}

AES_KEY ResourceEncryptor::GetEncryptionkey()
{
    return m_encKey;
}

void ResourceEncryptor::EncryptChunk(unsigned char*
        inputBuf, unsigned char* encBuf, size_t chunkSize)
{
    Assert(inputBuf);
    Assert(encBuf);

    unsigned char ivec[16] = {0, };

    AES_cbc_encrypt(inputBuf, encBuf, chunkSize, &m_encKey, ivec, AES_ENCRYPT);
}
} //namespace ResourceEnc
