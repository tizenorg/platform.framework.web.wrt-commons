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
 * @file        address.cpp
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of address
 */
#include <dpl/address.h>
#include <sstream>
#include <dpl/assert.h>

namespace DPL
{
Address::Address()
    : m_port(0)
{
}

Address::Address(const std::string &address)
    : m_address(address),
      m_port(0)
{
}

Address::Address(const std::string &address, unsigned short port)
    : m_address(address),
      m_port(port)
{
}

Address::~Address()
{
}

std::string Address::GetAddress() const
{
    return m_address;
}

unsigned short Address::GetPort() const
{
    return m_port;
}

std::string Address::ToString() const
{
    std::ostringstream out;
    out << m_address << ":" << m_port;
    return out.str();
}

bool Address::operator<(const Address &addr) const
{
    return ToString() < addr.ToString();
}

} // namespace DPL
