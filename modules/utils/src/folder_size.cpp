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
 *
 * @file        folder_size.cpp
 * @author      Jaroslaw Osmanski (j.osmanski@samsung.com)
 * @version     1.0
 * @brief       Implementation for function calculating directory size
 */

#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <sstream>
#include <vector>

#include <dpl/log/log.h>
#include <dpl/foreach.h>
#include <dpl/utils/folder_size.h>

namespace Utils {
namespace {

size_t getObjectSize(const std::string& path)
{
    struct stat tmp;

    if (stat(path.c_str(), &tmp) == -1) {
        LogError("Failed to open file" << path);
        return 0;
    }
    //it is not a file nor a directory
    //not counting
    if (!S_ISDIR(tmp.st_mode) && !S_ISREG(tmp.st_mode)) {
        LogWarning("Not a regular file nor a directory: " << path);
        return 0;
    }
    return tmp.st_size;
}
}

size_t getFolderSize(const std::string& path)
{
    size_t size = 0;

    DIR *dir;
    std::vector<std::string> localDirs;
    if ((dir=opendir(path.c_str())) == NULL) {
        LogError("Cannot open dir " << path);
        return 0;
    }
    struct dirent* el;
    while ((el = readdir(dir)) != 0) {
        if (strcmp(el->d_name, ".") == 0 || strcmp(el->d_name, "..") == 0) {
            continue;
        }
        struct stat tmp;
        std::string local = path + el->d_name;
        if (stat(local.c_str(), &tmp) == -1) {
            LogError("Failed to open file " << local);
            char* errstring = strerror(errno);
            LogError("Reason: " << errstring);
            continue;
        }

        size += getObjectSize(local);
        if (S_ISDIR(tmp.st_mode)) {
            localDirs.push_back(local + "/");
        }
    }

    closedir(dir);

    FOREACH (localDir, localDirs) {
        size += getFolderSize(*localDir);
    }

    return size;
}





namespace {
#define DECLARE_PREFIX_STRUCT(name)     \
struct Prefix##name                     \
{                                       \
  static std::string get()              \
  {                                     \
        return std::string(#name);      \
  }                                     \
};                                      \

DECLARE_PREFIX_STRUCT(B)
DECLARE_PREFIX_STRUCT(KB)
DECLARE_PREFIX_STRUCT(MB)
DECLARE_PREFIX_STRUCT(GB)

#undef DECLARE_PREFIX_STRUCT


const int stepSize = 1024;
template<typename... Rest>
struct Pre;

template<typename Postfix, typename... Rest>
struct Pre<Postfix, Rest...>
{
    static const double value = Pre<Rest...>::value * stepSize;
    static std::string printSize(double fileSize)
    {
        if(fileSize >= Pre<Rest...>::value) {
            double now = fileSize / Pre<Rest...>::value;
            std::ostringstream outputStream;
            outputStream.setf(std::ios::fixed, std::ios::floatfield);
            outputStream.precision(2);
            outputStream << now << Postfix::get();
            return outputStream.str();
        } else {
            return Pre<Rest...>::printSize(fileSize);
        }

    }
};

template<>
struct Pre<>
{
        static const double value;
        static std::string printSize(double /*fileSize*/)
        {
                return "0B";
        }

};
const double Pre<>::value = 1.0;

typedef Pre<PrefixGB, PrefixMB, PrefixKB, PrefixB> FolderSizeToStringType;
} //anonymous namespace


DPL::String fromFileSizeString(size_t fileSize)
{

    std::string output =
        FolderSizeToStringType::printSize(static_cast<double>(fileSize));
    return DPL::FromUTF8String(output);
}

} // end of namespace Utils
