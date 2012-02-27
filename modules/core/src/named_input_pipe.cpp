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
 * @file        named_input_pipe.cpp
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of named input pipe
 */
#include <dpl/named_input_pipe.h>
#include <dpl/binary_queue.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

namespace DPL
{
namespace // anonymous
{
const size_t DEFAULT_READ_BUFFER_SIZE = 1024;
} // namespace anonymous

NamedInputPipe::NamedInputPipe()
    : m_fifo(-1)
{
}

NamedInputPipe::~NamedInputPipe()
{
    Close();
}

void NamedInputPipe::Open(const std::string& pipeName)
{
    // Open pipe for reading
    int fifo = TEMP_FAILURE_RETRY(open(pipeName.c_str(), O_RDONLY | O_NONBLOCK));

    if (fifo == -1)
        ThrowMsg(Exception::OpenFailed, pipeName);

    m_fifo = fifo;
}

void NamedInputPipe::Close()
{
    if (m_fifo == -1)
        return;

    if (TEMP_FAILURE_RETRY(close(m_fifo)) == -1)
        Throw(Exception::CloseFailed);

    m_fifo = -1;
}

BinaryQueueAutoPtr NamedInputPipe::Read(size_t size)
{
    size_t bytesToRead = size > DEFAULT_READ_BUFFER_SIZE ? DEFAULT_READ_BUFFER_SIZE : size;

    // Malloc default read buffer size
    // It is unmanaged, so it can be then attached directly to binary queue
    void *buffer = malloc(bytesToRead);

    if (buffer == NULL)
        throw std::bad_alloc();

    ssize_t result = TEMP_FAILURE_RETRY(read(m_fifo, buffer, bytesToRead));

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
        (void)lastErrno;

        // FIXME: Handle specific errno
        Throw(AbstractInput::Exception::ReadFailed);
    }
}

int NamedInputPipe::WaitableReadHandle() const
{
    return m_fifo;
}
} // namespace DPL
