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
 * @file        copy.cpp
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of copy
 */
#include <stddef.h>
#include <dpl/copy.h>
#include <dpl/waitable_handle.h>
#include <dpl/binary_queue.h>

namespace DPL {
namespace // anonymous
{
const size_t DEFAULT_COPY_BUFFER_SIZE = 16768;
} // namespace anonymous

void Copy(AbstractWaitableInput *input, AbstractWaitableOutput *output)
{
    Try
    {
        while (true) {
            BinaryQueueAutoPtr buffer;

            while (true) {
                // Try to get data immediately
                buffer = input->Read(DEFAULT_COPY_BUFFER_SIZE);

                // Do we need to wait for data ?
                if (!buffer.get()) {
                    WaitForSingleHandle(
                        input->WaitableReadHandle(), WaitMode::Read);
                    continue;
                }

                if (buffer->Empty()) {
                    return; // Done
                }
                // Ok, to process
                break;
            }

            // Write out all data
            while (!buffer->Empty()) {
                // Try to write all data immediately
                size_t count = output->Write(*buffer, buffer->Size());

                // Do we need to wait for writing data ?
                if (count == 0) {
                    WaitForSingleHandle(
                        output->WaitableWriteHandle(), WaitMode::Write);
                    continue;
                }

                // Consume data
                buffer->Consume(count);
            }
        }
    }
    Catch(DPL::Exception)
    {
        ReThrow(CopyFailed);
    }
}

void Copy(AbstractWaitableInput *input,
          AbstractWaitableOutput *output,
          size_t totalBytes)
{
    Try
    {
        size_t bytesLeft = totalBytes;

        while (bytesLeft > 0) {
            BinaryQueueAutoPtr buffer;

            // Copy at most left bytes
            size_t bytesToCopy = bytesLeft >
                DEFAULT_COPY_BUFFER_SIZE ? DEFAULT_COPY_BUFFER_SIZE : bytesLeft;

            while (true) {
                // Try to get data immediately
                buffer = input->Read(bytesToCopy);

                // Do we need to wait for data ?
                if (!buffer.get()) {
                    WaitForSingleHandle(
                        input->WaitableReadHandle(), WaitMode::Read);
                    continue;
                }

                if (buffer->Empty()) {
                    ThrowMsg(CopyFailed, "Unexpected end of abstract input");
                }

                // Ok, to process
                break;
            }

            // Write out all data
            while (!buffer->Empty()) {
                // Try to write all data immediately
                size_t count = output->Write(*buffer, buffer->Size());

                // Do we need to wait for writing data ?
                if (count == 0) {
                    WaitForSingleHandle(
                        output->WaitableWriteHandle(), WaitMode::Write);
                    continue;
                }

                // Consume data
                buffer->Consume(count);
                bytesLeft -= count;
            }
        }
    }
    Catch(DPL::Exception)
    {
        ReThrow(CopyFailed);
    }
}
} // namespace DPL
