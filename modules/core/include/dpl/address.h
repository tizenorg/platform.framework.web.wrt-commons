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
 * @file        address.h
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the header file of address
 */
#ifndef DPL_ADDRESS_H
#define DPL_ADDRESS_H

#include <dpl/exception.h>
#include <string>

namespace DPL
{
class Address
{
public:
    class Exception
    {
    public:
        DECLARE_EXCEPTION_TYPE(DPL::Exception, Base)
        DECLARE_EXCEPTION_TYPE(Base, InvalidAddress)
    };

private:
    std::string m_address;
    unsigned short m_port;

public:
    Address();
    Address(const std::string &address);
    Address(const std::string &address, unsigned short port);

    virtual ~Address();

    std::string GetAddress() const;
    unsigned short GetPort() const;

    std::string ToString() const;

    bool operator<(const Address &addr) const;
};

} // namespace DPL

#endif // DPL_ADDRESS_H
