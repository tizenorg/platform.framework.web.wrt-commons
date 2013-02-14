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
/**
 * @file    PathBuilder.h
 * @author  Zbigniew Kostrzewa (z.kostrzewa@samsung.com)
 * @version 1.0
 * @brief   Header file for PathBuilder class.
 */
#ifndef WRT_UTILS_PATHBUILDER_H
#define WRT_UTILS_PATHBUILDER_H

#include <string>
#include <dpl/noncopyable.h>

namespace WrtDB {
class PathBuilderImpl;

class PathBuilder : private DPL::Noncopyable
{
  public:
    PathBuilder();
    explicit PathBuilder(const std::string& path);

    ~PathBuilder();

    PathBuilder& Append(const std::string& path);

    PathBuilder& Concat(const std::string& arg);
    PathBuilder& Concat(int arg);

    PathBuilder& Reset();

    bool Empty() const;

    std::string GetFullPath() const;

  private:
    PathBuilderImpl* m_impl;
};
} // namespace WrtDB

#endif
