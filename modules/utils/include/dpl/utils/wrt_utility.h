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
 * @file    wrt_utility.h
 * @version    0.6
 * @author    Wei Dong(d.wei@samsung.com)
 * @author    Ma Quan(jason.ma@samsung.com)
 * @brief     Header file of widget manager common functions
 */

#ifndef _WRT_UTILITY_H_
#define _WRT_UTILITY_H_

#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef MAX_WIDGET_PATH_LENGTH
#define MAX_WIDGET_PATH_LENGTH    1024
#endif

/**
 * File options
 */
enum
{
    WRT_FILEUTILS_PRESERVE_STATUS = 1,
    WRT_FILEUTILS_PRESERVE_SYMLINKS = 2,
    WRT_FILEUTILS_RECUR = 4,
    WRT_FILEUTILS_FORCE = 8,
    WRT_FILEUTILS_INTERACTIVE = 16
};

/**
 * Combine the parentPath and fileName into a new absolute file name.
 *
 * @param[out]    absolutePath
 * @param[in]    parentPath
 * @param[in]    fileName
 *
 * @return    if success, return true; or return false.
 */
bool _WrtUtilSetAbsolutePath(char* absolutePath,
        const char* parentPath,
        const char* fileName);

/**
 * Change the string to bool value,no case sensitive, e.x., "true" or "1" to ture; "False" or "0" to false.
 *
 * @param[in]    value
 * @param[out]    result
 *
 * @return    if success, return true; or return false.
 */
// KW bool _WrtUtilConvertStrToBool(char* value, bool *result);

/**
 * Get the dir path and file name from the full path. e.x., "/opt/lib/filename" as fullPath, "/opt/lib/" as dirName, "filename" as fileName.
 * it's necessary to free *dirName or *fileName if either of them is not NULL.
 *
 * @param[in]    fullPath
 * @param[out]    dirName
 * @param[out]    fileName
 *
 * @return
 */
void _WrtUtilGetDirAndFileName(const char* fullPath,
        char** dirName,
        char** fileName);

#if 0
/**
 * Change the provided string into lower case, caller should allocate the memory of source and dest.
 *
 * @param[in] source    the source string to be changed
 * @param[out] dest    the dest string with lower case
 *
 * return        if success, return true, or return false.
 */
bool _WrtUtilStringToLower(const char* source,
        char* dest);
#endif

/**
 * Compare two string, no case sensitive.
 *
 * @param[in] srcStr
 * @param[in] destStr
 *
 * return        return true if the two strings are identical, or return false.
 */
// KW bool _WrtUtilStringCmp(const char* srcStr, const char* destStr);

/**
 * This function is used to make a directory.
 * it's neccessary to free the returned dir path.
 *
 * @param[in] path    Specified the directory path
 * @param[in] mode    Operation mode the you want to set
 * @param[in] flags   WRT_FILEUTILS_RECUR if you want to make parent's directory recusively,
 *                    WRT_FILEUTILS_NONE if not.
 *
 * @return    TRUE on success or FALSE on failure.
 */
bool _WrtMakeDir (const char *path,
        long mode,
        int flags);

/**
 * This function is used to change to specified directory.
 * If the directory does not exist, it will create it directly.
 *
 * @param[in]  path    Specified the directory path
 *
 * @return    TRUE on success or FALSE on failure.
 */
bool _WrtUtilChangeDir(const char* path);

/**
 * This function is used to remove a directory from the file system.
 *
 * @param[in]  path    Specified the directory path
 *
 * @return    TRUE on success or FALSE on failure.
 */
bool _WrtUtilRemoveDir(const char* path);

// KW /**
// KW  * This function is used to make a temp directory in root directory.
// KW  *
// KW  * @param[in]  root    Specified the root directory
// KW  *
// KW  * @return    if fails, return NULL, else return the temp path.
// KW  * it's necessary to free the returned memory space.
// KW  */
// KW char* _WrtUtilMakeTempDir(const char* root);

/**
 * This function is used to convert a string to lowercase.
 *
 * @param[in]  str            the string need to be converted.
 * @param[in]  lowerStr    the converted string.
 *
 * @return    TRUE on success or FALSE on failure.
 */
bool _WrtUtilStringToLower(const char* str,
        char** lowerStr);

#ifdef __cplusplus
}
#endif

#endif //_WRT_UTILITY_H_

