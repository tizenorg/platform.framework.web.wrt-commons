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
 * @file        abstract_waitable_input_adapter.cpp
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of abstract waitable input adapter
 */
#include <dpl/abstract_waitable_input_adapter.h>

namespace DPL
{

AbstractWaitableInputAdapter::AbstractWaitableInputAdapter(AbstractInput *input)
    : m_input(input)
{
    m_waitableEvent.Signal();
}

BinaryQueueAutoPtr AbstractWaitableInputAdapter::Read(size_t size)
{
    return m_input->Read(size);
}

WaitableHandle AbstractWaitableInputAdapter::WaitableReadHandle() const
{
    return m_waitableEvent.GetHandle();
}

} // namespace DPL
