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
 * @file    file_utils.cpp
 * @author  Bartosz Janiak (b.janiak@samsung.com)
 * @version 1.0
 */


#include <sys/stat.h>
#include <cerrno>
#include <cstring>
#include <cstdlib>
#include <dirent.h>
#include <unistd.h>

#include <dpl/exception.h>
#include <dpl/errno_string.h>
#include <file_utils.h>

#include <dpl/wrt-dao-ro/path_builder.h>

namespace {
int try_mkdir(const char* path,
        mode_t mode)
{
    struct stat st;
    int err = 0;

    if (::stat(path, &st) != 0) {
        if (::mkdir(path, mode) != 0) {
            err = -1;
        }
    } else if (!S_ISDIR(st.st_mode)) {
        errno = ENOTDIR;
        err = -1;
    }

    return err;
}

int mkpath(const char* path,
        mode_t mode)
{
    char* copy = ::strdup(path);
    if (NULL == copy) {
        return -1;
    }

    int err = 0;
    char* ptr = copy;
    char* slash = NULL;

    while ((0 == err) && (NULL != (slash = ::strchr(ptr, '/')))) {
        if (slash != ptr) {
            *slash = '\0';
            err = try_mkdir(copy, mode);
            *slash = '/';
        }
        ptr = slash + 1;
    }

    if (0 == err) {
        err = try_mkdir(path, mode);
    }

    ::free(copy);
    return err;
}

int RmNode(const char* path);

int RmDir(const char* path)
{
    DIR* dir = ::opendir(path);
    if (NULL == dir) {
        return -1;
    }

    struct dirent* entry = NULL;
    do {
        errno = 0;
        if (NULL != (entry = ::readdir(dir))) {
            if (!::strncmp(entry->d_name, ".", 1) ||
                !::strncmp(entry->d_name, "..", 2)) {
                continue;
            }
            std::string fullPath = WrtDB::PathBuilder(path)
                    .Append(entry->d_name)
                    .GetFullPath();
            if (RmNode(fullPath.c_str()) != 0) {
                int error = errno;
                TEMP_FAILURE_RETRY(::closedir(dir));
                errno = error;
                return -1;
            }
        }
    }
    while (NULL != entry);

    int error = errno;
    if (TEMP_FAILURE_RETRY(::closedir(dir)) != 0) {
        return -1;
    }
    errno = error;

    return (errno == 0 ? ::rmdir(path) : -1);
}

int RmNode(const char* path)
{
    struct stat st;
    if (::lstat(path, &st) != 0) {
        return -1;
    }
    return (S_ISDIR(st.st_mode) ? RmDir(path) : ::unlink(path));
}
}

namespace FileUtils {
bool FileExists(const DPL::String& absolutePath)
{
    struct stat statStruct;
    if (stat(DPL::ToUTF8String(absolutePath).c_str(), &statStruct) == 0) {
        return S_ISREG(statStruct.st_mode);
    } else {
        return false;
    }
}

void MakePath(const std::string& path,
        mode_t mode)
{
    if (mkpath(path.c_str(), mode) == -1) {
        ThrowMsg(CreateDirectoryException, "Cannot make path");
    }
}

void RemoveDir(const std::string& path)
{
    if (RmDir(path.c_str()) != 0) {
        ThrowMsg(RemoveDirectoryException, DPL::GetErrnoString());
    }
}
}
