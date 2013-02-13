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
 * @file        test_binary_queue.cpp
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of test binary queue
 */
#include <dpl/test/test_runner.h>
#include <dpl/binary_queue.h>
RUNNER_TEST_GROUP_INIT(DPL)

inline std::string BinaryQueueToString(const DPL::BinaryQueue &queue)
{
    char *buffer = new char[queue.Size()];
    queue.Flatten(buffer, queue.Size());
    std::string result = std::string(buffer, buffer + queue.Size());
    delete[] buffer;
    return result;
}

/*
Name: BinaryQueue_InitialEmpty
Description: tests emptiness of new constructed queue
Expected: new queue should be empty
*/
RUNNER_TEST(BinaryQueue_InitialEmpty)
{
    DPL::BinaryQueue queue;
    RUNNER_ASSERT(queue.Empty() == true);
}

/*
Name: BinaryQueue_InitialSize
Description: tests emptiness of new constructed queue
Expected: new queue size should be equal 0
*/
RUNNER_TEST(BinaryQueue_InitialSize)
{
    DPL::BinaryQueue queue;
    RUNNER_ASSERT(queue.Size() == 0);
}

/*
Name: BinaryQueue_InitialCopy
Description: tests emptiness of new copy-constructed empty queue
Expected: queue constructed on base on empty queue should be empty
*/
RUNNER_TEST(BinaryQueue_InitialCopy)
{
    DPL::BinaryQueue queue;
    DPL::BinaryQueue copy = queue;

    RUNNER_ASSERT(copy.Size() == 0);
}

/*
Name: BinaryQueue_InitialConsumeZero
Description: tests consume method accepts 0 bytes
Expected: it should be avaliable to discard 0 bytes from empty queue
*/
RUNNER_TEST(BinaryQueue_InitialConsumeZero)
{
    DPL::BinaryQueue queue;
    queue.Consume(0);
}

/*
Name: BinaryQueue_InitialFlattenConsumeZero
Description: tests returning data from queue and discarding
Expected: it should be able to call flattenconsume with empty buffer if 0 bytes are read
*/
RUNNER_TEST(BinaryQueue_InitialFlattenConsumeZero)
{
    DPL::BinaryQueue queue;
    queue.FlattenConsume(NULL, 0);
}

/*
Name: BinaryQueue_InitialFlattenZero
Description: tests returning data from queue
Expected: it should be able to call flatten with empty buffer if 0 bytes are read
*/
RUNNER_TEST(BinaryQueue_InitialFlattenZero)
{
    DPL::BinaryQueue queue;
    queue.Flatten(NULL, 0);
}

/*
Name: BinaryQueue_InitialConsumeOne
Description: tests discarding more bytes than it is avaliable
Expected: exception throw
*/
RUNNER_TEST(BinaryQueue_InitialConsumeOne)
{
    DPL::BinaryQueue queue;

    Try
    {
        queue.Consume(1);
    }
    Catch(DPL::BinaryQueue::Exception::OutOfData)
    {
        return;
    }

    RUNNER_FAIL;
}

/*
Name: BinaryQueue_InitialFlattenConsumeOne
Description: tests reading and discarding more bytes than it is avaliable
Expected: exception throw
*/
RUNNER_TEST(BinaryQueue_InitialFlattenConsumeOne)
{
    DPL::BinaryQueue queue;

    Try
    {
        char data;
        queue.FlattenConsume(&data, 1);
    }
    Catch(DPL::BinaryQueue::Exception::OutOfData)
    {
        return;
    }

    RUNNER_FAIL;
}

/*
Name: BinaryQueue_InitialFlattenOne
Description: tests reading more bytes than it is avaliable
Expected: exception throw
*/
RUNNER_TEST(BinaryQueue_InitialFlattenOne)
{
    DPL::BinaryQueue queue;

    Try
    {
        char data;
        queue.Flatten(&data, 1);
    }
    Catch(DPL::BinaryQueue::Exception::OutOfData)
    {
        return;
    }

    RUNNER_FAIL;
}

/*
Name: BinaryQueue_ZeroCopyFrom
Description: tests coping content of empty queue to another (AppendCopyFrom)
Expected: source queue should be empty
*/
RUNNER_TEST(BinaryQueue_ZeroCopyFrom)
{
    DPL::BinaryQueue queue;
    DPL::BinaryQueue copy;

    copy.AppendCopyFrom(queue);
    RUNNER_ASSERT(queue.Empty());
}

