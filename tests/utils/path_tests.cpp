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

#include <set>
#include <memory>

#include <dpl/test/test_runner.h>
#include <dpl/scoped_dir.h>
#include <dpl/scoped_free.h>
#include <dpl/utils/path.h>
#include <dpl/foreach.h>
#include <dpl/log/log.h>
#include <dpl/binary_queue.h>
#include <dpl/file_input.h>
#include <dpl/file_output.h>

#include <sys/stat.h>
#include <unistd.h>

using namespace DPL::Utils;

namespace {
//do not used path here we test it
std::string rootTest = "/tmp/wrttest/";
}

#define ROOTGUARD_TESTMETHOD(FUNC)                                                         \
{                                                                                          \
    bool catched = false;                                                                  \
    Try {                                                                                  \
        FUNC;                                                                              \
    } Catch(Path::RootDirectoryError) {                                                    \
        catched = true;                                                                    \
    }                                                                                      \
    RUNNER_ASSERT_MSG(catched, "Use of method should be protected against root diretory"); \
}                                                                                          \

RUNNER_TEST_GROUP_INIT(DPL_Path)

/*
Name: path_touch
Description: constructs paths in many ways
Expected: success full constrution
*/
RUNNER_TEST(path_mkfile)
{
    DPL::ScopedDir sd(rootTest);

    struct stat st;
    memset(&st, 0, sizeof(struct stat));
    Path path = Path(rootTest) / "touch.txt";
    RUNNER_ASSERT_MSG(lstat(path.Fullpath().c_str(), &st) != 0, "File should not be created");
    RUNNER_ASSERT(!path.Exists());
    MakeEmptyFile(path);
    RUNNER_ASSERT_MSG(lstat(path.Fullpath().c_str(), &st) == 0, "File should be created");
    RUNNER_ASSERT(path.Exists());
    RUNNER_ASSERT(path.IsFile());
    RUNNER_ASSERT(!path.IsDir());
    RUNNER_ASSERT(!path.IsSymlink());
}

/*
Name: path_touch
Description: tries to craete file when it exists
Expected: failure
*/
RUNNER_TEST(path_mkfile_exists)
{
    DPL::ScopedDir sd(rootTest);

    Path path = Path(rootTest) / "touch.txt";
    MakeEmptyFile(path);
    RUNNER_ASSERT(path.Exists());
    bool cannotCreate2ndTime = false;
    Try
    {
        MakeEmptyFile(path);
    }
    Catch(Path::AlreadyExists)
    {
        cannotCreate2ndTime = true;
    }
    RUNNER_ASSERT_MSG(cannotCreate2ndTime, "File created should not be able to be created second time");
}

/*
Name: path_mkfile_invalid_path
Description: tries to create file in not exisitng directory
Expected: failure at creation
*/
RUNNER_TEST(path_mkfile_invalid_path)
{
    DPL::ScopedDir sd(rootTest);

    Path path = Path(rootTest) / "not_existing" / "touch.txt";
    bool cannotCreate = false;
    Try
    {
        MakeEmptyFile(path);
    }
    Catch(Path::OperationFailed)
    {
        cannotCreate = true;
    }
    RUNNER_ASSERT_MSG(cannotCreate, "File created should not be able to be created");
}

/*
Name: path_mkdir
Description: creates valid directory
Expected: success direcotry creation
*/
RUNNER_TEST(path_mkdir)
{
    DPL::ScopedDir sd(rootTest);

    struct stat st;
    memset(&st, 0, sizeof(struct stat));
    Path path = Path(rootTest) / "touchDir";
    RUNNER_ASSERT_MSG(lstat(path.Fullpath().c_str(), &st) != 0, "Directory should not be created");
    RUNNER_ASSERT(!path.Exists());
    MakeDir(path);
    RUNNER_ASSERT_MSG(lstat(path.Fullpath().c_str(), &st) == 0, "Directory should be created");
    RUNNER_ASSERT(path.Exists());
    RUNNER_ASSERT(!path.IsFile());
    RUNNER_ASSERT(path.IsDir());
    RUNNER_ASSERT(!path.IsSymlink());
}

