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
 * @file        named_base_pipe.h
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of named base pipe
 */
#ifndef DPL_NAMED_BASE_PIPE_H
#define DPL_NAMED_BASE_PIPE_H

#include <dpl/exception.h>

namespace DPL {
class NamedBasePipe
{
  public:
    class Exception
    {
      public:
        DECLARE_EXCEPTION_TYPE(DPL::Exception, Base)
        DECLARE_EXCEPTION_TYPE(Base, AlreadyExist)
        DECLARE_EXCEPTION_TYPE(Base, CreateFailed)
        DECLARE_EXCEPTION_TYPE(Base, DestroyFailed)
    };

  public:
    virtual ~NamedBasePipe();

    static void Create(const std::string &fileName);
    static void Destroy(const std::string &fileName);
};
} // namespace DPL

#endif // DPL_NAMED_BASE_PIPE_H
