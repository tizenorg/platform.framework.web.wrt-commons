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
 * @file        binary_queue.cpp
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of binary queue example
 */
#include <dpl/binary_queue.h>
#include <dpl/exception.h>
#include <dpl/log/log.h>
#include <dpl/assert.h>

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    DPL::BinaryQueue queue;
    Assert(queue.Size() == 0);

    for (int i=0; i<10; ++i)
    {
        int value = 12345;
        queue.AppendCopy(&value, sizeof(value));
        queue.AppendUnmanaged(malloc(100), 100);
    }

    Assert(queue.Size() == 10 * sizeof(int) + 10 * 100);

    for (size_t i = 0; i < 10 * sizeof(int) + 10 * 100; ++i)
    {
        char buffer[1];
        queue.Flatten(buffer, 1);

        queue.FlattenConsume(NULL, 0);
        queue.Flatten(NULL, 0);
        queue.FlattenConsume(buffer, 1);
    }

//    UNHANDLED_EXCEPTION_HANDLER_BEGIN
//    {
//        char a;
//        queue.FlattenConsume(&a, sizeof(a));
//    }
//    UNHANDLED_EXCEPTION_HANDLER_END

    return 0;
}
