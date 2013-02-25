/*
 * Copyright (c) 2013 Samsung Electronics Co., Ltd All Rights Reserved
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
/*!
 * @file        scoped_dir.h
 * @author      Iwanek Tomasz (t.iwanek@smasung.com)
 * @version     1.0
 * @brief       This file is the implementation file of scoped directory existence
 */

#ifndef DPL_SCOPED_DIR_H
#define DPL_SCOPED_DIR_H

#include <sys/stat.h>
#include <sys/types.h>
#include <string>
#include <dpl/scoped_resource.h>

namespace DPL {

struct ScopedDirPolicy
{
    typedef std::string Type;
    static Type NullValue();
    static void Destroy(Type ptr);
};

class ScopedDir : public ScopedResource<ScopedDirPolicy>
{
    typedef ScopedDirPolicy Policy;
    typedef ScopedResource<Policy> BaseType;

  public:
    explicit ScopedDir(const std::string & str = Policy::NullValue(), mode_t mode = S_IRWXU|S_IRGRP|S_IXGRP);
};
} // namespace DPL

#endif // DPL_SCOPED_DIR_H
