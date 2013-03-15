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
/**
 * @file    path.h
 * @author  Tomasz Iwanek (t.iwanek@samsung.com)
 * @version 1.0
 */

#include "dpl/utils/path.h"

#include <dpl/utils/wrt_utility.h>

#include <dpl/scoped_free.h>
#include <dpl/errno_string.h>
#include <dpl/file_input.h>
#include <dpl/file_output.h>
#include <dpl/copy.h>

#include <sys/stat.h>

namespace DPL {

namespace Utils {

Path::Iterator::Iterator() //end iterator by default
{
}

Path::Iterator::Iterator(const char * str)
{
    m_root = std::shared_ptr<Path>(new Path(str));
    m_dir = std::shared_ptr<DIR>(opendir(str), [](DIR * d){ if(d)closedir(d); }); //custom delete
    if(m_dir.get() == NULL)
    {
        ThrowMsg(NotDirectory, "Not directory");
    }
    ReadNext();
}

Path::Iterator& Path::Iterator::operator++()
{
    ReadNext();
    return *this;
}

Path::Iterator Path::Iterator::operator++(int)
{
    Path::Iterator copy(*this);
    ReadNext();
    return copy;
}

void Path::Iterator::ReadNext()
{
    struct dirent * entry = readdir(m_dir.get());
    while(entry && (strcmp(entry->d_name, ".") == 0 ||
          strcmp(entry->d_name, "..") == 0))
    {
        entry = readdir(m_dir.get());
    }
    if(entry)
    {
        m_path = std::shared_ptr<Path>(new Path(*m_root));
        m_path->Append(entry->d_name);
    }
    else //transform into end iterator
    {
        m_path.reset();
        m_dir.reset();
    }
}

bool Path::Iterator::operator==(const Path::Iterator& rhs) const
{
    if(m_dir.get() == NULL)
    {
        if(rhs.m_dir.get() == NULL) return true;
        else return false;
    }
    else
    {
        if(rhs.m_dir.get() == NULL) return false;
    }
    return *m_path == *rhs.m_path;
}

bool Path::Iterator::operator!=(const Path::Iterator& rhs) const
{
    return !this->operator==(rhs);
}

const Path & Path::Iterator::operator*()
{
    return *m_path;
}

const Path * Path::Iterator::operator->()
{
    return m_path.get();
}

Path::Path(const DPL::String & str)
{
    Construct(ToUTF8String(str));
}

Path::Path(const std::string & str)
{
    Construct(str);
}

Path::Path(const char * str)
{
    Construct(std::string(str));
}

void Path::Construct(const std::string & src)
{
    if(src.empty()) ThrowMsg(EmptyPath, "Path cannot be empty");
    if(src[0] != '/')
    {
        DPL::ScopedFree<char> root(getcwd(NULL,0));
        Tokenize(std::string(root.Get()), "\\/", std::inserter(m_parts, m_parts.begin()), true);
    }
    Tokenize(src, "\\/", std::inserter(m_parts, m_parts.end()), true);
}

Path::Path() //for private usage
{
}

std::string Path::DirectoryName() const
{
    if(m_parts.empty()) ThrowMsg(InternalError, "Asking DirectoryName for root directory");
    std::string ret = Join(m_parts.begin(), --m_parts.end(), "/");
    return std::string("/") + ret;
}

std::string Path::Basename() const
{
    if(m_parts.empty()) return "";
    else return m_parts.back();
}

std::string Path::Fullpath() const
{
    std::string ret = Join(m_parts.begin(), m_parts.end(), "/");
    return std::string ("/") + ret;
}

//foreach
Path::Iterator Path::begin() const
{
    if(IsDir())
    {
        return Iterator(Fullpath().c_str());
    }
    else
    {
        ThrowMsg(NotDirectory, "Cannot iterate not a directory");
    }
}

Path::Iterator Path::end() const
{
    return Iterator();
}


bool Path::Exists() const
{
    struct stat tmp;
    return (0 == lstat(Fullpath().c_str(), &tmp));
}

bool Path::IsDir() const
{
    struct stat tmp;
    if (-1 == lstat(Fullpath().c_str(), &tmp))
    {
        ThrowMsg(NotExists, DPL::GetErrnoString());
    }
    return S_ISDIR(tmp.st_mode);
}

bool Path::IsFile() const
{
    struct stat tmp;
    if (-1 == lstat(Fullpath().c_str(), &tmp))
    {
        ThrowMsg(NotExists, DPL::GetErrnoString());
    }
    return S_ISREG(tmp.st_mode);
}

bool Path::IsSymlink() const
{
    struct stat tmp;
    if (-1 == lstat(Fullpath().c_str(), &tmp))
    {
        ThrowMsg(NotExists, DPL::GetErrnoString());
    }
    return S_ISLNK(tmp.st_mode);
}

bool Path::operator==(const Path & other) const
{
    return m_parts == other.m_parts;
}

bool Path::operator!=(const Path & other) const
{
    return m_parts != other.m_parts;
}

Path Path::operator/(const DPL::String& part) const
{
    Path newOne;
    newOne.Append(ToUTF8String(part));
    return newOne;
}

Path Path::operator/(const std::string& part) const
{
    Path newOne;
    newOne.Append(part);
    return newOne;
}

Path Path::operator/(const char * part) const
{
    Path newOne(*this);
    newOne.Append(std::string(part));
    return newOne;
}

Path & Path::operator/=(const DPL::String& part)
{
    Append(ToUTF8String(part));
}

Path & Path::operator/=(const std::string& part)
{
    Append(part);
}

Path & Path::operator/=(const char * part)
{
    Append(std::string(part));
}

void Path::Append(const std::string& part)
{
    std::vector<std::string> tokens;
    Tokenize(part, "\\/", std::inserter(tokens, tokens.end()), true);
    std::copy(tokens.begin(), tokens.end(), std::inserter(m_parts, m_parts.end()));
}

void MakeDir(const Path & path, mode_t mode)
{
    if(!WrtUtilMakeDir(path.Fullpath(), mode)) ThrowMsg(Path::OperationFailed, "Cannot make directory");
}

void MakeEmptyFile(const Path & path)
{
    std::string fp = path.Fullpath();
    FILE* fd = fopen(fp.c_str(), "wx");
    if(!fd)
    {
        struct stat st;
        if(lstat(fp.c_str(), &st) == 0)
        {
            ThrowMsg(Path::AlreadyExists, "File already exists");
        }
        else
        {
            ThrowMsg(Path::OperationFailed, "Operation failed");
        }
    }
    fclose(fd);
}

void Remove(const Path & path)
{
    if(!WrtUtilRemove(path.Fullpath())) ThrowMsg(Path::OperationFailed, "Cannot remove path");
}

void Rename(const Path & from, const Path & to)
{
    if(from == to)
    {
        return;
    }
    int code = 0;
    if( (code = rename(from.Fullpath().c_str(), to.Fullpath().c_str())) )
    {
        if(code == EXDEV)
        {
            Try
            {
                DPL::FileInput in(from.Fullpath());
                DPL::FileOutput out(to.Fullpath());
                DPL::Copy(&in, &out);
            }
            Catch(DPL::FileInput::Exception::Base)
            {
                ThrowMsg(Path::OperationFailed, "Cannot open input file " << from.Fullpath());
            }
            Catch(DPL::FileOutput::Exception::Base)
            {
                ThrowMsg(Path::OperationFailed, "Cannot open output file " << to.Fullpath());
            }
        }
        else
        {
            ThrowMsg(Path::OperationFailed, DPL::GetErrnoString());
        }
    }
}

bool Exists(const Path & path)
{
    return path.Exists();
}

}

}

std::ostream & operator<<(std::ostream & str, const DPL::Utils::Path & path)
{
    str << path.Fullpath();
    return str;
}

//TODO: uncomment when used defiend literals are supported
///DPL::Utils::Path operator""p(const char * str, size_t)
//{
//    return DPL::Utils::Path(str);
//}