/*
Name: path_symlink
Description: chekc if symlink is correctly recognized
Expected: method isSymlink returns true
*/
RUNNER_TEST(path_symlink)
{
    DPL::ScopedDir sd(rootTest);

    struct stat st;
    memset(&st, 0, sizeof(struct stat));
    Path path = Path(rootTest) / "symlink";
    RUNNER_ASSERT_MSG(lstat(path.Fullpath().c_str(), &st) != 0, "Symlink should not be created");
    RUNNER_ASSERT(!path.Exists());
    (void)symlink("/nonexistisfile/file/file/file ", path.Fullpath().c_str());
    RUNNER_ASSERT_MSG(lstat(path.Fullpath().c_str(), &st) == 0, "Symlink should be created");
    RUNNER_ASSERT(path.Exists());
    RUNNER_ASSERT(!path.IsFile());
    RUNNER_ASSERT(!path.IsDir());
    RUNNER_ASSERT(path.IsSymlink());
}

/*
Name: path_construction_empty
Description: tries to construct empty path
Expected: failure
*/
RUNNER_TEST(path_construction_empty)
{
    bool passed = false;
    Try
    {
        Path path1(std::string(""));
    }
    Catch(Path::EmptyPath)
    {
        passed = true;
    }
    RUNNER_ASSERT_MSG(passed, "Construction with empty path do not fails");
}

/*
Name: path_construction_root
Description: tries to construct root path
Expected: success
*/
RUNNER_TEST(path_construction_root)
{
    Path path1(std::string("/"));
    RUNNER_ASSERT(path1.Fullpath() == "/");
    RUNNER_ASSERT(path1.Filename() == "");
    bool passed = false;
    Try
    {
        RUNNER_ASSERT(path1.DirectoryName() == "/");
    }
    Catch(Path::InternalError)
    {
        passed = true;
    }
    RUNNER_ASSERT_MSG(passed, "Directory name for root should be an error");
}

/*
Name: path_construction_1
Description: constructs paths in many ways
Expected: success full constrution
*/
RUNNER_TEST(path_construction_1)
{
    DPL::ScopedFree<char> sf(getcwd(NULL, 0));

    Path path1(std::string("/test/bin/file"));
    RUNNER_ASSERT(path1.Fullpath() == "/test/bin/file");
    RUNNER_ASSERT(path1.Filename() == "file");
    RUNNER_ASSERT(path1.DirectoryName() == "/test/bin");
}

/*
Name: path_construction_2
Description: constructs paths in many ways
Expected: success full constrution
*/
RUNNER_TEST(path_construction_2)
{
    DPL::ScopedFree<char> sf(getcwd(NULL, 0));
    std::string cwd(sf.Get());

    Path path2(std::string("test/bin/file.eas"));
    RUNNER_ASSERT(path2.Fullpath() == cwd + "/test/bin/file.eas");
    RUNNER_ASSERT(path2.Filename() == "file.eas");
    RUNNER_ASSERT(path2.DirectoryName() == cwd + "/test/bin");
}

/*
Name: path_construction_3
Description: constructs paths in many ways
Expected: success full constrution
*/
RUNNER_TEST(path_construction_3)
{
    DPL::ScopedFree<char> sf(getcwd(NULL, 0));
    std::string cwd(sf.Get());

    Path path3("test/23/abc");
    RUNNER_ASSERT(path3.Fullpath() == cwd + "/test/23/abc");
    RUNNER_ASSERT(path3.Filename() == "abc");
    RUNNER_ASSERT(path3.DirectoryName() == cwd + "/test/23");
}

/*
Name: path_construction_4
Description: constructs paths in many ways
Expected: success full constrution
*/
RUNNER_TEST(path_construction_4)
{
    DPL::ScopedFree<char> sf(getcwd(NULL, 0));

    Path path4("/test/bin/abc");
    RUNNER_ASSERT(path4.Fullpath() == "/test/bin/abc");
    RUNNER_ASSERT(path4.Filename() == "abc");
    RUNNER_ASSERT(path4.DirectoryName() == "/test/bin");
}

