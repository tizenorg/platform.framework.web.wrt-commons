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
 * @file        named_output_pipe.cpp
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of named output pipe
 */
#include <stddef.h>
#include <dpl/named_output_pipe.h>
#include <dpl/binary_queue.h>
#include <dpl/scoped_free.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

namespace DPL {
NamedOutputPipe::NamedOutputPipe() :
    m_fifo(-1)
{}

NamedOutputPipe::~NamedOutputPipe()
{
    Close();
}

void NamedOutputPipe::Open(const std::string& pipeName)
{
    // Then open it for reading or writing
    int fifo = TEMP_FAILURE_RETRY(open(pipeName.c_str(), O_WRONLY | O_NONBLOCK));

    if (fifo == -1) {
        ThrowMsg(Exception::OpenFailed, pipeName);
    }

    m_fifo = fifo;
}

void NamedOutputPipe::Close()
{
    if (m_fifo == -1) {
        return;
    }

    if (TEMP_FAILURE_RETRY(close(m_fifo)) == -1) {
        Throw(Exception::CloseFailed);
    }

    m_fifo = -1;
}

size_t NamedOutputPipe::Write(const BinaryQueue &buffer, size_t bufferSize)
{
    // Adjust write size
    if (bufferSize > buffer.Size()) {
        bufferSize = buffer.Size();
    }

    // FIXME: User write visitor to write !
    // WriteVisitor visitor

    ScopedFree<void> flattened(malloc(bufferSize));
    buffer.Flatten(flattened.Get(), bufferSize);

    ssize_t result =
        TEMP_FAILURE_RETRY(write(m_fifo, flattened.Get(), bufferSize));

    if (result > 0) {
        // Successfuly written some bytes
        return static_cast<size_t>(result);
    } else if (result == 0) {
        // This is abnormal result
        ThrowMsg(CommonException::InternalError,
                 "Invalid socket write result, 0 bytes written");
    } else {
        // Interpret error result
        // FIXME: Handle errno
        Throw(AbstractOutput::Exception::WriteFailed);
    }
}

int NamedOutputPipe::WaitableWriteHandle() const
{
    return m_fifo;
}
} // namespace DPL
