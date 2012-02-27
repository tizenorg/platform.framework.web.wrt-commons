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
 * @file        waitable_input_output_execution_context_support.cpp
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of waitable input-output execution context support
 */
#include <dpl/socket/waitable_input_output_execution_context_support.h>
#include <dpl/scoped_array.h>
#include <dpl/socket/abstract_socket.h> // FIXME: Remove !!!
#include <dpl/log/log.h>
#include <dpl/assert.h>

namespace DPL
{
namespace Socket
{

namespace // anonymous
{
const size_t DEFAULT_READ_SIZE = 2048;
} // namespace anonymous

WaitableInputOutputExecutionContextSupport::WaitableInputOutputExecutionContextSupport()
    : m_opened(false),
      m_waitableInputOutput(NULL),
      m_hasReadWatch(false),
      m_hasWriteWatch(false)
{
}

WaitableInputOutputExecutionContextSupport::~WaitableInputOutputExecutionContextSupport()
{
    // Ensure support is closed
    Close();
}

void WaitableInputOutputExecutionContextSupport::Open(AbstractWaitableInputOutput *inputOutput)
{
    if (m_opened)
        Throw(Exception::AlreadyOpened);

    LogPedantic("Opening waitable input-output execution context support...");

    // Save IO handle
    m_waitableInputOutput = inputOutput;

    // Register read watch
    Assert(m_hasReadWatch == false);

    AddReadWatch();
    m_hasReadWatch = true;

    // Done
    m_opened = true;

    LogPedantic("Waitable input-output execution context support opened");
}

void WaitableInputOutputExecutionContextSupport::Close()
{
    if (!m_opened)
        return;

    LogPedantic("Closing waitable input-output execution context support...");

    // Remove read and write watches
    CheckedRemoveReadWriteWatch();

    // Set proper state
    m_opened = false;

    LogPedantic("Waitable input-output execution context support closed");
}

void WaitableInputOutputExecutionContextSupport::AddReadWatch()
{
    WaitableHandleWatchSupport::InheritedContext()->AddWaitableHandleWatch(this, m_waitableInputOutput->WaitableReadHandle(), WaitMode::Read);
}

void WaitableInputOutputExecutionContextSupport::RemoveReadWatch()
{
    WaitableHandleWatchSupport::InheritedContext()->RemoveWaitableHandleWatch(this, m_waitableInputOutput->WaitableReadHandle(), WaitMode::Read);
}

void WaitableInputOutputExecutionContextSupport::AddWriteWatch()
{
    WaitableHandleWatchSupport::InheritedContext()->AddWaitableHandleWatch(this, m_waitableInputOutput->WaitableWriteHandle(), WaitMode::Write);
}

void WaitableInputOutputExecutionContextSupport::RemoveWriteWatch()
{
    WaitableHandleWatchSupport::InheritedContext()->RemoveWaitableHandleWatch(this, m_waitableInputOutput->WaitableWriteHandle(), WaitMode::Write);
}

void WaitableInputOutputExecutionContextSupport::CheckedRemoveReadWatch()
{
    if (!m_hasReadWatch)
        return;

    RemoveReadWatch();
    m_hasReadWatch = false;
}

void WaitableInputOutputExecutionContextSupport::CheckedRemoveWriteWatch()
{
    if (!m_hasWriteWatch)
        return;

    RemoveWriteWatch();
    m_hasWriteWatch = false;
}

void WaitableInputOutputExecutionContextSupport::CheckedRemoveReadWriteWatch()
{
    // Remove read watch if any
    CheckedRemoveReadWatch();

    // Remove write watch if any
    CheckedRemoveWriteWatch();
}

void WaitableInputOutputExecutionContextSupport::OnWaitableHandleEvent(WaitableHandle waitableHandle, WaitMode::Type mode)
{
    (void)waitableHandle;

    switch (mode)
    {
        case WaitMode::Read:
            LogPedantic("Read event occurred");

            // Read and parse bytes
            ReadInput();

            // Done
            break;

        case WaitMode::Write:
            LogPedantic("Write event occurred");

            // Push bytes and unregister from write event
            FeedOutput();

            // Unregister write watch only if no more data is available
            if (m_outputStream.Empty())
            {
                Assert(m_hasWriteWatch == true);
                CheckedRemoveWriteWatch();
            }

            // Done
            break;

        default:
            Assert(0);
            break;
    }
}

void WaitableInputOutputExecutionContextSupport::ReadInput()
{
    LogPedantic("Reading input bytes");

    Try
    {
        BinaryQueueAutoPtr inputBuffer = m_waitableInputOutput->Read(DEFAULT_READ_SIZE);

        if (inputBuffer.get() == NULL)
        {
            // No data, should not occur
            LogPedantic("WARNING: Spontaneous ReadSocket occurred");
            return;
        }

        if (inputBuffer->Empty())
        {
            // Connection was closed
            OnInputStreamClosed();

            // Unregister from further event insisting
            Assert(m_hasReadWatch == true);
            CheckedRemoveReadWriteWatch();

            // Set proper state
            m_opened = false;

            // Done
            return;
        }

        LogPedantic("Read " << inputBuffer->Size() << " input bytes");

        // Append all read data
        m_inputStream.AppendMoveFrom(*inputBuffer);
    }
    Catch (AbstractSocket::Exception::ConnectionBroken) // FIXME: Inproper exception abstraction !!!
    {
        // Some errors occurred while feeding abstract IO
        // Interpret connection broken errors, and pass futher other ones
        LogPedantic("Abstract IO connection was broken during read");

        // Signal broken connection
        OnInputStreamBroken();

        // Unregister from further event insisting
        Assert(m_hasReadWatch == true);
        CheckedRemoveReadWriteWatch();

        // Set proper state
        m_opened = false;

        // Do not continue
        return;
    }

    // Interpret data
    OnInputStreamRead();
}

void WaitableInputOutputExecutionContextSupport::FeedOutput()
{
    if (!m_opened)
        Throw(Exception::NotOpened);

    // Anything to feed ?
    if (m_outputStream.Empty())
        return;

    // OK to feed output
    LogPedantic("Feeding output");

    Try
    {
        // Try to write some bytes
        size_t bytes = m_waitableInputOutput->Write(m_outputStream, m_outputStream.Size());

        if (bytes < m_outputStream.Size())
        {
            // Start exhaustive output feeding if it is blocked and not already started
            if (!m_hasWriteWatch)
            {
                AddWriteWatch();
                m_hasWriteWatch = true;

                LogPedantic("Started exhaustive output feeding");
            }
        }

        // Some bytes were written, consume them
        m_outputStream.Consume(bytes);
    }
    Catch (AbstractSocket::Exception::ConnectionBroken) // FIXME: Inproper exception abstraction !!!
    {
        // Some errors occurred while feeding abstract IO
        // Interpret connection broken errors, and pass futher other ones
        LogPedantic("Abstract IO connection was broken during write");

        // Signal broken connection
        OnInputStreamBroken();

        // Unregister from further event insisting
        Assert(m_hasReadWatch == true);
        CheckedRemoveReadWriteWatch();

        // Set proper state
        m_opened = false;

        // Do not continue
        return;
    }
}

}
} // namespace DPL
