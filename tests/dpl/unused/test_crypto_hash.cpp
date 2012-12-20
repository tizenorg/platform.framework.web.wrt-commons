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
 * @file        test_crypto_hash.cpp
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of test crypto hash
 */
#include <dpl/test_runner.h>
#include <dpl/crypto_hash.h>
RUNNER_TEST_GROUP_INIT(DPL)

#define TEST_CRYPTO_HASH(Class, Input, Output)   \
    Class crypto;                                \
    crypto.Append(Input);                        \
    crypto.Finish();                             \
    RUNNER_ASSERT(crypto.ToString() == Output);

RUNNER_TEST(CryptoHash_MD2)
{
    TEST_CRYPTO_HASH(DPL::Crypto::Hash::MD2, "sample_input_string", "c9f26439c9882cccc98467dbdf07b1fc");
}

RUNNER_TEST(CryptoHash_MD4)
{
    TEST_CRYPTO_HASH(DPL::Crypto::Hash::MD4, "sample_input_string", "8cd0720f7ec98c8e5f008afb54054677");
}

RUNNER_TEST(CryptoHash_MD5)
{
    TEST_CRYPTO_HASH(DPL::Crypto::Hash::MD5, "sample_input_string", "eb7ae4f28fecbd1fd777d9b7495fc8ec");
}

RUNNER_TEST(CryptoHash_SHA)
{
    TEST_CRYPTO_HASH(DPL::Crypto::Hash::SHA, "sample_input_string", "0a5725f3586616a4049730f3ba14c8aeda79ab21");
}

RUNNER_TEST(CryptoHash_SHA1)
{
    TEST_CRYPTO_HASH(DPL::Crypto::Hash::SHA1, "sample_input_string", "be0ed9040af0c2b772b2dd0776f6966b5f4d1206");
}

RUNNER_TEST(CryptoHash_DSS)
{
    TEST_CRYPTO_HASH(DPL::Crypto::Hash::DSS, "sample_input_string", "be0ed9040af0c2b772b2dd0776f6966b5f4d1206");
}

RUNNER_TEST(CryptoHash_DSS1)
{
    TEST_CRYPTO_HASH(DPL::Crypto::Hash::DSS1, "sample_input_string", "be0ed9040af0c2b772b2dd0776f6966b5f4d1206");
}

RUNNER_TEST(CryptoHash_ECDSA)
{
    TEST_CRYPTO_HASH(DPL::Crypto::Hash::ECDSA, "sample_input_string", "be0ed9040af0c2b772b2dd0776f6966b5f4d1206");
}

RUNNER_TEST(CryptoHash_SHA224)
{
    TEST_CRYPTO_HASH(DPL::Crypto::Hash::SHA224, "sample_input_string", "d4dde2370eb869f6e790133b94d58e45417392b9d899af883a274011");
}

RUNNER_TEST(CryptoHash_SHA256)
{
    TEST_CRYPTO_HASH(DPL::Crypto::Hash::SHA256, "sample_input_string", "a470ec7c783ac51f9eb1772132e6bde1a053bbc81650719dd0ac62ecd93caf12");
}

RUNNER_TEST(CryptoHash_SHA384)
{
    TEST_CRYPTO_HASH(DPL::Crypto::Hash::SHA384, "sample_input_string", "63d8bfa95c95c6906d1816965431c065278a655c60f786c9b246c1f73ba7ac557007f5064ba54ebd3a1988e6f37baa97");
}

RUNNER_TEST(CryptoHash_SHA512)
{
    TEST_CRYPTO_HASH(DPL::Crypto::Hash::SHA512, "sample_input_string", "799317a140741937d9e5d8dbf9d3045d2c220de5ac33b3d5897acf873291ed14379eb15ef406d2284313d40edb0e01affac8efeb01cb47c2042e3e62a4a83d7d");
}
