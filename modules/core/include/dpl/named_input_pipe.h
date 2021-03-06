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
 * @file        named_input_pipe.h
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of named input pipe
 */
#ifndef DPL_NAMED_PIPE_H
#define DPL_NAMED_PIPE_H

#include <dpl/noncopyable.h>
#include <dpl/exception.h>
#include <dpl/abstract_waitable_input.h>
#include <dpl/named_base_pipe.h>

namespace DPL
{
class NamedInputPipe
    : private Noncopyable,
      public NamedBasePipe,
      public AbstractWaitableInput
{
public:
    class Exception
    {
    public:
        DECLARE_EXCEPTION_TYPE(DPL::Exception, Base)
        DECLARE_EXCEPTION_TYPE(Base, OpenFailed)
        DECLARE_EXCEPTION_TYPE(Base, CloseFailed)
    };

protected:
    int m_fifo;

public:
    NamedInputPipe();
    virtual ~NamedInputPipe();

    void Open(const std::string &fileName);
    void Close();

    // AbstractInput
    virtual BinaryQueueAutoPtr Read(size_t size);

    // AbstractWaitableInput
    virtual WaitableHandle WaitableReadHandle() const;
};
} // namespace DPL

#endif // DPL_NAMED_PIPE_H
