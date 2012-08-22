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
 * @file       wrt_utility.cpp
 * @version    0.6
 * @author Wei Dong(d.wei@samsung.com)
 * @author Ma Quan(jason.ma@samsung.com)
 * @brief  This file implemented some common functions for widget manager
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdarg.h>
#include <errno.h>
#include <libgen.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dpl/log/log.h>
#include <dpl/utils/wrt_utility.h>

using namespace std;

bool _WrtUtilSetAbsolutePath(char* absolutePath,
        const char* parentPath,
        const char* fileName)
{
    int len;
    if (NULL == absolutePath || NULL == parentPath || NULL == fileName) {
        return false;
    }
    len = strlen(parentPath);
    if (len > 0) {
        // not check the valid of parentPath and fileName.
        if (parentPath[len - 1] == '/') {
            snprintf(absolutePath,
                     MAX_WIDGET_PATH_LENGTH,
                     "%s%s",
                     parentPath,
                     fileName);
        } else {
            snprintf(absolutePath,
                     MAX_WIDGET_PATH_LENGTH,
                     "%s/%s",
                     parentPath,
                     fileName);
        }
    } else {
        LogDebug("The parent path is null");
        return false;
    }

    //some widget use Windows notation. We need to change '\' to '/'
    for (int i = 0; absolutePath[i] != 0; ++i) {
        if (absolutePath[i] == '\\') {
            absolutePath[i] = '/';
        }
    }

    return true;
}

// KW bool _WrtUtilConvertStrToBool(char* value, bool *result)
// KW {
// KW     bool ret = false;
// KW     if (NULL == value || NULL == result)
// KW     {
// KW         return ret;
// KW     }
// KW
// KW     char* source = value;
// KW     char* changed = (char*)malloc(strlen(value) + 1);
// KW     if (NULL == changed)
// KW     {
// KW         return ret;
// KW     }
// KW     memset(changed, 0, strlen(value) + 1);
// KW
// KW     char* cur = changed;
// KW     while(*source)
// KW     {
// KW         *cur++ = tolower(*source++);
// KW     }
// KW     if (!strcmp(changed,"false") || !strcmp(changed,"0"))
// KW     {
// KW         *result = false;
// KW         ret = true;
// KW     }
// KW     else if(!strcmp(changed,"true") || !strcmp(changed,"1"))
// KW     {
// KW         *result = true;
// KW         ret = true;
// KW     }
// KW     free(changed);
// KW
// KW     return ret;
// KW }

void _WrtUtilGetDirAndFileName(const char* fullPath,
        char** dirName,
        char** fileName)
{
    int length = 0;
    int index = 0;
    if (NULL == fullPath || (NULL == dirName && NULL == fileName)) {
        return;
    }

    length = strlen(fullPath);
    for (index = length - 1; index >= 0; index--) {
        if ('/' == fullPath[index]) {
            if (index == length - 1) {
                LogDebug(" Warning: The end of directroy is '/'! ");
                if (dirName) {
                    *dirName = (char*)malloc(sizeof(char) * (length + 1));
                    if (*dirName != NULL) {
                        memset(*dirName, 0, sizeof(char) * (length + 1));
                        strncpy(*dirName, fullPath, length);
                    }
                }
                return;
            }
            break;
        }
    }
    if (index >= 0) {
        if (dirName) {
            int dirName_len = index + 2;

            *dirName = (char*)malloc(sizeof(char) * dirName_len);
            if (*dirName != NULL) {
                memset(*dirName, 0, sizeof(char) * dirName_len);
                strncpy(*dirName, fullPath, dirName_len - 1);
            }
        }

        if (fileName) {
            int fileName_len = length - index;

            *fileName = (char*)malloc(sizeof(char) * fileName_len);
            if (*fileName != NULL) {
                memset(*fileName, 0, sizeof(char) * fileName_len);
                strncpy(*fileName, &fullPath[index + 1], fileName_len - 1);
            }
        }
    } else {
        if (fileName) {
            *fileName = (char*)malloc(sizeof(char) * (length + 1));
            if (*fileName != NULL) {
                memset(*fileName, 0, sizeof(char) * (length + 1));
                strncpy(*fileName, fullPath, length);
            }
        }
    }
}