/*
Name: path_construction_5
Description: constructs paths in many ways
Expected: success full constrution
*/
RUNNER_TEST(path_construction_5)
{
    DPL::ScopedFree<char> sf(getcwd(NULL, 0));
    std::string cwd(sf.Get());

    Path path5(DPL::String(L"test/bin/file.st.exe"));
    RUNNER_ASSERT(path5.Fullpath() == cwd + "/test/bin/file.st.exe");
    RUNNER_ASSERT(path5.Filename() == "file.st.exe");
    RUNNER_ASSERT(path5.DirectoryName() == cwd + "/test/bin");
}

/*
Name: path_construction_6
Description: constructs paths in many ways
Expected: success full constrution
*/
RUNNER_TEST(path_construction_6)
{
    DPL::ScopedFree<char> sf(getcwd(NULL, 0));

    Path path6(DPL::String(L"/test/bin/file"));
    RUNNER_ASSERT(path6.Fullpath() == "/test/bin/file");
    RUNNER_ASSERT(path6.Filename() == "file");
    RUNNER_ASSERT(path6.DirectoryName() == "/test/bin");
}

/*
Name: path_construction_7
Description: constructs paths in many ways
Expected: success full constrution
*/
RUNNER_TEST(path_construction_7)
{
    DPL::ScopedFree<char> sf(getcwd(NULL, 0));
    std::string cwd(sf.Get());

    Path path7 = Path("test") / "a///23/lol";
    RUNNER_ASSERT(path7.Fullpath() == cwd + "/test/a/23/lol");
    RUNNER_ASSERT(path7.Filename() == "lol");
    RUNNER_ASSERT(path7.DirectoryName() == cwd + "/test/a/23");
}

/*
Name: path_construction_8
Description: constructs paths in many ways
Expected: success full constrution
*/
RUNNER_TEST(path_construction_8)
{
    DPL::ScopedFree<char> sf(getcwd(NULL, 0));

    Path path8 = Path("/test/bin/") / "123" / "dir1.dll";
    RUNNER_ASSERT(path8.Fullpath() == "/test/bin/123/dir1.dll");
    RUNNER_ASSERT(path8.Filename() == "dir1.dll");
    RUNNER_ASSERT(path8.DirectoryName() ==  "/test/bin/123");
}

/*
Name: path_construction_9
Description: constructs paths in many ways
Expected: success full constrution
*/
RUNNER_TEST(path_construction_9)
{
    DPL::ScopedFree<char> sf(getcwd(NULL, 0));

    Path path9 = Path("/test/bin/file.txt//");
    RUNNER_ASSERT(path9.Fullpath() == "/test/bin/file.txt");
    RUNNER_ASSERT(path9.Filename() == "file.txt");
    RUNNER_ASSERT(path9.DirectoryName() ==  "/test/bin");
}

/*
Name: path_construction_10
Description: constructs paths by appending
Expected: success full constrution
*/
RUNNER_TEST(path_construction_10)
{
    Path path10 = Path("/test/");
    path10 /= "one";
    path10 /= std::string("two");
    path10 /= DPL::String(L"three");
    RUNNER_ASSERT(path10.Fullpath() == "/test/one/two/three");
    RUNNER_ASSERT(path10.Filename() == "three");
    RUNNER_ASSERT(path10.DirectoryName() ==  "/test/one/two");
}

