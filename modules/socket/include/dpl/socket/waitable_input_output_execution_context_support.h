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
 * @file        waitable_input_output_execution_context_support.h
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the header file for waitable input-output execution
 * context support
 */
#ifndef DPL_WAITABLE_INPUT_OUTPUT_EXECUTION_CONTEXT_SUPPORT_H
#define DPL_WAITABLE_INPUT_OUTPUT_EXECUTION_CONTEXT_SUPPORT_H

#include <dpl/abstract_waitable_input_output.h>
#include <dpl/waitable_handle_watch_support.h>
#include <dpl/binary_queue.h>

namespace DPL {
namespace Socket {
class WaitableInputOutputExecutionContextSupport :
    private WaitableHandleWatchSupport::WaitableHandleListener
{
  public:
    class Exception
    {
      public:
        DECLARE_EXCEPTION_TYPE(DPL::Exception, Base)
        DECLARE_EXCEPTION_TYPE(Base, AlreadyOpened)
        DECLARE_EXCEPTION_TYPE(Base, NotOpened)
        DECLARE_EXCEPTION_TYPE(Base, OpenFailed)
        DECLARE_EXCEPTION_TYPE(Base, CloseFailed)
    };

  private:
    bool m_opened;
    AbstractWaitableInputOutput *m_waitableInputOutput;

    // Watch state
    bool m_hasReadWatch;
    bool m_hasWriteWatch;

    void AddReadWatch();
    void RemoveReadWatch();
    void AddWriteWatch();
    void RemoveWriteWatch();

    void ReadInput();

    void CheckedRemoveReadWatch();
    void CheckedRemoveWriteWatch();

    void CheckedRemoveReadWriteWatch();

    virtual void OnWaitableHandleEvent(WaitableHandle waitableHandle,
                                       WaitMode::Type mode);

  protected:
    // Incoming/Outgoing streams
    BinaryQueue m_inputStream;
    BinaryQueue m_outputStream;

    // Support calbback methods
    virtual void OnInputStreamRead() = 0;
    virtual void OnInputStreamClosed() = 0;
    virtual void OnInputStreamBroken() = 0;

    // Trigger feeding output - after updating output stream
    void FeedOutput();

    // Open/Close destination waitable input-output
    void Open(AbstractWaitableInputOutput *waitableInputOutput);
    void Close();

  public:
    /**
     * Constructor
     */
    explicit WaitableInputOutputExecutionContextSupport();

    /**
     * Destructor
     */
    virtual ~WaitableInputOutputExecutionContextSupport();
};
}
} // namespace DPL

#endif // DPL_WAITABLE_INPUT_OUTPUT_EXECUTION_CONTEXT_SUPPORT_H
