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
 * @file        test_message_queue.cpp
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of message queue tests
 */
#include <dpl/test_runner.h>
#include <dpl/abstract_waitable_input_adapter.h>
#include <dpl/abstract_waitable_output_adapter.h>
#include <dpl/message_queue.h>
#include <dpl/binary_queue.h>
#include <dpl/copy.h>
#include <dpl/log.h>
#include <dpl/application.h>
#include <dpl/controller.h>
#include <dpl/generic_event.h>

DECLARE_GENERIC_EVENT_0(QuitEvent)

class QuitController :
    public DPL::Controller<DPL::TypeListDecl<QuitEvent>::Type>,
    public DPL::ApplicationExt
{
  public:
    QuitController() : DPL::ApplicationExt(1, NULL, "test-app")
    {
        Touch();
    }

  protected:
    virtual void OnEventReceived(const QuitEvent &)
    {
        Quit();
    }
};

RUNNER_TEST_GROUP_INIT(DPL)

class CopyThread :
    public DPL::Thread
{
  private:
    bool m_success;
    DPL::AbstractWaitableInput *m_input;
    DPL::AbstractWaitableOutput *m_output;
    std::size_t m_dataSize;

  public:
    CopyThread(DPL::AbstractWaitableInput *input,
               DPL::AbstractWaitableOutput *output,
               std::size_t dataSize) :
        m_success(true),
        m_input(input),
        m_output(output),
        m_dataSize(dataSize)
    {
        LogDebug("Thread created");
    }

  protected:
    virtual int ThreadEntry()
    {
        LogDebug("Entering copy thread");

        Try
        {
            DPL::Copy(m_input, m_output, m_dataSize);
        }
        Catch(DPL::CopyFailed)
        {
            m_success = false;

            LogWarning("Copy failed!");
            return 0;
        }

        LogDebug("Copy finished");
        return 0;
    }
};

inline std::string BinaryQueueToString(const DPL::BinaryQueue &queue)
{
    char *buffer = new char[queue.Size()];
    queue.Flatten(buffer, queue.Size());
    std::string result = std::string(buffer, buffer + queue.Size());
    delete[] buffer;
    return result;
}

RUNNER_TEST(MessageQueue_DoubleCopy)
{
    DPL::BinaryQueue dataA;
    DPL::MessageQueue dataB("/test_mqueue_dataB", true, false, 0660, true);
    DPL::MessageQueue dataC("/test_mqueue_dataC", true, false, 0660, true);
    DPL::BinaryQueue dataD;

    DPL::AbstractWaitableInputAdapter dataAdapterA(&dataA);
    DPL::AbstractWaitableOutputAdapter dataAdapterD(&dataD);

    const std::string testData =
        "Lorem ipsum dolor sit amet, consectetur adipiscing elit."
        "Cras elementum venenatis velit, sit amet vehicula odio gravida a."
        "Curabitur id nibh id ante adipiscing sollicitudin."
        "Maecenas in tellus vel augue vehicula pharetra hendrerit cursus est."
        ""
        "Ut malesuada quam porttitor dui euismod lacinia."
        "Phasellus quis lectus sed lectus dictum tincidunt et vitae leo."
        "Fusce id est massa, condimentum bibendum urna."
        "Donec venenatis quam eget sapien vulputate egestas."
        "Maecenas scelerisque lorem a neque molestie a varius erat condimentum."
        "Maecenas varius hendrerit ligula, sed iaculis justo pretium id."
        "Nunc sit amet nisl vitae justo tristique suscipit id eget tortor."
        ""
        "Pellentesque sollicitudin nulla at metus dapibus tincidunt."
        "Integer consequat justo eget dui imperdiet iaculis."
        "Sed vestibulum ipsum vitae libero accumsan non molestie metus adipiscing."
        ""
        "Vivamus quis dui enim, in blandit urna."
        "In imperdiet lacus at orci elementum a scelerisque dui blandit."
        "Donec vulputate enim metus, eget convallis ante."
        "Etiam mollis enim eget eros pulvinar nec sagittis justo fermentum."
        ""
        "Vestibulum sed nunc eu leo lobortis ultrices."
        "Nullam placerat nulla et est blandit nec interdum nunc pulvinar."
        "Vivamus a lectus eget dui fermentum hendrerit.";

    QuitController quitter;
    quitter.PostTimedEvent(QuitEvent(), 1.0);

    CopyThread threadA(&dataAdapterA, &dataB, testData.size());
    CopyThread threadB(&dataB, &dataC, testData.size());
    CopyThread threadC(&dataC, &dataAdapterD, testData.size());

    dataA.AppendCopy(testData.c_str(), testData.size());

    threadA.Run();
    threadB.Run();
    threadC.Run();

    quitter.Exec();

    threadA.Quit();
    threadB.Quit();
    threadC.Quit();

    // Now, test data should be in dataD
    RUNNER_ASSERT(BinaryQueueToString(dataD) == testData);
}
