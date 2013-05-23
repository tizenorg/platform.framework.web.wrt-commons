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
 * @file        named_base_pipe.cpp
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of named base pipe
 */
#include <stddef.h>
#include <dpl/named_base_pipe.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

namespace DPL {
namespace // anonymous
{
const mode_t FIFO_MODE = 0600;
} // namespace anonymous

NamedBasePipe::~NamedBasePipe()
{}

void NamedBasePipe::Create(const std::string &pipeName)
{
    // Create new fifo
    int status = mkfifo(pipeName.c_str(), FIFO_MODE);

    if (status == -1) {
        // Ignore error it it already exists
        if (errno == EEXIST) {
            ThrowMsg(Exception::AlreadyExist, pipeName);
        } else {
            ThrowMsg(Exception::CreateFailed, pipeName);
        }
    }
}

void NamedBasePipe::Destroy(const std::string &fileName)
{
    // Destroy fifo
    unlink(fileName.c_str()); // FIXME: Add error handling
}
} // namespace DPL