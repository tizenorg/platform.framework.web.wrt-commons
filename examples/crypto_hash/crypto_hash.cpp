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
 * @file        crypto_hash.cpp
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of crypto hash example
 */
#include <stddef.h>
#include <dpl/crypto_hash.h>
#include <iostream>
#include <string>

void help()
{
    std::cout << "Invalid parameters: crypto_hash [hash_function] [message]" << std::endl;
    std::cout << "hash_function is one of following: MD2, MD4, MD5, SHA, SHA1, DSS, DSS1, ECDSA, SHA224, SHA256, SHA384, SHA512" << std::endl;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        help();
        return 0;
    }

    DPL::Crypto::Hash::Base *crypto;
    std::string algorithm = argv[1];

    if (algorithm == "MD2")
        crypto = new DPL::Crypto::Hash::MD2();
    else if (algorithm == "MD4")
        crypto = new DPL::Crypto::Hash::MD4();
    else if (algorithm == "MD5")
        crypto = new DPL::Crypto::Hash::MD5();
    else if (algorithm == "SHA")
        crypto = new DPL::Crypto::Hash::SHA();
    else if (algorithm == "SHA1")
        crypto = new DPL::Crypto::Hash::SHA1();
    else if (algorithm == "DSS")
        crypto = new DPL::Crypto::Hash::DSS();
    else if (algorithm == "DSS1")
        crypto = new DPL::Crypto::Hash::DSS1();
    else if (algorithm == "ECDSA")
        crypto = new DPL::Crypto::Hash::ECDSA();
    else if (algorithm == "SHA224")
        crypto = new DPL::Crypto::Hash::SHA224();
    else if (algorithm == "SHA256")
        crypto = new DPL::Crypto::Hash::SHA256();
    else if (algorithm == "SHA384")
        crypto = new DPL::Crypto::Hash::SHA384();
    else if (algorithm == "SHA512")
        crypto = new DPL::Crypto::Hash::SHA512();
    else
    {
        help();
        return 0;
    }

    crypto->Append(argv[2]);
    crypto->Finish();
    
    std::cout << crypto->ToString() << std::endl;

    delete crypto;

    return 0;
}
