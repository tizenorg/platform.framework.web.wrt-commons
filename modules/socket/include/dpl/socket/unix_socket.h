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
 * @file        unix_socket.h
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the header file of unix socket
 */
#ifndef DPL_UNIX_SOCKET_H
#define DPL_UNIX_SOCKET_H

#include <dpl/socket/generic_socket.h>
#include <dpl/exception.h>

namespace DPL {
namespace Socket {
class UnixSocket :
    public GenericSocket<UnixSocket>
{
  public:
    class Exception
    {
      public:
        DECLARE_EXCEPTION_TYPE(DPL::Exception, Base)
        DECLARE_EXCEPTION_TYPE(Base, CreateFailed)
    };

  protected:
    /**
     * Translate generic Address to specific socket kernel structure
     */
    virtual std::pair<sockaddr *, socklen_t> TranslateAddressGenericToSpecific(
        const Address &address) const;

    /**
     * Translate specific socket kernel structure to generic Address
     */
    virtual Address TranslateAddressSpecificToGeneric(sockaddr *,
                                                      socklen_t) const;

    /**
     * Get specific socket kernel structure size
     */
    virtual socklen_t GetSpecificAddressSize() const;

    /**
     * Alloc specific implementation of socket from descriptor
     */
    virtual UnixSocket *AllocAcceptedSpecificSocket() const;

    /**
     * Alloc specific implementation of socket descriptor
     */
    virtual int AllocSpecificDescriptor() const;

  public:
    UnixSocket();

    virtual void Bind(const Address &address);
};
}
} // namespace DPL

#endif // DPL_GENERIC_SOCKET_H
