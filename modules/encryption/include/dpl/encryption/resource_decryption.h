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
 * @file    resource_decryption.h
 * @author  Soyoung Kim (sy037.kim@samsung.com)
 * @version 1.0
 * @brief   Header file for resource decryption
 */
#ifndef RESOURCE_DECRYPTION_H
#define RESOURCE_DECRYPTION_H

#include <string.h>
#include <string>
#include <openssl/aes.h>
#include <openssl/sha.h>
#include <dpl/exception.h>

extern char** calculate(char*pappId, int idLen, int keyLen);

namespace WRTDecryptor{
class ResourceDecryptor
{
  public:
      class Exception
      {
        public:
            DECLARE_EXCEPTION_TYPE(DPL::Exception, Base)
            DECLARE_EXCEPTION_TYPE(Base, GetDecKeyFailed)
            DECLARE_EXCEPTION_TYPE(Base, EncryptionFailed)
      };

      ResourceDecryptor();
      ResourceDecryptor(std::string userKey);
      virtual ~ResourceDecryptor();

      void SetDecryptionKey(std::string userKey);
      void GetDecryptedChunk(unsigned char* inBuf, unsigned char* decBuf, size_t chunkSize);

  private:
      AES_KEY* GetDecryptionKey();
      AES_KEY m_decKey;

};
} //namespace WRTDecryptor 

#endif /* RESOURCE_DECRYPTION_H */
