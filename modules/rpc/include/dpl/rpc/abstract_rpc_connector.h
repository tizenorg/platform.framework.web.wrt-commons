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
 * @file        abstract_rpc_connector.h
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the header file for abstract RPC connector
 */
#ifndef DPL_ABSTRACT_RPC_CONNECTOR_H
#define DPL_ABSTRACT_RPC_CONNECTOR_H

#include <dpl/rpc/abstract_rpc_connection.h>
#include <dpl/event/event_support.h>
#include <dpl/generic_event.h>

namespace DPL {
namespace RPC {
namespace AbstractRPCConnectorEvents {
/**
 * RPC connection established
 */
DECLARE_GENERIC_EVENT_2(ConnectionEstablishedEvent,
                        AbstractRPCConnectionID,
                        AbstractRPCConnection *)
} // namespace AbstractRPCClientEvents

class AbstractRPCConnector :
    public DPL::Event::EventSupport<AbstractRPCConnectorEvents::
                                        ConnectionEstablishedEvent>
{
  public:
    /**
     * Destructor
     */
    virtual ~AbstractRPCConnector() {}
};
}
} // namespace DPL

#endif // DPL_ABSTRACT_RPC_CONNECTOR_H