/*
Name: path_remove
Description: tests removing paths
Expected: successfull path remove
*/
RUNNER_TEST(path_remove_valid)
{
    DPL::ScopedDir sd(rootTest);

    struct stat st;
    memset(&st, 0, sizeof(struct stat));
    Path path = Path(rootTest) / "touchDir";
    RUNNER_ASSERT(!path.Exists());

    MakeDir(path);
    RUNNER_ASSERT_MSG(lstat(path.Fullpath().c_str(), &st) == 0, "Directory should be created");
    RUNNER_ASSERT(path.Exists());

    Remove(path);
    RUNNER_ASSERT_MSG(lstat(path.Fullpath().c_str(), &st) != 0, "Directory should not be created");
    RUNNER_ASSERT(!path.Exists());

    MakeEmptyFile(path);
    RUNNER_ASSERT_MSG(lstat(path.Fullpath().c_str(), &st) == 0, "File should be created");
    RUNNER_ASSERT(path.Exists());

    Remove(path);
    RUNNER_ASSERT_MSG(lstat(path.Fullpath().c_str(), &st) != 0, "File should not be created");
    RUNNER_ASSERT(!path.Exists());
}

/*
Name: path_try_remove
Description: tests removing paths
Expected: successfull path remove once
*/
RUNNER_TEST(path_try_remove_valid)
{
    DPL::ScopedDir sd(rootTest);

    struct stat st;
    memset(&st, 0, sizeof(struct stat));
    Path path = Path(rootTest) / "touchDir";
    RUNNER_ASSERT(!path.Exists());

    MakeDir(path);
    RUNNER_ASSERT_MSG(lstat(path.Fullpath().c_str(), &st) == 0, "Directory should be created");
    RUNNER_ASSERT(path.Exists());

    RUNNER_ASSERT(TryRemove(path));
    RUNNER_ASSERT(!TryRemove(path));
    RUNNER_ASSERT_MSG(lstat(path.Fullpath().c_str(), &st) != 0, "Directory should not be created");
    RUNNER_ASSERT(!path.Exists());

    MakeEmptyFile(path);
    RUNNER_ASSERT_MSG(lstat(path.Fullpath().c_str(), &st) == 0, "File should be created");
    RUNNER_ASSERT(path.Exists());

    RUNNER_ASSERT(TryRemove(path));
    RUNNER_ASSERT(!TryRemove(path));
    RUNNER_ASSERT_MSG(lstat(path.Fullpath().c_str(), &st) != 0, "File should not be created");
    RUNNER_ASSERT(!path.Exists());
}

/*
Name: path_remove_invalid
Description: tests removing invalid paths
Expected: failure at path remove
*/
RUNNER_TEST(path_remove_invalid)
{
    DPL::ScopedDir sd(rootTest);

    Path path = Path(rootTest) / "touchDir";

    bool removedNotExisting = true;
    Try
    {
        Remove(path);
    }
    Catch(Path::OperationFailed)
    {
        removedNotExisting = false;
    }
    RUNNER_ASSERT_MSG(!removedNotExisting, "Removing not existing file");
}

/*
Name: path_rename
Description: tests path renaming
Expected: path is successfully renamed
*/
RUNNER_TEST(path_rename)
{
    DPL::ScopedDir sd(rootTest);

    struct stat st;
    memset(&st, 0, sizeof(struct stat));
    Path path = Path(rootTest) / "touchDir";
    Path dirpath = Path(rootTest) / "directory";
    Path path2 = dirpath / "touchDir2";

    MakeDir(dirpath);

    MakeEmptyFile(path);
    RUNNER_ASSERT_MSG(lstat(path.Fullpath().c_str(), &st) == 0, "File should be created");
    RUNNER_ASSERT(path.Exists());
    RUNNER_ASSERT(!path2.Exists());

    Rename(path, path2);
    RUNNER_ASSERT(!path.Exists());
    RUNNER_ASSERT(path2.Exists());

    Rename(path2, path);
    RUNNER_ASSERT(path.Exists());
    RUNNER_ASSERT(!path2.Exists());
}

