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
#include <algorithm>
#include <string>
#include <string.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>

#include <dpl/log/log.h>
#include <dpl/scoped_free.h>

#include "Base64.h"

namespace ValidationCore {
Base64Encoder::Base64Encoder() :
    m_b64(0),
    m_bmem(0),
    m_finalized(false)
{
}

void Base64Encoder::append(const std::string &data)
{
    if (m_finalized) {
        LogWarning("Already finalized.");
        ThrowMsg(Exception::AlreadyFinalized, "Already finalized");
    }

    if (!m_b64) {
        reset();
    }
    BIO_write(m_b64, data.c_str(), data.size());
}

void Base64Encoder::finalize()
{
    if (m_finalized) {
        LogWarning("Already finalized.");
        ThrowMsg(Exception::AlreadyFinalized, "Already finalized.");
    }
    m_finalized = true;
    BIO_flush(m_b64);
}

std::string Base64Encoder::get()
{
    if (!m_finalized) {
        LogWarning("Not finalized");
        ThrowMsg(Exception::NotFinalized, "Not finalized");
    }
    BUF_MEM *bptr = 0;
    BIO_get_mem_ptr(m_b64, &bptr);
    if (bptr == 0) {
        LogError("Bio internal error");
        ThrowMsg(Exception::InternalError, "Bio internal error");
    }

    if (bptr->length > 0) {
        return std::string(bptr->data, bptr->length - 1);
    }
    return std::string();
}

void Base64Encoder::reset()
{
    m_finalized = false;
    BIO_free_all(m_b64);
    m_b64 = BIO_new(BIO_f_base64());
    m_bmem = BIO_new(BIO_s_mem());
    if (!m_b64 || !m_bmem) {
        LogError("Error during allocation memory in BIO");
        ThrowMsg(Exception::InternalError,
                 "Error during allocation memory in BIO");
    }
    m_b64 = BIO_push(m_b64, m_bmem);
}

Base64Encoder::~Base64Encoder()
{
    BIO_free_all(m_b64);
}

Base64Decoder::Base64Decoder() :
    m_finalized(false)
{
}

void Base64Decoder::append(const std::string &data)
{
    if (m_finalized) {
        LogWarning("Already finalized.");
        ThrowMsg(Exception::AlreadyFinalized, "Already finalized.");
    }
    m_input.append(data);
}

static bool whiteCharacter(char a)
{
    if (a == '\n') { return true; }
    return false;
}

bool Base64Decoder::finalize()
{
    if (m_finalized) {
        LogWarning("Already finalized.");
        ThrowMsg(Exception::AlreadyFinalized, "Already finalized.");
    }

    m_finalized = true;

    m_input.erase(std::remove_if(m_input.begin(),
                                 m_input.end(),
                                 whiteCharacter),
                  m_input.end());

    for (size_t i = 0; i<m_input.size(); ++i) {
        if (isalnum(m_input[i])
            || m_input[i] == '+'
            || m_input[i] == '/'
            || m_input[i] == '=')
        {
            continue;
        }
        LogError("Base64 input contains illegal chars: " << m_input[i]);
        return false;
    }

    BIO *b64, *bmem;
    size_t len = m_input.size();

    DPL::ScopedFree<char> buffer(static_cast<char*>(malloc(len)));

    if (!buffer) {
        LogError("Error in malloc.");
        ThrowMsg(Exception::InternalError, "Error in malloc.");
    }

    memset(buffer.Get(), 0, len);
    b64 = BIO_new(BIO_f_base64());
    if (!b64) {
        LogError("Couldn't create BIO object.");
        ThrowMsg(Exception::InternalError, "Couldn't create BIO object.");
    }
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    DPL::ScopedFree<char> tmp(strdup(m_input.c_str()));
    m_input.clear();

    bmem = BIO_new_mem_buf(tmp.Get(), len);

    if (!bmem) {
        BIO_free(b64);
        LogError("Internal error in BIO");
        ThrowMsg(Exception::InternalError, "Internal error in BIO");
    }

    bmem = BIO_push(b64, bmem);

    if (!bmem) {
        BIO_free(b64);
        LogError("Internal error in BIO");
        ThrowMsg(Exception::InternalError, "Internal error in BIO");
    }

    int readlen = BIO_read(bmem, buffer.Get(), len);
    m_output.clear();

    bool status = true;

    if (readlen > 0) {
        m_output.append(buffer.Get(), readlen);
    } else {
        status = false;
    }

    BIO_free_all(bmem);
    return status;
}

std::string Base64Decoder::get() const
{
    if (!m_finalized) {
        LogWarning("Not finalized.");
        ThrowMsg(Exception::NotFinalized, "Not finalized");
    }
    return m_output;
}

void Base64Decoder::reset()
{
    m_finalized = false;
    m_input.clear();
    m_output.clear();
}
} // namespace ValidationCore