/*
Name: BinaryQueue_ZeroMoveFrom
Description: tests moving content of empty queue to another
Expected: source queue should be empty
*/
RUNNER_TEST(BinaryQueue_ZeroMoveFrom)
{
    DPL::BinaryQueue queue;
    DPL::BinaryQueue copy;

    copy.AppendMoveFrom(queue);
    RUNNER_ASSERT(queue.Empty());
}

/*
Name: BinaryQueue_ZeroCopyTo
Description: tests moving content of empty queue to another (AppendCopyTo)
Expected: source queue should be empty
*/
RUNNER_TEST(BinaryQueue_ZeroCopyTo)
{
    DPL::BinaryQueue queue;
    DPL::BinaryQueue copy;

    queue.AppendCopyTo(copy);
    RUNNER_ASSERT(queue.Empty());
}

/*
Name: BinaryQueue_InsertSingleCharacters
Description: tests inserting single bytes to queue
Expected: stringified content and size shoudl match expected
*/
RUNNER_TEST(BinaryQueue_InsertSingleCharacters)
{
    DPL::BinaryQueue queue;

    queue.AppendCopy("a", 1);
    queue.AppendCopy("b", 1);
    queue.AppendCopy("c", 1);
    queue.AppendCopy("d", 1);

    RUNNER_ASSERT(queue.Size() == 4);
    RUNNER_ASSERT(BinaryQueueToString(queue) == "abcd");
}

/*
Name: BinaryQueue_Consume
Description: tests comsuming portions of 1 or 2 bytes
Expected: stringified content and size should match expected
 Bytes should be pope from begin.
*/
RUNNER_TEST(BinaryQueue_Consume)
{
    DPL::BinaryQueue queue;

    queue.AppendCopy("abcd", 4);
    queue.AppendCopy("ef", 2);

    RUNNER_ASSERT(queue.Size() == 6);

    queue.Consume(1);
    RUNNER_ASSERT(queue.Size() == 5);
    RUNNER_ASSERT(BinaryQueueToString(queue) == "bcdef");

    queue.Consume(2);
    RUNNER_ASSERT(queue.Size() == 3);
    RUNNER_ASSERT(BinaryQueueToString(queue) == "def");

    queue.Consume(1);
    RUNNER_ASSERT(queue.Size() == 2);
    RUNNER_ASSERT(BinaryQueueToString(queue) == "ef");

    queue.Consume(2);
    RUNNER_ASSERT(queue.Size() == 0);
    RUNNER_ASSERT(BinaryQueueToString(queue) == "");
}

/*
Name: BinaryQueue_Flatten
Description: tests comsuming portions of 1 and more bytes
Expected: stringified content and size should match expected
*/
RUNNER_TEST(BinaryQueue_Flatten)
{
    DPL::BinaryQueue queue;

    queue.AppendCopy("abcd", 4);
    queue.AppendCopy("ef", 2);
    queue.AppendCopy("g", 1);

    RUNNER_ASSERT(queue.Size() == 7);

    RUNNER_ASSERT(BinaryQueueToString(queue) == "abcdefg");
}

/*
Name: BinaryQueue_FlattenConsume
Description: tests comsuming portions of 1 and more bytes
Expected: stringified content and size should match expected
 reading and discarding bytes should affect queue's size and content
*/
RUNNER_TEST(BinaryQueue_FlattenConsume)
{
    DPL::BinaryQueue queue;

    queue.AppendCopy("abcd", 4);
    queue.AppendCopy("ef", 2);

    RUNNER_ASSERT(queue.Size() == 6);

    char buffer[7] = { '\0' };
    queue.FlattenConsume(buffer, 3);

    RUNNER_ASSERT(queue.Size() == 3);
    RUNNER_ASSERT(BinaryQueueToString(queue) == "def");
}

/*
Name: BinaryQueue_AppendCopyFrom
Description: tests creating copy of not empty queue (use of: AppendCopyFrom)
Expected: stringified content and size should match expected
 from original queue and it's copy
*/
RUNNER_TEST(BinaryQueue_AppendCopyFrom)
{
    DPL::BinaryQueue queue;
    DPL::BinaryQueue copy;

    queue.AppendCopy("abcd", 4);
    queue.AppendCopy("ef", 2);

    copy.AppendCopyFrom(queue);

    RUNNER_ASSERT(queue.Size() == 6);
    RUNNER_ASSERT(copy.Size() == 6);
    RUNNER_ASSERT(BinaryQueueToString(queue) == "abcdef");
    RUNNER_ASSERT(BinaryQueueToString(copy) == "abcdef");
}

