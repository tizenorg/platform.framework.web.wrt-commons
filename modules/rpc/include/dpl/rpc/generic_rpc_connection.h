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
 * @file        generic_rpc_connection.h
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the header file for generic RPC connection
 */
#ifndef DPL_GENERIC_RPC_CONNECTION_H
#define DPL_GENERIC_RPC_CONNECTION_H

#include <dpl/rpc/abstract_rpc_connection.h>
#include <dpl/abstract_waitable_input_output.h>
#include <dpl/socket/waitable_input_output_execution_context_support.h>
#include <memory>

namespace DPL
{
namespace RPC
{

class GenericRPCConnection
    : public AbstractRPCConnection,
      private DPL::Socket::WaitableInputOutputExecutionContextSupport
{
private:
    // WaitableInputOutputExecutionContextSupport
    virtual void OnInputStreamRead();
    virtual void OnInputStreamClosed();
    virtual void OnInputStreamBroken();

    std::unique_ptr<AbstractWaitableInputOutput> m_inputOutput;

public:
    /**
     * Costructor
     *
     * Abstract waitable input/outobject is acquired by class and destroyed upon desctructor
     */
    explicit GenericRPCConnection(AbstractWaitableInputOutput *inputOutput);
    virtual ~GenericRPCConnection();

    virtual void AsyncCall(const RPCFunction &function);
    virtual void Ping();
};

}
} // namespace DPL

#endif // DPL_GENERIC_RPC_CONNECTION_H