/*
Name: path_rename_xdev
Description: tests path renaming between devices
Expected: path is successfully renamed
*/
RUNNER_TEST(path_rename_xdev)
{
    //assuming /opt/usr and /usr is normally on other partitions on device
    //TODO: better
    Path path = Path("/opt/usr") / "touchDir";
    Path dirpath = path / "directory";
    Path filepath = path / "file.txt";

    Path path2 = Path("/usr") / "touchDir2";
    Path dirpath2 = path2 / "directory";
    Path filepath2 = path2 / "file.txt";

    TryRemove(path);

    MakeDir(path);
    MakeDir(dirpath);
    MakeEmptyFile(filepath);

    RUNNER_ASSERT(path.Exists());
    RUNNER_ASSERT(dirpath.Exists());
    RUNNER_ASSERT(filepath.Exists());
    RUNNER_ASSERT(!path2.Exists());
    RUNNER_ASSERT(!dirpath2.Exists());
    RUNNER_ASSERT(!filepath2.Exists());

    Rename(path, path2);
    RUNNER_ASSERT(!path.Exists());
    RUNNER_ASSERT(!dirpath.Exists());
    RUNNER_ASSERT(!filepath.Exists());
    RUNNER_ASSERT(path2.Exists());
    RUNNER_ASSERT(dirpath2.Exists());
    RUNNER_ASSERT(filepath2.Exists());

    Rename(path2, path);
    RUNNER_ASSERT(path.Exists());
    RUNNER_ASSERT(dirpath.Exists());
    RUNNER_ASSERT(filepath.Exists());
    RUNNER_ASSERT(!path2.Exists());
    RUNNER_ASSERT(!dirpath2.Exists());
    RUNNER_ASSERT(!filepath2.Exists());

    Remove(path);
}

/**
 * Name: path_basename
 * Description: check basename equivalents
 * Expected: failure
 */
RUNNER_TEST(path_basename)
{
    DPL::ScopedDir sd(rootTest);
    Path path = Path(rootTest) / "directory" / "touch.txt";
    RUNNER_ASSERT(path.DirectoryName() == path.DirectoryPath().Fullpath());
    RUNNER_ASSERT(path.DirectoryPath().DirectoryName() == path.DirectoryPath().DirectoryPath().Fullpath());
}

/**
 * Name: path_safe
 * Description: check if operations cannot be executed on root directory
 *
 * This is check because of default path is root and it should not be used usually
 * Default constructor is unfortnatelly easier to use
 *
 * Expected: failure
 */
RUNNER_TEST(path_safe)
{
    DPL::ScopedDir sd(rootTest);
    Path normal = Path(rootTest) / "directory" / "touch.txt";
    Path root("/");
    ROOTGUARD_TESTMETHOD( Rename(normal, root) );
    ROOTGUARD_TESTMETHOD( Rename(root, root) );
    ROOTGUARD_TESTMETHOD( Rename(root, normal) );
    ROOTGUARD_TESTMETHOD( CopyDir(normal, root) );
    ROOTGUARD_TESTMETHOD( CopyDir(root, root) );
    ROOTGUARD_TESTMETHOD( CopyDir(root, normal) );
    ROOTGUARD_TESTMETHOD( CopyFile(normal, root) );
    ROOTGUARD_TESTMETHOD( CopyFile(root, root) );
    ROOTGUARD_TESTMETHOD( CopyFile(root, normal) );
    ROOTGUARD_TESTMETHOD( Remove(root) );
    ROOTGUARD_TESTMETHOD( MakeEmptyFile(root) );
    ROOTGUARD_TESTMETHOD( MakeDir(root) );
}

/**
 * Name: path_size
 * Description: check testing size of file
 * Expected: correct size
 */
RUNNER_TEST(path_size)
{
    DPL::ScopedDir sd(rootTest);
    Path path = Path(rootTest) / "touch.txt";
    DPL::Utils::MakeEmptyFile(path);
    RUNNER_ASSERT(path.Size() == 0);

    {
        DPL::FileOutput out(path.Fullpath());
        DPL::BinaryQueue bq;
        bq.AppendCopy("123456789", 9);
        out.Write(bq, bq.Size());
        out.Close();
        RUNNER_ASSERT(path.Size() == 9);
    }

    {
        DPL::FileOutput out(path.Fullpath());
        DPL::BinaryQueue bq;
        bq.AppendCopy("123456789", 4);
        out.Write(bq, bq.Size());
        out.Close();
        RUNNER_ASSERT(path.Size() == 4);
    }
}

