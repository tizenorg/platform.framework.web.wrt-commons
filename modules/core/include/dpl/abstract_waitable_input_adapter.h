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
 * @file        abstract_waitable_input.h
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the header file of abstract waitable input
 */
#ifndef DPL_ABSTRACT_WAITABLE_INPUT_ADAPTER_H
#define DPL_ABSTRACT_WAITABLE_INPUT_ADAPTER_H

#include <dpl/abstract_waitable_input.h>
#include <dpl/waitable_event.h>
#include <dpl/abstract_input.h>

namespace DPL
{

class AbstractWaitableInputAdapter
    : public AbstractWaitableInput
{
private:
    AbstractInput *m_input;
    WaitableEvent m_waitableEvent;

public:
    explicit AbstractWaitableInputAdapter(AbstractInput *input);

    virtual BinaryQueueAutoPtr Read(size_t size);

    virtual WaitableHandle WaitableReadHandle() const;
};

} // namespace DPL

#endif // DPL_ABSTRACT_WAITABLE_INPUT_ADAPTER_H
