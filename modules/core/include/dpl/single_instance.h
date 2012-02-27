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
 * @file        single_instance.h
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of single instance
 */
#ifndef DPL_SINGLE_INSTANCE_H
#define DPL_SINGLE_INSTANCE_H

#include <dpl/noncopyable.h>
#include <dpl/exception.h>
#include <string>

namespace DPL
{
class SingleInstance
    : private Noncopyable
{
public:
    class Exception
    {
    public:
        DECLARE_EXCEPTION_TYPE(DPL::Exception, Base)
        DECLARE_EXCEPTION_TYPE(Base, LockError)
        DECLARE_EXCEPTION_TYPE(Base, ReleaseError)
    };

private:
    bool m_locked;
    int m_fdLock;

public:
    SingleInstance();
    virtual ~SingleInstance();

    bool TryLock(const std::string &lockName);
    void Release();
};
} // namespace DPL

#endif // DPL_SINGLE_INSTANCE_H
