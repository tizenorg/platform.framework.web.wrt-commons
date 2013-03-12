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
 * @file        generic_rpc_connection.cpp
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of generic RPC connection
 */
#include <stddef.h>
#include <dpl/rpc/generic_rpc_connection.h>
#include <dpl/scoped_array.h>
#include <dpl/log/log.h>
#include <dpl/aligned.h>
#include <stdexcept>

namespace DPL
{
namespace RPC
{
namespace // anonymous
{
namespace Protocol
{
// Packet definitions
enum PacketType
{
    PacketType_AsyncCall,
    PacketType_PingPong
};

struct Header
{
    unsigned short size;
    unsigned short type;
} DPL_ALIGNED(1);

struct AsyncCall
    : public Header
{
    unsigned char data[1];
} DPL_ALIGNED(1);

} // namespace Protocol
} // namespace anonymous

GenericRPCConnection::GenericRPCConnection(AbstractWaitableInputOutput *inputOutput)
    : m_inputOutput(inputOutput)
{
    LogPedantic("Opening generic RPC...");
    WaitableInputOutputExecutionContextSupport::Open(inputOutput);
    LogPedantic("Generic RPC opened");
}

GenericRPCConnection::~GenericRPCConnection()
{
    // Ensure RPC is closed
    LogPedantic("Closing generic RPC...");
    WaitableInputOutputExecutionContextSupport::Close();
    LogPedantic("Generic RPC closed");
}

void GenericRPCConnection::AsyncCall(const RPCFunction &function)
{
    LogPedantic("Executing async call");

    // Create binary call
    BinaryQueue serializedCall = function.Serialize();

    // Append buffers
    Protocol::AsyncCall call;
    call.size = static_cast<unsigned short>(serializedCall.Size());
    call.type = Protocol::PacketType_AsyncCall;

    m_outputStream.AppendCopy(&call, sizeof(Protocol::Header));
    m_outputStream.AppendMoveFrom(serializedCall);

    // Try to feed output with data
    Try
    {
        FeedOutput();
    }
    Catch (WaitableInputOutputExecutionContextSupport::Exception::NotOpened)
    {
        // Error occurred while feeding
        ReThrow(AbstractRPCConnection::Exception::AsyncCallFailed);
    }
}

void GenericRPCConnection::Ping()
{
    LogPedantic("Executing ping call");

    // Append buffers
    Protocol::AsyncCall call;
    call.size = 0;
    call.type = Protocol::PacketType_PingPong;

    m_outputStream.AppendCopy(&call, sizeof(Protocol::Header));

    // Try to feed output with data
    Try
    {
        FeedOutput();
    }
    Catch (WaitableInputOutputExecutionContextSupport::Exception::NotOpened)
    {
        // Error occurred while feeding
        ReThrow(AbstractRPCConnection::Exception::PingFailed);
    }
}

void GenericRPCConnection::OnInputStreamRead()
{
    LogPedantic("Interpreting " << m_inputStream.Size() << " bytes buffer");

    // Enough bytes to read at least one header ?
    if (m_inputStream.Size() >= sizeof(Protocol::Header))
    {
        // Begin consuming as much packets as it is possible
        while (m_inputStream.Size() >= sizeof(Protocol::Header))
        {
            Protocol::Header header;
            m_inputStream.Flatten(&header, sizeof(header));

            if (m_inputStream.Size() >= sizeof(Protocol::Header) + header.size)
            {
                LogPedantic("Will parse packet of type: " << header.type);

                // Allocate new packet (header + real packet data)
                void *binaryPacket = malloc(sizeof(Protocol::Header) + header.size);

                if (binaryPacket == NULL)
                    throw std::bad_alloc();

                // Get it from stream
                m_inputStream.FlattenConsume(binaryPacket, sizeof(Protocol::Header) + header.size);

                // Parse specific packet
                switch (header.type)
                {
                    case Protocol::PacketType_AsyncCall:
                        {
                            BinaryQueue call;

                            // No need to delete packet data, we can use it
                            call.AppendUnmanaged(binaryPacket, sizeof(Protocol::Header) + header.size, &BinaryQueue::BufferDeleterFree, NULL);

                            // ...but just remove protocol header
                            call.Consume(sizeof(Protocol::Header));

                            LogPedantic("Async call of size: " << header.size << " parsed");

                            // Call async call event listeners
                            DPL::Event::EventSupport<AbstractRPCConnectionEvents::AsyncCallEvent>::
                                EmitEvent(AbstractRPCConnectionEvents::AsyncCallEvent(
                                    RPCFunction(call), EventSender(this)), DPL::Event::EmitMode::Queued);
                        }
                        break;

                    case Protocol::PacketType_PingPong:
                        {
                            // Reply with ping/pong
                            Ping();

                            // Do not need packet data
                            free(binaryPacket);

                            LogPedantic("Ping pong replied");
                        }
                        break;

                    default:
                        LogPedantic("Warning: Unknown packet type");
                        free(binaryPacket);
                        break;
                }
            }
            else
            {
                LogPedantic("Too few bytes to read packet");
                break;
            }
        }
    }
    else
    {
        LogPedantic("Too few bytes to read header");
    }
}

void GenericRPCConnection::OnInputStreamClosed()
{
    // Emit closed event
    DPL::Event::EventSupport<AbstractRPCConnectionEvents::ConnectionClosedEvent>::
        EmitEvent(AbstractRPCConnectionEvents::ConnectionClosedEvent(
            EventSender(this)), DPL::Event::EmitMode::Queued);
}

void GenericRPCConnection::OnInputStreamBroken()
{
    // Emit broken event
    DPL::Event::EventSupport<AbstractRPCConnectionEvents::ConnectionBrokenEvent>::
        EmitEvent(AbstractRPCConnectionEvents::ConnectionBrokenEvent(
            EventSender(this)), DPL::Event::EmitMode::Queued);
}

}
} // namespace DPL
