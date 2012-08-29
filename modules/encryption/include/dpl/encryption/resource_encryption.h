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
 * @file    resource_encryption.h
 * @author  Soyoung Kim (sy037.kim@samsung.com)
 * @version 1.0
 * @brief   Header file for resource encryption
 */
#ifndef RESOURCE_ENCRYPTION_H
#define RESOURCE_ENCRYPTION_H

#include <string.h>
#include <string>
#include <openssl/aes.h>
#include <openssl/sha.h>
#include <dpl/exception.h>

namespace WRTEncryptor{

class ResourceEncryptor
{
  public:
      class Exception
      {
          public:
              DECLARE_EXCEPTION_TYPE(DPL::Exception, Base)
              DECLARE_EXCEPTION_TYPE(DPL::Exception, CreateEncKeyFailed)
              DECLARE_EXCEPTION_TYPE(DPL::Exception, CreateDecKeyFailed)
              DECLARE_EXCEPTION_TYPE(DPL::Exception, CreateEncKeyFileFailed)
              DECLARE_EXCEPTION_TYPE(DPL::Exception, CreateDecKeyFileFailed)
              DECLARE_EXCEPTION_TYPE(DPL::Exception, EncryptionFailed)
      };

      ResourceEncryptor();
      virtual ~ResourceEncryptor();

      int GetBlockSize(int inSize);
      void CreateEncryptionKey(std::string userKey);
      void EncryptChunk(unsigned char* inputBuf, unsigned char* encBuf, size_t
              chunkSize);

  private:
      AES_KEY GetEncryptionkey();
      AES_KEY m_encKey;
};
} //namespace WRTEncryptor

#endif /* RESOURCE_DECRYPTION_H */