/**
Name: path_copy
Description: tests path coping directory andfiles
Expected: coping should be done
*/
RUNNER_TEST(path_copy_directory)
{
    DPL::ScopedDir sd(rootTest);

    Path path = Path(rootTest) / "sourceDir";
    Path innerPath = Path(rootTest) / "sourceDir" / "level1" ;
    Path innerPath2 = Path(rootTest) / "sourceDir" / "level1" / "level2";
    Path file1 = Path(rootTest) / "sourceDir" / "level1" / "level2" / "file1.txt";
    Path file2 = Path(rootTest) / "sourceDir" / "level1" / "level2" / "file2.txt";
    Path file3 = Path(rootTest) / "sourceDir" / "level1" / "file3.txt";
    Path file4 = Path(rootTest) / "sourceDir" /  "file4.txt";

    Path tfile1 = Path(rootTest) / "targetDir" / "level1" / "level2" / "file1.txt";
    Path tfile2 = Path(rootTest) / "targetDir" / "level1" / "level2" / "file2.txt";
    Path tfile3 = Path(rootTest) / "targetDir" / "level1" / "file3.txt";
    Path tfile4 = Path(rootTest) / "targetDir" /  "file4.txt";

    Path target = Path(rootTest) / "targetDir";

    DPL::Utils::MakeDir(path);
    DPL::Utils::MakeDir(innerPath);
    DPL::Utils::MakeDir(innerPath2);
    DPL::Utils::MakeEmptyFile(file1);
    DPL::Utils::MakeEmptyFile(file2);
    DPL::Utils::MakeEmptyFile(file3);
    DPL::Utils::MakeEmptyFile(file4);

    DPL::Utils::CopyDir(path, target);

    RUNNER_ASSERT_MSG(tfile1.Exists(), tfile1.Fullpath() + " not exists");
    RUNNER_ASSERT_MSG(tfile1.IsFile(), tfile1.Fullpath() + " is not file");
    RUNNER_ASSERT_MSG(tfile2.Exists(), tfile2.Fullpath() + " not exists");
    RUNNER_ASSERT_MSG(tfile2.IsFile(), tfile2.Fullpath() + " is not file");
    RUNNER_ASSERT_MSG(tfile3.Exists(), tfile3.Fullpath() + " not exists");
    RUNNER_ASSERT_MSG(tfile3.IsFile(), tfile3.Fullpath() + " is not file");
    RUNNER_ASSERT_MSG(tfile4.Exists(), tfile4.Fullpath() + " not exists");
    RUNNER_ASSERT_MSG(tfile4.IsFile(), tfile4.Fullpath() + " is not file");
}

/*
Name: path_copy_inner
Description: tests path coping to subdirectory
Expected: coping shoudl fail
*/
RUNNER_TEST(path_copy_inner)
{
    DPL::ScopedDir sd(rootTest);

    Path path = Path(rootTest) / "touchDir";
    Path inner = Path(rootTest) / "touchDir" / "innerDirectory";

    bool exceptionCatched = false;
    Try
    {
        DPL::Utils::CopyDir(path, inner);
    }
    Catch(DPL::Utils::Path::CannotCopy)
    {
        exceptionCatched = true;
    }
    RUNNER_ASSERT_MSG(exceptionCatched, "Copy should fail");
}

/*
Name: issubpath
Description: tests method compare if one path is subpath of another
Expected: correct results
*/
RUNNER_TEST(path_issubpath)
{
    Path path1 = Path(rootTest) / "touchDir/asd/sdf";
    Path path2 = Path(rootTest) / "touchDir/asd/sdf/123";
    Path path3 = Path(rootTest) / "touchDir/asd/sdno";
    Path path4 = Path("/");

    RUNNER_ASSERT(path1.isSubPath(path2));
    RUNNER_ASSERT(!path1.isSubPath(path3));
    RUNNER_ASSERT(!path1.isSubPath(path4));

    RUNNER_ASSERT(!path2.isSubPath(path1));
    RUNNER_ASSERT(!path2.isSubPath(path3));
    RUNNER_ASSERT(!path2.isSubPath(path4));

    RUNNER_ASSERT(path4.isSubPath(path1));
    RUNNER_ASSERT(path4.isSubPath(path2));
    RUNNER_ASSERT(path4.isSubPath(path3));
}

