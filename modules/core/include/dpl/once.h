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
 * @file        once.h
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of once
 */
#ifndef DPL_ONCE_H
#define DPL_ONCE_H

#include <dpl/noncopyable.h>
#include <dpl/fast_delegate.h>
#include <dpl/atomic.h>
#include <dpl/mutex.h>

namespace DPL
{
class Once
    : private Noncopyable
{
public:
    typedef FastDelegate<void ()> Delegate;

    void Call(Delegate delegate);

private:
    Atomic m_atomic;
    Mutex m_mutex;
};
} // namespace DPL

#endif // DPL_ONCE_H
