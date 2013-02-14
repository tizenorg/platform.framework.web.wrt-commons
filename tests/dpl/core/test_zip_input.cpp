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
 * @file        test_zip_input.cpp
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of zip input tests
 */
#include <dpl/test/test_runner.h>
#include <dpl/zip_input.h>
#include <dpl/foreach.h>
#include <dpl/abstract_waitable_input_adapter.h>
#include <dpl/abstract_waitable_output_adapter.h>
#include <dpl/binary_queue.h>
#include <dpl/scoped_array.h>
#include <dpl/copy.h>
#include <dpl/log/log.h>

namespace {
const char* PATH_NO_FILE = "/opt/share/wrt/wrt-commons/tests/core/no_such_file";
const char* PATH_ARCHIVE = "/opt/share/wrt/wrt-commons/tests/core/sample.zip";
const char* ARCHIVED_FILE = "sample.txt";
}

RUNNER_TEST_GROUP_INIT(DPL)

/*
Name: ZipInput_OpenFailed
Description: tests opening non existing file
Expected: exception throw
*/
RUNNER_TEST(ZipInput_OpenFailed)
{
    bool opened = true;

    Try
    {
        DPL::ZipInput zip(PATH_NO_FILE);
        (void)zip;
    }
    Catch(DPL::ZipInput::Exception::OpenFailed)
    {
        opened = false;
    }

    RUNNER_ASSERT(opened == false);
}

/*
Name: ZipInput_OpenFile
Description: tests opening existing file
Expected: zip stats should mkatch expected
*/
RUNNER_TEST(ZipInput_OpenFile)
{
    DPL::ZipInput zip(PATH_ARCHIVE);

    FOREACH(iter, zip)
    {
        LogDebug("---------");
        LogDebug("FileInfo: ");
#define FIELD(X) LogDebug(#X ": " << iter->X)
        FIELD(name);
        FIELD(comment);
        FIELD(compressedSize);
        FIELD(uncompressedSize);
#undef  FIELD
    }
}

/*
Name: ZipInput_UnzipSingleFile
Description: tests opening existing file and unzipping single file
Expected: right content
*/
RUNNER_TEST(ZipInput_UnzipSingleFile)
{
    DPL::ZipInput zip(PATH_ARCHIVE);
    DPL::ZipInput::File *file = zip.OpenFile(ARCHIVED_FILE);
    DPL::AbstractWaitableInputAdapter fileAdapter(file);
    DPL::BinaryQueue buffer;
    DPL::AbstractWaitableOutputAdapter bufferAdapter(&buffer);

    DPL::Copy(&fileAdapter, &bufferAdapter);

    DPL::ScopedArray<char> data(new char[buffer.Size() + 1]);
    buffer.Flatten(data.Get(), buffer.Size());
    data[buffer.Size()] = '\0';

    RUNNER_ASSERT(std::string(data.Get()) == "test");
}