/*
Name: path_rename_same
Description: tests if renam does not brokens file if target location is equal to source location
Expected: path is avaliable
*/
RUNNER_TEST(path_rename_same)
{
    DPL::ScopedDir sd(rootTest);

    struct stat st;
    memset(&st, 0, sizeof(struct stat));
    Path path = Path(rootTest) / "touchDir";

    MakeDir(path);
    Rename(path, path);
    RUNNER_ASSERT(path.Exists());
}

/*
Name: path_iterate_not_directory
Description: iterates not a directory
Expected: success full constrution
*/
RUNNER_TEST(path_iterate_not_directory)
{
    DPL::ScopedDir sd(rootTest);

    Path fileTest = Path(rootTest) / "file.txt";
    MakeEmptyFile(fileTest);

    bool passed = false;
    Try
    {
        FOREACH(file, fileTest)
        {

        }
    }
    Catch(Path::NotDirectory)
    {
        passed = true;
    }
    RUNNER_ASSERT(passed);
}

/*
Name: path_construction
Description: constructs paths in many ways
Expected: success full constrution
*/
RUNNER_TEST(path_iterate_empty_directory)
{
    DPL::ScopedDir sd(rootTest);

    Path dirTest = Path(rootTest) / "directory";
    MakeDir(dirTest);

    bool passed = true;
    Try
    {
        FOREACH(file, dirTest)
        {
            passed = false;
            LogError("Directory should be empty");
        }
    }
    Catch(Path::NotDirectory)
    {
        passed = false;
        LogError("Directory should exists");
    }
    RUNNER_ASSERT(passed);
}

/*
Name: path_construction
Description: constructs paths in many ways
Expected: success full constrution
*/
RUNNER_TEST(path_iterate_notempty_directory)
{
    DPL::ScopedDir sd(rootTest);

    Path dirTest = Path(rootTest) / "directory";

    Path path1 = Path(rootTest) / "directory" / "file1";
    Path path2 = Path(rootTest) / "directory" / "file2";
    Path path3 = Path(rootTest) / "directory" / "file3";

    std::set<std::string> resultSet;
    std::set<std::string> testSet;
    testSet.insert(path1.Fullpath());
    testSet.insert(path2.Fullpath());
    testSet.insert(path3.Fullpath());

    MakeDir(dirTest);
    MakeEmptyFile(path1);
    MakeEmptyFile(path2);
    MakeEmptyFile(path3);

    FOREACH(file, dirTest)
    {
        resultSet.insert(file->Fullpath());
    }

    RUNNER_ASSERT_MSG(testSet == resultSet, "Testing");
}

/*
Name: path_construction
Description: constructs paths in many ways
Expected: success full constrution
*/
RUNNER_TEST(path_iterator_copy_constructor)
{
    DPL::ScopedDir sd(rootTest);

    Path dirTest = Path(rootTest) / "directory";

    Path path1 = Path(rootTest) / "directory" / "file1";
    Path path2 = Path(rootTest) / "directory" / "file2";
    Path path3 = Path(rootTest) / "directory" / "file3";

    MakeDir(dirTest);
    MakeEmptyFile(path1);
    MakeEmptyFile(path2);
    MakeEmptyFile(path3);

    std::shared_ptr<Path::Iterator> iter1(new Path::Iterator((Path(rootTest) / "directory").Fullpath().c_str()));

    //as it's input iterator it's guaranteed for one element to be iterate only once
    (*iter1)++;
    std::shared_ptr<Path::Iterator> iter2(new Path::Iterator( (*iter1)));
    iter1.reset();
    (*iter2)++;
    ++(*iter2);
    RUNNER_ASSERT_MSG(*iter2 == dirTest.end(), "Iterator is in broken state");
    iter2.reset();
}