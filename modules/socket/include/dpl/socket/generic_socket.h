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
 * @file        generic_socket.h
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the header file of generic socket
 */
#ifndef DPL_GENERIC_SOCKET_H
#define DPL_GENERIC_SOCKET_H

#include <dpl/socket/abstract_socket.h>
#include <dpl/waitable_handle_watch_support.h>
#include <dpl/binary_queue.h>
#include <dpl/thread.h>
#include <dpl/main.h>
#include <dpl/log/log.h>
#include <dpl/scoped_free.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <errno.h>
#include <dpl/assert.h>

namespace DPL
{
namespace Socket
{
//
// Generic abstract socket implementation
// Execution context is inherited
//
template<typename SocketType>
class GenericSocket
    : public AbstractSocket,
      private WaitableHandleWatchSupport::WaitableHandleListener
{
protected:
    /**
     * Translate generic Address to specific socket kernel structure
     *
     * @warning Must be implemented in derived class
     */
    virtual std::pair<sockaddr *, socklen_t> TranslateAddressGenericToSpecific(const Address &address) const = 0;

    /**
     * Translate specific socket kernel structure to generic Address
     *
     * @warning Must be implemented in derived class
     */
    virtual Address TranslateAddressSpecificToGeneric(sockaddr *, socklen_t) const = 0;

    /**
     * Get specific socket kernel structure size
     *
     * @warning Must be implemented in derived class
     */
    virtual socklen_t GetSpecificAddressSize() const = 0;

    /**
     * Alloc specific implementation of socket from descriptor
     *
     * @warning Must be implemented in derived class
     */
    virtual SocketType *AllocAcceptedSpecificSocket() const = 0;

    /**
     * Alloc specific implementation of socket descriptor
     *
     * @warning Must be implemented in derived class
     */
    virtual int AllocSpecificDescriptor() const = 0;

private:
    // Constants
    static const size_t DEFAULT_READ_BUFFER_SIZE = 4096;

    // Socket handle
    int m_socket; // FIXME: Consider generalization to WaitableHandle upon leaving nix platform

    // Internal state
    enum InternalState
    {
        InternalState_None,        ///< Not connected and not listening state
        InternalState_Prepare,     ///< Descriptor allocated, but not connected
        InternalState_Listening,   ///< Listening state
        InternalState_Connecting,  ///< Connecting state
        InternalState_Connected    ///< Connected state
    };

    InternalState m_internalState;

    void SetNonBlocking()
    {
        // Set non-blocking mode
        if (fcntl(m_socket, F_SETFL, O_NONBLOCK | fcntl(m_socket, F_GETFL)) == -1)
            Throw(AbstractSocket::Exception::SetNonBlockingFailed);
    }

    // WaitableHandleWatchSupport::WaitableHandleListener
    virtual void OnWaitableHandleEvent(WaitableHandle waitableHandle, WaitMode::Type mode)
    {
        (void)waitableHandle;
        Assert(waitableHandle == m_socket);

        switch (m_internalState)
        {
            case InternalState_None:
                break;

            case InternalState_Prepare:
                Assert(0 && "Invalid internal generic socket state!");
                break;

            case InternalState_Listening:
                Assert(mode == WaitMode::Read);

                // New client waiting for accept
                DPL::Event::EventSupport<AbstractSocketEvents::AcceptEvent>::
                    EmitEvent(AbstractSocketEvents::AcceptEvent(
                        EventSender(this)), DPL::Event::EmitMode::Queued);

                // Done
                break;

            case InternalState_Connecting:
                Assert(mode == WaitMode::Write);

                // Connected to server
                RemoveConnectWatch();
                m_internalState = InternalState_Connected;

                // Add read watch
                AddReadWatch();

                // Emit event
                DPL::Event::EventSupport<AbstractSocketEvents::ConnectedEvent>::
                    EmitEvent(AbstractSocketEvents::ConnectedEvent(
                        EventSender(this)), DPL::Event::EmitMode::Queued);

                // Done
                break;

            case InternalState_Connected:
                if (mode == WaitMode::Read)
                {
                    // Emit ReadEvent
                    DPL::Event::EventSupport<AbstractSocketEvents::ReadEvent>::
                        EmitEvent(AbstractSocketEvents::ReadEvent(
                            EventSender(this)), DPL::Event::EmitMode::Queued);
                }
                else if (mode == WaitMode::Write)
                {
                    // Emit WriteEvent
                    DPL::Event::EventSupport<AbstractSocketEvents::WriteEvent>::
                        EmitEvent(AbstractSocketEvents::WriteEvent(
                            EventSender(this)), DPL::Event::EmitMode::Queued);
                }
                else
                {
                    Assert(0);
                }

                break;

            default:
                Assert(0);
                break;
        }
    }

    void AddAcceptWatch()
    {
        WaitableHandleWatchSupport::InheritedContext()->AddWaitableHandleWatch(this, m_socket, WaitMode::Read);
    }

    void RemoveAcceptWatch()
    {
        WaitableHandleWatchSupport::InheritedContext()->RemoveWaitableHandleWatch(this, m_socket, WaitMode::Read);
    }

    void AddConnectWatch()
    {
        WaitableHandleWatchSupport::InheritedContext()->AddWaitableHandleWatch(this, m_socket, WaitMode::Write);
    }

    void RemoveConnectWatch()
    {
        WaitableHandleWatchSupport::InheritedContext()->RemoveWaitableHandleWatch(this, m_socket, WaitMode::Write);
    }

    void AddReadWatch()
    {
        WaitableHandleWatchSupport::InheritedContext()->AddWaitableHandleWatch(this, m_socket, WaitMode::Read);
    }

    void RemoveReadWatch()
    {
        WaitableHandleWatchSupport::InheritedContext()->RemoveWaitableHandleWatch(this, m_socket, WaitMode::Read);
    }

    void AddWriteWatch()
    {
        WaitableHandleWatchSupport::InheritedContext()->AddWaitableHandleWatch(this, m_socket, WaitMode::Write);
    }

    void RemoveWriteWatch()
    {
        WaitableHandleWatchSupport::InheritedContext()->RemoveWaitableHandleWatch(this, m_socket, WaitMode::Write);
    }

public:
    GenericSocket()
        : m_socket(-1),
          m_internalState(InternalState_None)
    {
    }

    virtual ~GenericSocket()
    {
        // Always ensure to close socket
        Try
        {
            Close();
        }
        Catch(Exception::CloseFailed)
        {
            LogPedantic("Close failed and ignored");
        }

        // Check consistency
        Assert(m_socket == -1);
    }

    virtual void Open()
    {
        if (m_internalState != InternalState_None)
            ThrowMsg(AbstractSocket::Exception::OpenFailed, "Invalid socket state, should be 'None'");

        LogPedantic("Opening socket...");

        // Check consistency
        Assert(m_socket == -1);

        // Allocate specific socket descriptor
        m_socket = AllocSpecificDescriptor();

        // Set socket non-blocking
        SetNonBlocking();

        // State is prepared
        m_internalState = InternalState_Prepare;

        LogPedantic("Socket opened");
    }

    virtual void Connect(const Address &address)
    {
        if (m_internalState != InternalState_Prepare)
            ThrowMsg(AbstractSocket::Exception::ConnectFailed, "Invalid socket state, should be 'Prepare'");

        LogPedantic("Connecting to: " << address.ToString());

        // Try to convert address
        std::pair<sockaddr *, socklen_t> socketAddress;

        Try
        {
            // Translate address to specific socket address struct
            socketAddress = TranslateAddressGenericToSpecific(address);
        }
        Catch (Address::Exception::InvalidAddress)
        {
            // This address is invalid. Cannot connect.
            ReThrowMsg(AbstractSocket::Exception::ConnectFailed, address.ToString());
        }

        // Do connect
        int result = TEMP_FAILURE_RETRY(connect(m_socket, socketAddress.first, socketAddress.second));

        if (result == 0)
        {
            // Immediate connect
            LogPedantic("Immediate connected to: " << address.ToString());

            // Add read watch
            AddReadWatch();
            m_internalState = InternalState_Connected;

            // Emit connected event
            DPL::Event::EventSupport<AbstractSocketEvents::ConnectedEvent>::
                EmitEvent(AbstractSocketEvents::ConnectedEvent(
                    EventSender(this)), DPL::Event::EmitMode::Queued);
        }
        else
        {
            if (errno == EINTR || errno == EINPROGRESS)
            {
                LogPedantic("Asynchronous connect in progress: " << address.ToString());

                // Connecting in progress
                AddConnectWatch();
                m_internalState = InternalState_Connecting;
            }
            else
            {
                // Free translation structure
                free(socketAddress.first);

                // Error occurred
                ThrowMsg(AbstractSocket::Exception::ConnectFailed, address.ToString());
            }
        }

        // Free translation structure
        free(socketAddress.first);
    }

    virtual void Close()
    {
        if (m_internalState == InternalState_None)
            return;

        Assert(m_socket != -1);

        if (m_internalState == InternalState_Listening)
        {
            // Remove watch in listening state
            LogPedantic("Removing accept watch");
            RemoveAcceptWatch();
            m_internalState = InternalState_None;
        }
        else if (m_internalState == InternalState_Connecting)
        {
            // Remove watch in connecting state
            LogPedantic("Removing connect watch");
            RemoveConnectWatch();
            m_internalState = InternalState_None;
        }
        else if (m_internalState == InternalState_Connected)
        {
            // Remove watch in connected state
            LogPedantic("Removing read watch");
            RemoveReadWatch();
            m_internalState = InternalState_None;
        }
        else
        {
            // State must be just prepared only
            Assert(m_internalState == InternalState_Prepare);
        }

        if (TEMP_FAILURE_RETRY(close(m_socket)) == -1)
            Throw(Exception::CloseFailed);

        // Reset socket
        m_socket = -1;

        // Reset socket state
        m_internalState = InternalState_None;

        LogPedantic("Socket closed");
    }

    virtual void Bind(const Address &address)
    {
        if (m_internalState != InternalState_Prepare)
            ThrowMsg(AbstractSocket::Exception::BindFailed, "Invalid socket state, should be 'Prepare'");

        LogPedantic("Binding to: " << address.GetAddress() << ":" << address.GetPort());

        // Translate address to specific socket address struct
        std::pair<sockaddr *, socklen_t> socketAddress = TranslateAddressGenericToSpecific(address);

        // Do bind
        if (::bind(m_socket, socketAddress.first, socketAddress.second) == -1)
            ThrowMsg(AbstractSocket::Exception::BindFailed, address.ToString());

        // Free translation structure
        free(socketAddress.first);

        // Show info
        LogPedantic("Bound to address: " << address.GetAddress() << ":" << address.GetPort());
    }

    virtual void Listen(int backlog)
    {
        if (m_internalState != InternalState_Prepare)
            ThrowMsg(AbstractSocket::Exception::ListenFailed, "Invalid socket state, should be 'None'");

        LogPedantic("Starting to listen...");

        // Do listen
        if (listen(m_socket, backlog) != 0)
            Throw(AbstractSocket::Exception::ListenFailed);

        // Begin read watch
        AddAcceptWatch();
        m_internalState = InternalState_Listening;

        LogPedantic("Listen started");
    }

    virtual AbstractSocket *Accept()
    {
        if (m_internalState != InternalState_Listening)
            ThrowMsg(AbstractSocket::Exception::AcceptFailed, "Invalid socket state, should be 'Listening'");

        LogPedantic("Accepting...");

        // Do listen
        socklen_t length = 0;
        int client = TEMP_FAILURE_RETRY(accept(m_socket, NULL, &length));

        LogPedantic("Socket accept returned " << client);
        if (client == -1)
        {
            // Check if there is any client waiting
            if (errno == EWOULDBLOCK || errno == EAGAIN)
                return NULL;
            int err = errno;
            if (errno == ENOENT)
                return NULL;
            LogPedantic("throwing error. errrno " << err);
            // Error occurred
            Throw(AbstractSocket::Exception::AcceptFailed);
        }

        LogPedantic("Accepted client. Seting up...");

        // Create derived class type
        GenericSocket *acceptedSocket = AllocAcceptedSpecificSocket();

        // Save client socket specific descriptor
        acceptedSocket->m_socket = client;

        // Enter proper states and add read watch
        acceptedSocket->AddReadWatch();
        acceptedSocket->m_internalState = InternalState_Connected;

        // Set non-blocking mode for new socket
        acceptedSocket->SetNonBlocking();

        // Show info
        LogPedantic("Accepted client set up");

        // return new conneced client socket
        return acceptedSocket;
    }

    virtual Address GetLocalAddress() const
    {
        // FIXME: Additional internal state check

        socklen_t length = GetSpecificAddressSize();
        ScopedFree<sockaddr> address(static_cast<sockaddr *>(calloc(static_cast<size_t>(length), 1)));

        if (getsockname(m_socket, address.Get(), &length) == -1)
            ThrowMsg(AbstractSocket::Exception::GetPeerNameFailed, "Failed to get local address");

        return TranslateAddressSpecificToGeneric(address.Get(), length);
    }

    virtual Address GetRemoteAddress() const
    {
        // FIXME: Additional internal state check

        socklen_t length = GetSpecificAddressSize();
        ScopedFree<sockaddr> address(static_cast<sockaddr *>(calloc(static_cast<size_t>(length), 1)));

        if (getpeername(m_socket, address.Get(), &length) == -1)
            ThrowMsg(AbstractSocket::Exception::GetPeerNameFailed, "Failed to get remote address");

        return TranslateAddressSpecificToGeneric(address.Get(), length);
    }

    virtual BinaryQueueAutoPtr Read(size_t size)
    {
        if (m_internalState != InternalState_Connected)
            ThrowMsg(AbstractSocket::Exception::AcceptFailed, "Invalid socket state, should be 'Connected'");

        Try
        {
            // Adjust bytes to be read
            size_t bytesToRead = size > DEFAULT_READ_BUFFER_SIZE ? DEFAULT_READ_BUFFER_SIZE : size;

            // Malloc default read buffer size
            // It is unmanaged, so it can be then attached directly to binary queue
            void *buffer = malloc(bytesToRead);

            if (buffer == NULL)
                throw std::bad_alloc();

            // Receive bytes from socket
            ssize_t result = TEMP_FAILURE_RETRY(recv(m_socket, buffer, bytesToRead, 0));

            if (result > 0)
            {
                // Succedded to read socket data
                BinaryQueueAutoPtr binaryQueue(new BinaryQueue());

                // Append unmanaged memory
                binaryQueue->AppendUnmanaged(buffer, result, &BinaryQueue::BufferDeleterFree, NULL);

                // Return buffer
                return binaryQueue;
            }
            else if (result == 0)
            {
                // Socket was gracefuly closed
                free(buffer);

                // Return empty buffer
                return BinaryQueueAutoPtr(new BinaryQueue());
            }
            else
            {
                // Must first save errno value, because it may be altered
                int lastErrno = errno;

                // Free buffer
                free(buffer);

                // Interpret error result
                switch (lastErrno)
                {
                    case EAGAIN: // = EWOULDBLOCK
                        //
                        // * The  socket's file descriptor is marked O_NONBLOCK and no data is waiting
                        //   to be received; or MSG_OOB is set and no out-of-band data is available
                        //   and either the socket's file descriptor is marked O_NONBLOCK or the socket
                        //   does not support blocking to await out-of-band data.
                        //
                        // return null data buffer to indicate no data waiting
                        //
                        return BinaryQueueAutoPtr();

                    case EBADF:
                        //
                        // * The socket argument is not a valid file descriptor.
                        //
                        // This is internal error
                        //
                        ThrowMsg(CommonException::InternalError, "Invalid socket descriptor");

                    case ECONNRESET:
                        //
                        // * A connection was forcibly closed by a peer.
                        //
                        // In result, we can interpret this error as a broken connection
                        //
                        Throw(AbstractSocket::Exception::ConnectionBroken);

                    case EINTR:
                        //
                        // * The recv() function was interrupted by a signal that was caught, before any
                        //   data was available.
                        //
                        // No interrupt here is expected, due to fact that we used TEMP_FAILURE_RETRY
                        //
                        ThrowMsg(CommonException::InternalError, "Unexpected interrupt occurred");

                    case EINVAL:
                        //
                        // * The MSG_OOB flag is set and no out-of-band data is available.
                        //
                        // We did not specified OOB data. This is an error.
                        //
                        ThrowMsg(CommonException::InternalError, "Unexpected OOB error occurred");

                    case ENOTCONN:
                        //
                        // * A receive is attempted on a connection-mode socket that is not connected.
                        //
                        // FIXME: Is this proper exception here ?
                        //
                        ThrowMsg(CommonException::InternalError, "Socket is not connected");

                    case ENOTSOCK:
                        //
                        // * The socket argument does not refer to a socket.
                        //
                        ThrowMsg(CommonException::InternalError, "Handle is not a socket");

                    case EOPNOTSUPP:
                        //
                        // * The specified flags are not supported for this socket type or protocol.
                        //
                        ThrowMsg(CommonException::InternalError, "Socket flags not supported");

                    case ETIMEDOUT:
                        //
                        // * The connection timed out during connection establishment, or due to a transmission timeout on active connection.
                        //
                        // In result, we can interpret this error as a broken connection
                        //
                        Throw(AbstractSocket::Exception::ConnectionBroken);

                    case EIO:
                        //
                        // * An I/O error occurred while reading from or writing to the file system.
                        //
                        // In result, we can interpret this error as a broken connection
                        //
                        Throw(AbstractSocket::Exception::ConnectionBroken);

                    case ENOBUFS:
                        //
                        // * Insufficient resources were available in the system to perform the operation.
                        //
                        ThrowMsg(CommonException::InternalError, "Insufficient system resources");

                    case ENOMEM:
                        //
                        // * Insufficient memory was available to fulfill the request.
                        //
                        ThrowMsg(CommonException::InternalError, "Insufficient system memory");

                    default:
                        // Some kernel error occurred, should never happen
                        ThrowMsg(CommonException::InternalError, "Unknown kernel read error returned");
                        break;
                }
            }
        }
        Catch (CommonException::InternalError)
        {
            // If any internal error occurred, this is fatal for Write method
            // interpret this as WriteError exception and rethrow
            ReThrow(AbstractSocket::Exception::ReadFailed);
        }
    }

    virtual size_t Write(const BinaryQueue &buffer, size_t bufferSize)
    {
        if (m_internalState != InternalState_Connected)
            ThrowMsg(AbstractSocket::Exception::AcceptFailed, "Invalid socket state, should be 'Connected'");

        Try
        {
            // Adjust write size
            if (bufferSize > buffer.Size())
                bufferSize = buffer.Size();

            // FIXME: User write visitor to write !
            // WriteVisitor visitor

            ScopedFree<void> flattened(malloc(bufferSize));
            buffer.Flatten(flattened.Get(), bufferSize);

            // Linux: MSG_NOSIGNAL is supported, but it is not an ideal solution
            // FIXME: Should we setup signal PIPE ignoring for whole process ?
            // In BSD, there is: setsockopt(c, SOL_SOCKET, SO_NOSIGPIPE, (void *)&on, sizeof(on))
            ssize_t result = TEMP_FAILURE_RETRY(send(m_socket, flattened.Get(), bufferSize, MSG_NOSIGNAL));

            if (result > 0)
            {
                // Successfuly written some bytes
                return static_cast<size_t>(result);
            }
            else if (result == 0)
            {
                // This is abnormal result
                ThrowMsg(CommonException::InternalError, "Invalid socket write result, 0 bytes written");
            }
            else if (result == -1)
            {
                // Interpret error result
                switch (errno)
                {
                    case EAGAIN: // = EWOULDBLOCK
                        //
                        // * The socket's file descriptor is marked O_NONBLOCK and the requested operation would block.
                        //
                        // We should wait for writability
                        //
                        return 0;

                    case EBADF:
                        //
                        // * The socket argument is not a valid file descriptor.
                        //
                        // This is internal error
                        //
                        ThrowMsg(CommonException::InternalError, "Invalid socket descriptor");

                    case ECONNRESET:
                        //
                        // * A connection was forcibly closed by a peer.
                        //
                        // In result, we can interpret this error as a broken connection
                        //
                        Throw(AbstractSocket::Exception::ConnectionBroken);

                    case EDESTADDRREQ:
                        //
                        // * The socket is not connection-mode and no peer address is set.
                        //
                        // FIXME: Is this proper exception here ?
                        //
                        ThrowMsg(CommonException::InternalError, "Socket is not connected");

                    case EINTR:
                        //
                        // * A signal interrupted send() before any data was transmitted.
                        //
                        // No interrupt here is expected, due to fact that we used TEMP_FAILURE_RETRY
                        //
                        ThrowMsg(CommonException::InternalError, "Unexpected interrupt occurred");

                    case EMSGSIZE:
                        //
                        // * The message is too large to be sent all at once, as the socket requires.
                        //
                        // FIXME: Is this proper exception here ?
                        //
                        ThrowMsg(CommonException::InternalError, "Socket message is too big");

                    case ENOTCONN:
                        //
                        // * The socket is not connected or otherwise has not had the peer pre-specified.
                        //
                        // FIXME: Is this proper exception here ?
                        //
                        ThrowMsg(CommonException::InternalError, "Socket is not connected");

                    case ENOTSOCK:
                        //
                        // * The socket argument does not refer to a socket.
                        //
                        ThrowMsg(CommonException::InternalError, "Handle is not a socket");

                    case EOPNOTSUPP:
                        //
                        // * The socket argument is associated with a socket that does not support one or more of the values set in flags.
                        //
                        ThrowMsg(CommonException::InternalError, "Socket flags not supported");

                    case EPIPE:
                        //
                        // * The socket is shut down for writing, or the socket is connection-mode and
                        //   is no longer connected. In the latter case, and if the socket is of type
                        //   SOCK_STREAM, the SIGPIPE signal is  generated to the calling thread.
                        //
                        // In result, we can interpret this error as a broken connection
                        //
                        Throw(AbstractSocket::Exception::ConnectionBroken);

                    case EACCES:
                        //
                        // * The calling process does not have the appropriate privileges.
                        //
                        // Priviledges might have changed.
                        // In result, we can interpret this error as a broken connection
                        //
                        Throw(AbstractSocket::Exception::ConnectionBroken);

                    case EIO:
                        //
                        // * An I/O error occurred while reading from or writing to the file system.
                        //
                        // In result, we can interpret this error as a broken connection
                        //
                        Throw(AbstractSocket::Exception::ConnectionBroken);

                    case ENETDOWN:
                        //
                        // * The local network interface used to reach the destination is down.
                        //
                        // In result, we can interpret this error as a broken connection
                        //
                        Throw(AbstractSocket::Exception::ConnectionBroken);

                    case ENETUNREACH:
                        //
                        // * No route to the network is present.
                        //
                        // In result, we can interpret this error as a broken connection
                        //
                        Throw(AbstractSocket::Exception::ConnectionBroken);

                    case ENOBUFS:
                        //
                        // * Insufficient resources were available in the system to perform the operation.
                        //
                        ThrowMsg(CommonException::InternalError, "Insufficient system resources");

                    default:
                        // Some kernel error occurred, should never happen
                        ThrowMsg(CommonException::InternalError, "Unknown kernel write error returned");
                        break;
                }
            }
        }
        Catch (CommonException::InternalError)
        {
            // If any internal error occurred, this is fatal for Write method
            // interpret this as WriteError exception and rethrow
            ReThrow(AbstractSocket::Exception::WriteFailed);
        }

        // Does not apply
        return 0;
    }

    // AbstractWaitableInput
    virtual WaitableHandle WaitableReadHandle() const
    {
        return m_socket;
    }

    // AbstractWaitableOutput
    virtual WaitableHandle WaitableWriteHandle() const
    {
        return m_socket;
    }
};

}
} // namespace DPL

#endif // DPL_GENERIC_SOCKET_H
