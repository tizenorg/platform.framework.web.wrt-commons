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
 * @file        abstract_rpc_connection.h
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the header file for abstract RPC connection
 */
#ifndef DPL_ABSTRACT_RPC_CONNECTION_H
#define DPL_ABSTRACT_RPC_CONNECTION_H

#include <dpl/rpc/rpc_function.h>
#include <dpl/exception.h>
#include <dpl/generic_event.h>
#include <dpl/event/event_support.h>
#include <memory>
#include <string>

namespace DPL {
namespace RPC {
namespace AbstractRPCConnectionEvents {
/**
 * Asynchronous call event
 */
DECLARE_GENERIC_EVENT_1(AsyncCallEvent, RPCFunction)

/**
 * Connection closed event
 */
DECLARE_GENERIC_EVENT_0(ConnectionClosedEvent)

/**
 * Connection broken event
 */
DECLARE_GENERIC_EVENT_0(ConnectionBrokenEvent)
} // namespace AbstractRPCConnectionEvents

class AbstractRPCConnection :
    public DPL::Event::EventSupport<AbstractRPCConnectionEvents::AsyncCallEvent>,
    public DPL::Event::EventSupport<AbstractRPCConnectionEvents::
                                        ConnectionClosedEvent>,
    public DPL::Event::EventSupport<AbstractRPCConnectionEvents::
                                        ConnectionBrokenEvent>
{
  public:
    class Exception
    {
      public:
        DECLARE_EXCEPTION_TYPE(DPL::Exception, Base)
        DECLARE_EXCEPTION_TYPE(Base, AsyncCallFailed)
        DECLARE_EXCEPTION_TYPE(Base, PingFailed)
    };

  public:
    virtual ~AbstractRPCConnection() {}

    /**
     * Call asynchronously RPC function
     *
     * @param function COnstant reference to RPC function to call
     * @return none
     */
    virtual void AsyncCall(const RPCFunction &function) = 0;

    /**
     * Ping RPC connection
     * This will send a ping/pong packet over connection to ensure it is alive
     * If any errors are to occur proper events will be emitted
     *
     * @return none
     */
    virtual void Ping() = 0;
};

/**
 * Abstract connection ID used to represent connection being established
 * or RPC server accepting connection
 */
typedef void *AbstractRPCConnectionID;
}
} // namespace DPL

#endif // DPL_ABSTRACT_RPC_CONNECTION_H