/*
Name: BinaryQueue_AppendCopyTo
Description: tests creating copy of not empty queue (use of: AppendCopyTo)
Expected: stringified content and size should match expected
 from original queue and it's copy
*/
RUNNER_TEST(BinaryQueue_AppendCopyTo)
{
    DPL::BinaryQueue queue;
    DPL::BinaryQueue copy;

    queue.AppendCopy("abcd", 4);
    queue.AppendCopy("ef", 2);

    queue.AppendCopyTo(copy);

    RUNNER_ASSERT(queue.Size() == 6);
    RUNNER_ASSERT(copy.Size() == 6);
    RUNNER_ASSERT(BinaryQueueToString(queue) == "abcdef");
    RUNNER_ASSERT(BinaryQueueToString(copy) == "abcdef");
}

/*
Name: BinaryQueue_AppendMoveFrom
Description: tests moving content of not empty queue (use of: AppendMoveFrom)
Expected: stringified content and size should match expected
 for new queue. Old queue should be empty after operation
*/
RUNNER_TEST(BinaryQueue_AppendMoveFrom)
{
    DPL::BinaryQueue queue;
    DPL::BinaryQueue copy;

    queue.AppendCopy("abcd", 4);
    queue.AppendCopy("ef", 2);

    copy.AppendMoveFrom(queue);

    RUNNER_ASSERT(queue.Size() == 0);
    RUNNER_ASSERT(copy.Size() == 6);
    RUNNER_ASSERT(BinaryQueueToString(queue) == "");
    RUNNER_ASSERT(BinaryQueueToString(copy) == "abcdef");
}

/*
Name: BinaryQueue_AppendMoveFrom
Description: tests moving content of not empty queue (use of: AppendMoveTo)
Expected: stringified content and size should match expected
 for new queue. Old queue should be empty after operation
*/
RUNNER_TEST(BinaryQueue_AppendMoveTo)
{
    DPL::BinaryQueue queue;
    DPL::BinaryQueue copy;

    queue.AppendCopy("abcd", 4);
    queue.AppendCopy("ef", 2);

    queue.AppendMoveTo(copy);

    RUNNER_ASSERT(queue.Size() == 0);
    RUNNER_ASSERT(copy.Size() == 6);
    RUNNER_ASSERT(BinaryQueueToString(queue) == "");
    RUNNER_ASSERT(BinaryQueueToString(copy) == "abcdef");
}

class Visitor :
    public DPL::BinaryQueue::BucketVisitor
{
  private:
    int m_index;

  public:
    Visitor() :
        m_index(0)
    {}

    virtual void OnVisitBucket(const void *buffer, size_t bufferSize)
    {
        const char *str = static_cast<const char *>(buffer);

        if (m_index == 0) {
            RUNNER_ASSERT(bufferSize == 4);
            RUNNER_ASSERT(str[0] == 'a');
            RUNNER_ASSERT(str[1] == 'b');
            RUNNER_ASSERT(str[2] == 'c');
            RUNNER_ASSERT(str[3] == 'd');
        } else if (m_index == 1) {
            RUNNER_ASSERT(bufferSize == 2);
            RUNNER_ASSERT(str[0] == 'e');
            RUNNER_ASSERT(str[1] == 'f');
        } else {
            RUNNER_FAIL;
        }

        ++m_index;
    }
};

/*
Name: BinaryQueue_Visitor
Description: tests byte by byte content of queue by use of visitor
Expected: stringified content and size should match expected
 Each byte should be at right position
*/
RUNNER_TEST(BinaryQueue_Visitor)
{
    DPL::BinaryQueue queue;

    queue.AppendCopy("abcd", 4);
    queue.AppendCopy("ef", 2);

    Visitor visitor;
    queue.VisitBuckets(&visitor);
}

RUNNER_TEST(BinaryQueue_AbstracInputRead)
{
    DPL::BinaryQueue queue;

    queue.AppendCopy("abcd", 4);

    queue.Read(0);

    RUNNER_ASSERT(BinaryQueueToString(*queue.Read(1).get()) == "a");
    RUNNER_ASSERT(BinaryQueueToString(*queue.Read(2).get()) == "bc");
    RUNNER_ASSERT(BinaryQueueToString(*queue.Read(1).get()) == "d");

    RUNNER_ASSERT(queue.Size() == 0);
}

/*
Name: BinaryQueue_AbstracOutputWrite
Description: tests appending one queue to another
Expected: written bytes shoudl affect content and size of queue
*/
RUNNER_TEST(BinaryQueue_AbstracOutputWrite)
{
    DPL::BinaryQueue queue;
    queue.AppendCopy("abcd", 4);

    DPL::BinaryQueue stream;

    stream.Write(queue, 4);

    RUNNER_ASSERT(BinaryQueueToString(*queue.Read(4).get()) == "abcd");
}
