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
 * @file    wrt_utility.cpp
 * @author  Janusz Majnert (j.majnert@samsung.com)
 * @version 1.0
 * @brief   Implementation file for test cases for wrt_utility functions
 */
#include <string>
#include <fstream>
#include <errno.h>
#include <pwd.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <dpl/test/test_runner.h>
#include <dpl/utils/wrt_utility.h>
#include <dpl/log/log.h>

RUNNER_TEST_GROUP_INIT(DPL_WRT_UTILITY)

RUNNER_TEST(wrt_utility_WrtUtilJoinPaths)
{
    std::string result;

    WrtUtilJoinPaths(result, "a/b/c/", "e/f/g.asd");
    RUNNER_ASSERT(result == "a/b/c/e/f/g.asd");

    WrtUtilJoinPaths(result, "/a/b/c", "/e/f/g/");
    RUNNER_ASSERT(result == "/a/b/c/e/f/g/");

    WrtUtilJoinPaths(result, "/a/b/c/", "/e/f/g/");
    RUNNER_ASSERT(result == "/a/b/c/e/f/g/");

    WrtUtilJoinPaths(result, "/a/b/c", "e/f/g/");
    RUNNER_ASSERT(result == "/a/b/c/e/f/g/");
}

/**
 * Create recursive path with specified permissions.
 * Check if folders exist.
 * Check if permissions are set.
 */
RUNNER_TEST(wrt_utility_WrtUtilMakeDir)
{
    struct stat st;
    //First delete the dir if it exists
    WrtUtilRemove("/tmp/test");
    WrtUtilMakeDir("/tmp/test/1/2/3/4/5/6/7/8/9", 0755);
    if (stat("/tmp/test/1/2/3/4/5/6/7/8/9", &st) == 0) {
        RUNNER_ASSERT_MSG(st.st_mode & S_IRWXU,
                          "read, write, execute/search by owner");
        RUNNER_ASSERT_MSG(st.st_mode & S_IXGRP,
                          "execute/search permission, group");
        RUNNER_ASSERT_MSG(st.st_mode & S_IRGRP, "read permission, group");
        RUNNER_ASSERT_MSG(!(st.st_mode & S_IWGRP),
                          "NO write permission, group ");
        RUNNER_ASSERT_MSG(st.st_mode & S_IXOTH,
                          "execute/search permission, others");
        RUNNER_ASSERT_MSG(st.st_mode & S_IROTH, "read permission, others");
        RUNNER_ASSERT_MSG(!(st.st_mode & S_IWOTH),
                          "NO write permission, others ");
    } else {
        RUNNER_ASSERT_MSG(false, "Cannot stat folder");
    }
}

/**
 * Create directory without permission to write.
 */
RUNNER_TEST(wrt_utility_WrtUtilMakeDir_PermissionError)
{
    if (0 == getuid()) {
        //Change UID to execute the test correctly
        errno = 0;
        struct passwd *p = getpwnam("app");
        if (p == NULL) {
            int error = errno;
            RUNNER_ASSERT_MSG(false, "Getting app user UID failed: "
                              << (error ==
                                  0 ? "No error detected" : strerror(error)));
        }
        if (setuid(p->pw_uid) != 0) {
            int error = errno;
            RUNNER_ASSERT_MSG(false, "Changing to app user's UID failed: "
                              << (error ==
                                  0 ? "No error detected" : strerror(error)));
        }
    }
    RUNNER_ASSERT_MSG(WrtUtilMakeDir("/tmp/test2/1",
                                     0055) == false,
                      "Creating directory '1' in /temp/test2/ should have failed");
    //Going back to root UID
    if (setuid(0) != 0) {
        int error = errno;
        LogWarning("Changing back to root UID failed: "
                   << (error == 0 ? "No error detected" : strerror(error)));
    }
}

/**
 * Create directory with file inside.
 * Check if file was removed with directory.
 */
RUNNER_TEST(wrt_utility_WrtUtilRemoveDir) {
    RUNNER_ASSERT_MSG(WrtUtilMakeDir("/tmp/test3/", 0755) == true,
                      "Could not set up directory for test");

    std::ofstream file;
    file.open("/tmp/test3/example.txt");
    file.close();
    struct stat tmp;
    RUNNER_ASSERT_MSG(stat("/tmp/test3/example.txt", &tmp) == 0,
                      "Couldn't create the test file");

    WrtUtilRemove("/tmp/test3");
    if (stat("/tmp/test3", &tmp) != 0) {
        int error = errno;
        RUNNER_ASSERT(error == ENOENT);
        return;
    }
    RUNNER_ASSERT(false);
}

/**
 * Try to remove not existing folder.
 */
RUNNER_TEST(wrt_utility_WrtUtilRemoveDir_NoDirError)
{
    //First making sure the test dir doesn't exist
    WrtUtilRemove("/tmp/NOT_EXISTING");

    RUNNER_ASSERT_MSG(WrtUtilRemove("/tmp/NOT_EXISTING") == false,
                      "Removing non existing directory returned success");
}

RUNNER_TEST(wrt_utility_WrtUtilFileExists)
{
    std::ofstream file;
    file.open("/tmp/test_file1");
    file.close();
    RUNNER_ASSERT(WrtUtilFileExists("/tmp/test_file1"));

    WrtUtilRemove("/tmp/test_file1");
    RUNNER_ASSERT(WrtUtilFileExists("/tmp/test_file1") == false);
}

RUNNER_TEST(wrt_utility_WrtUtilDirExists)
{
    RUNNER_ASSERT(WrtUtilDirExists("/tmp"));
    RUNNER_ASSERT(WrtUtilDirExists("/UNAVAILABLE_DIR") == false);
}
