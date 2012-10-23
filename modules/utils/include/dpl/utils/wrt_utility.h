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
#include <sys/stat.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * File options
 */
enum
{
    WRT_FILEUTILS_RECUR = 4 //this is the default for almost all WrtMakeDir calls (once called with 0)
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
 * Joins two paths into one
 *
 * @param[out] joined  String for storing joined paths
 * @param[in] parent   String containing the first part of path
 * @param[in] child    String containing the second part of the path
 *
 * Data stored in joined before the function call will be replaced with joined
 * paths.
 */
void WrtUtilJoinPaths(std::string &joined, const std::string &parent, const std::string &child);

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
 * Creates directories specified by path
 *
 * @param[in]  path    Path to create
 * @param[in]  mode    access flags, default to 0755
 * @return    true on success, false on failure
 *
 * Function creates directory specified by path argument and all directories
 * leading up to it, if they don't exist. Note that if yout wish to create
 * several nested directories, you must make sure that the mode flag allows you
 * to write and search the direcotries you create.
 */
bool WrtUtilMakeDir(const std::string &newpath, mode_t mode=0755);

/**
 * This function is used to remove a directory from the file system.
 *
 * @param[in]  path    Specified the directory path
 *
 * @return    TRUE on success or FALSE on failure.
 */
bool _WrtUtilRemoveDir(const char* path);

/**
 * This function removes the directory or file pointed to by path
 *
 * @param[in] path  Path to the file/directory to be deleted
 *
 * @return true on success, false otherwise
 */
bool WrtUtilRemove(const std::string &path);

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

/**
 * This function converts a string to lowercase
 *
 * @param[in]    in   the string to be converted
 * @param[out] out  the string for placing converted values in
 *
 * The previous data stored in string out will be replaced with converted
 * string from string in
 */
void WrtUtilStringToLower(std::string &out, const std::string &in);

/**
 * Checks if path exists and is a regular file
 *
 * @param[in] path   the string representing path to check
 *
 * @return true if regular file is accessible under path, false otherwise
 */
bool WrtUtilFileExists(const std::string &path);

/**
 * Checks if path exists and is a directory
 *
 * @param[in] path   the string representing path to check
 *
 * @return true if directory is accessible under path, false otherwise
 */
bool WrtUtilDirExists(const std::string &path);

#ifdef __cplusplus
}
#endif

#endif //_WRT_UTILITY_H_