// KW bool _WrtUtilStringCmp(const char* srcStr, const char* destStr)
// KW {
// KW     bool ret = false;
// KW     char* strString = NULL;
// KW     char* destString = NULL;
// KW
// KW     if (NULL == srcStr || NULL == destStr )
// KW     {
// KW         return ret;
// KW     }
// KW
// KW     _WrtUtilStringToLower(srcStr, &strString);
// KW     _WrtUtilStringToLower(destStr,&destString);
// KW
// KW     if(!strcmp(strString, destString))
// KW     {
// KW         ret = true;
// KW     }
// KW
// KW     free(strString);
// KW     free(destString);
// KW
// KW     return ret;
// KW }

// check it deeply later.
bool _WrtMakeDir (const char *path,
        long mode,
        int flags)
{
    if (NULL == path) {
        return false;
    }

    const int defaultMode = 0777;
    if (!(flags & WRT_FILEUTILS_RECUR)) {
        if (mkdir(path, defaultMode) < 0) {
            LogDebug("Failed to make dir " << path);
            return false;
        }
        if (mode != -1 && chmod(path, mode) < 0) {
            LogDebug("Failed to chmod");
            remove(path);
            return false;
        }
    } else {
        struct stat st;
        if (stat(path, &st) < 0 && errno == ENOENT) {
            bool ret;
            char *buf = NULL;
            char *parent = NULL;
            //            mode_t mask;

            //            mask = umask (0);
            //            umask (mask);

            buf = strdup(path);
            parent = dirname(buf);
            //ret = _WrtMakeDir(parent, (defaultMode & ~mask) | 0300, WRT_FILEUTILS_RECUR);
            ret = _WrtMakeDir(parent, (defaultMode) | 0300, WRT_FILEUTILS_RECUR);
            free(buf);

            if ((!ret) || (!_WrtMakeDir(path, mode, 0))) {
                LogDebug("Failed to _WrtMakeDir");
                return false;
            }
        }
    }

    return true;
}

bool _WrtUtilChangeDir(const char* path)
{
    if (NULL == path) {
        return false;
    }
    if (-1 == chdir(path)) {
        if (ENOENT == errno) {
            if (!_WrtMakeDir(path, 0664, WRT_FILEUTILS_RECUR)) {
                return false;
            }
            if (-1 == chdir(path)) {
                remove(path);
                return false;
            }
        } else {
            return false;
        }
    }

    return true;
}

bool _WrtUtilRemoveDir(const char* path)
{
    DIR* dir = NULL;
    struct dirent* ptr = NULL;
    char childPath[MAX_WIDGET_PATH_LENGTH + 1] = { 0 };
    if (path == NULL) {
        LogWarning("Path is null");
        return false;
    }
    dir = opendir(path);
    if (NULL != dir) {
        while ((ptr = readdir(dir)) != NULL) {
            if ((!strcmp(ptr->d_name, ".")) || (!strcmp(ptr->d_name, ".."))) {
                continue;
            }
            int len = strlen(path);
            if (path[len - 1] == '/') {
                snprintf(childPath,
                         MAX_WIDGET_PATH_LENGTH,
                         "%s%s",
                         path,
                         ptr->d_name);
            } else {
                snprintf(childPath,
                         MAX_WIDGET_PATH_LENGTH,
                         "%s/%s",
                         path,
                         ptr->d_name);
            }
            if (ptr->d_type == DT_DIR) {
                if (!_WrtUtilRemoveDir(childPath)) {
                    closedir(dir);
                    return false;
                }
            } else {
                if (unlink(childPath) != 0) {
                    closedir(dir);
                    LogWarning("Failed to remove file " << childPath);
                    return false;
                }
            }
        }
        closedir(dir);
    } else if (errno == ENOTDIR) {
        if (unlink(path) != 0) {
            LogWarning("Failed to remove file " << path);
            return false;
        }
        return true;
    } else if (errno == ENOENT) { //not exist
        LogWarning("Cannot remove non existent directory " << path);
        return true;
    } else {
        LogWarning("Can't remove directory " << path);
        return false;
    }
    if (rmdir(path) != 0) {
        LogWarning("Removing directory failed :" << path << " errno: " << errno);
        return false;
    }

    return true;
}

bool
_WrtUtilStringToLower(const char* str,
        char** lowerStr)
{
    if (!str || !lowerStr) {
        return true;
    }

    char* cur = NULL;
    int length = strlen(str);

    *lowerStr = (char*)malloc(length + 1);

    if (!(*lowerStr)) {
        return false;
    }

    memset(*lowerStr, 0, length + 1);
    strncpy(*lowerStr, str, length);

    cur = *lowerStr;

    while (*str != '\0') {
        *cur++ = tolower(*str++);
        //cur++;
    }

    return true;
}

