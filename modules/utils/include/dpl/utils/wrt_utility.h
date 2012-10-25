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
 * @file        wrt_utility.h
 * @version     0.8
 * @author      Janusz Majnert <j.majnert@samsung.com>
 * @brief       Common utility functions
 */

#ifndef _WRT_UTILITY_H_
#define _WRT_UTILITY_H_

#include <sys/stat.h>

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
 * This function removes the directory or file pointed to by path
 *
 * @param[in] path  Path to the file/directory to be deleted
 *
 * @return true on success, false otherwise
 */
bool WrtUtilRemove(const std::string &path);

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

#endif //_WRT_UTILITY_H_

