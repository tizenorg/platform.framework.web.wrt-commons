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
 * @file        file_output.cpp
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of file output
 */
#include <stddef.h>
#include <dpl/file_output.h>
#include <dpl/binary_queue.h>
#include <dpl/scoped_free.h>
#include <dpl/log/log.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

namespace DPL
{
FileOutput::FileOutput()
    : m_fd(-1)
{
}

FileOutput::FileOutput(const std::string& fileName)
    : m_fd(-1)
{
    Open(fileName);
}

FileOutput::~FileOutput()
{
    Close();
}

void FileOutput::Open(const std::string& fileName)
{
    // Open non-blocking
    int fd = TEMP_FAILURE_RETRY(open(fileName.c_str(), O_WRONLY | O_CREAT | O_TRUNC | O_NONBLOCK, 0664));

    // Throw an exception if an error occurred
    if (fd == -1)
        ThrowMsg(Exception::OpenFailed, fileName);

    // Close if any existing
    Close();

    // Save new descriptor
    m_fd = fd;

    LogPedantic("Opened file: " << fileName);
}

void FileOutput::Close()
{
    if (m_fd == -1)
        return;

    if (TEMP_FAILURE_RETRY(close(m_fd)) == -1)
        Throw(Exception::CloseFailed);

    m_fd = -1;

    LogPedantic("Closed file");
}

size_t FileOutput::Write(const BinaryQueue &buffer, size_t bufferSize)
{
    // Adjust write size
    if (bufferSize > buffer.Size())
        bufferSize = buffer.Size();

    // FIXME: User write visitor to write !
    // WriteVisitor visitor

    ScopedFree<void> flattened(malloc(bufferSize));
    buffer.Flatten(flattened.Get(), bufferSize);

    LogPedantic("Trying to write " << bufferSize << " bytes");

    ssize_t result = TEMP_FAILURE_RETRY(write(m_fd, flattened.Get(), bufferSize));

    LogPedantic("Wrote " << result << " bytes to file");

    if (result > 0)
    {
        // Successfuly written some bytes
        return static_cast<size_t>(result);
    }
    else if (result == 0)
    {
        // This is abnormal result
        ThrowMsg(CommonException::InternalError, "Invalid write result, 0 bytes written");
    }
    else
    {
        // Interpret error result
        // FIXME: Handle errno
        Throw(AbstractOutput::Exception::WriteFailed);
    }
}

WaitableHandle FileOutput::WaitableWriteHandle() const
{
    return static_cast<WaitableHandle>(m_fd);
}
} // namespace DPL
