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
 * @file    PathBuilder.cpp
 * @author  Zbigniew Kostrzewa (z.kostrzewa@samsung.com)
 * @version 1.0
 * @brief   Implementation file for PathBuilde class.
 */
#include <stddef.h>
#include <dpl/wrt-dao-ro/path_builder.h>
#include <sstream>

namespace WrtDB {
namespace {
const char PATH_SEPARATOR = '/';
}

class PathBuilderImpl : DPL::Noncopyable
{
  public:
    PathBuilderImpl()
    {
    }

    explicit PathBuilderImpl(const std::string& path) :
        m_stream(path, std::ios_base::app)
    {
    }

    void Append(const std::string& path)
    {
        // TODO Check additionally if last char is not separator.
        if (m_stream.tellp() > 0) {
            m_stream << PATH_SEPARATOR;
        }
        m_stream << path;
    }

    void Concat(const std::string& arg)
    {
        m_stream << arg;
    }

    void Concat(int arg)
    {
        m_stream << arg;
    }

    void Reset()
    {
        m_stream.clear();
        m_stream.str("");
    }

    bool Empty()
    {
        return (m_stream.tellp() == 0);
    }

    std::string GetFullPath() const
    {
        return m_stream.str();
    }

  private:
    std::ostringstream m_stream;
};

PathBuilder::PathBuilder() : m_impl(new PathBuilderImpl())
{
}

PathBuilder::PathBuilder(const std::string& path) :
    m_impl(new PathBuilderImpl(path))
{
}

PathBuilder::~PathBuilder()
{
    delete m_impl;
}

PathBuilder& PathBuilder::Append(const std::string& path)
{
    m_impl->Append(path);
    return *this;
}

PathBuilder& PathBuilder::Concat(const std::string& arg)
{
    m_impl->Concat(arg);
    return *this;
}

PathBuilder& PathBuilder::Concat(int arg)
{
    m_impl->Concat(arg);
    return *this;
}

PathBuilder& PathBuilder::Reset()
{
    m_impl->Reset();
    return *this;
}

bool PathBuilder::Empty() const
{
    return m_impl->Empty();
}

std::string PathBuilder::GetFullPath() const
{
    return m_impl->GetFullPath();
}

} // namespace WrtDB
