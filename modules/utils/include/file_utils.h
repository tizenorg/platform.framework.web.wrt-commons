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
 * @file    file_utils.h
 * @author  Bartosz Janiak (b.janiak@samsung.com)
 * @author  Zbigniew Kostrzewa (z.kostrzewa@samsung.com)
 * @version 1.0
 */

#ifndef FILEUTILS_H
#define FILEUTILS_H

#include <sys/types.h>
#include <string>

#include <dpl/string.h>
#include <dpl/exception.h>

namespace FileUtils {
DECLARE_EXCEPTION_TYPE(DPL::Exception, RemoveDirectoryException)

bool FileExists(const DPL::String& absolutePath);

/**
 * Creates specified path recursively.
 * @param path Path to create (e.g. /tmp/dir1/dir2).
 * @param mode Mode for the non-existing parts of the path (e.g. 0755).
 * @throw DPL::CommonException::::InternalError If sth bad happens.
 */
void MakePath(const std::string& path,
        mode_t mode);

/**
 * Removes specified directory recursively.
 * @param path Full path to directory to remove.
 * @throw FileUtils::DirectoryRemoveException If an error occured.
 */
void RemoveDir(const std::string& path);
};

#endif
