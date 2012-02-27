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
 * @file    TestSuite07.cpp
 * @author  unknown
 * @version 1.0
 * @brief   Test cases for ConfigurationManager
 */


#include <dirent.h>
#include <sys/types.h>

#include <list>
#include <string>

#include <dpl/log/log.h>
#include <dpl/test/test_runner.h>

#include <dpl/ace/ConfigurationManager.h>

#define TESTSUITE07(n,path)                  \
RUNNER_TEST(ts07_configuration_mng_ ## n){   \
    ConfigurationManagerTester cm;           \
    cm.setUp();                              \
    cm.parseTest(path);

#define TESTSUITEEND                         \
    cm.cleanUp();                            \
}

class ConfigurationManagerTester : public ConfigurationManager {
public:
    int parseTest(const std::string &file) {
        return parse(file);
    }

    const std::string& getConfigFileTest() const {
        return getConfigFile();
    }

    void prepareDir(std::string & path);
    void cleanUp();
    void setUp();
    bool assertPolicyNOTinAList(const char *) const;
    bool assertIsCurrentPolicy(const char * policy);
    bool assertEqual(const std::string& original, const std::string & intended);
    bool assertFileExists(const char * policyFile) const;
    bool assertFileDoesntExist(const char * policyFile) const;
    bool assertEqual(const int original,const int intended) const;
    bool assertPolicyInAList(const char *)const;

    ConfigurationManagerTester() { }
};

TESTSUITE07(01,CONFIGURATION_MGR_TEST_CONFIG)
    RUNNER_ASSERT(cm.assertIsCurrentPolicy("pms_general-test.xml") && cm.assertPolicyInAList("pms_general-test.xml"));
TESTSUITEEND

TESTSUITE07(02,CONFIGURATION_MGR_TEST_CONFIG)
    RUNNER_ASSERT(cm.assertEqual(cm.getStoragePath(),
        std::string(CONFIGURATION_MGR_TEST_POLICY_STORAGE)));
TESTSUITEEND

TESTSUITE07(03,CONFIGURATION_MGR_TEST_CONFIG)
    LogInfo("Full path "<<cm.getFullPathToCurrentPolicyFile());
    RUNNER_ASSERT(cm.assertEqual(cm.getFullPathToCurrentPolicyFile(),
        std::string(CONFIGURATION_MGR_TEST_POLICY)));
TESTSUITEEND

TESTSUITE07(04,CONFIGURATION_MGR_TEST_CONFIG)
    int ret = cm.addPolicyFile(std::string(CONFIGURATION_MGR_TEST_PATH"policyTest1.xml"));
    RUNNER_ASSERT(cm.assertFileExists(CONFIGURATION_MGR_TEST_POLICY_STORAGE"/policyTest1.xml"));
    RUNNER_ASSERT(cm.assertEqual(ret, ConfigurationManager::CM_OPERATION_SUCCESS));
    RUNNER_ASSERT(cm.assertPolicyInAList("policyTest1.xml"));
TESTSUITEEND

TESTSUITE07(05,CONFIGURATION_MGR_TEST_CONFIG)
    int ret = cm.addPolicyFile(std::string(CONFIGURATION_MGR_TEST_PATH"policyTest1.xml"));
    RUNNER_ASSERT(cm.assertFileExists(CONFIGURATION_MGR_TEST_POLICY_STORAGE"/policyTest1.xml"));
    RUNNER_ASSERT(cm.assertEqual(ret, ConfigurationManager::CM_OPERATION_SUCCESS));
    ret = cm.removePolicyFile(std::string("policyTest1.xml"));
    RUNNER_ASSERT(cm.assertFileDoesntExist(CONFIGURATION_MGR_TEST_POLICY_STORAGE"/policyTest1.xml"));
    RUNNER_ASSERT(cm.assertEqual(ret, ConfigurationManager::CM_OPERATION_SUCCESS));
    RUNNER_ASSERT(cm.assertPolicyNOTinAList("policyTest1.xml"));
TESTSUITEEND

//Try to upload the file once again
TESTSUITE07(06,CONFIGURATION_MGR_TEST_CONFIG)
    int ret = cm.addPolicyFile(std::string(CONFIGURATION_MGR_TEST_PATH"policyTest1.xml"));
    RUNNER_ASSERT(cm.assertFileExists(CONFIGURATION_MGR_TEST_POLICY_STORAGE"/policyTest1.xml"));
    RUNNER_ASSERT(cm.assertEqual(ret, ConfigurationManager::CM_OPERATION_SUCCESS));
    RUNNER_ASSERT(cm.assertPolicyInAList("policyTest1.xml"));
TESTSUITEEND

TESTSUITE07(07,CONFIGURATION_MGR_TEST_CONFIG)
    int ret = cm.addPolicyFile(std::string(CONFIGURATION_MGR_TEST_PATH"policyTest2.xml"));
    RUNNER_ASSERT(cm.assertFileExists(CONFIGURATION_MGR_TEST_POLICY_STORAGE"/policyTest2.xml"));
    RUNNER_ASSERT(cm.assertEqual(ret, ConfigurationManager::CM_OPERATION_SUCCESS));
    RUNNER_ASSERT(cm.assertPolicyInAList("policyTest2.xml"));
TESTSUITEEND

TESTSUITE07(08,CONFIGURATION_MGR_TEST_CONFIG)
    int ret = cm.addPolicyFile(std::string(CONFIGURATION_MGR_TEST_PATH"notExitingPolicy.xml"));
    RUNNER_ASSERT(cm.assertFileDoesntExist(CONFIGURATION_MGR_TEST_POLICY_STORAGE"/notExitingPolicy.xml"));
    RUNNER_ASSERT(cm.assertEqual(ret, ConfigurationManager::CM_GENERAL_ERROR));
    RUNNER_ASSERT(cm.assertPolicyNOTinAList("notExitingPolicy.xml"));
TESTSUITEEND

TESTSUITE07(09,CONFIGURATION_MGR_TEST_CONFIG)
    int ret = cm.addPolicyFile(CONFIGURATION_MGR_TEST_PATH"policyTest1.xml");
    RUNNER_ASSERT(cm.assertFileExists(CONFIGURATION_MGR_TEST_POLICY_STORAGE"/policyTest1.xml"));
    RUNNER_ASSERT(cm.assertPolicyInAList("policyTest1.xml"));
    ret = cm.addPolicyFile(CONFIGURATION_MGR_TEST_PATH"policyTest1.xml");
    RUNNER_ASSERT(cm.assertEqual(ret, ConfigurationManager::CM_FILE_EXISTS));
TESTSUITEEND

TESTSUITE07(10,CONFIGURATION_MGR_TEST_CONFIG)
    int ret = cm.removePolicyFile("policyTest12.xml");
    RUNNER_ASSERT(cm.assertFileDoesntExist(CONFIGURATION_MGR_TEST_POLICY_STORAGE"/policyTest12.xml"));
    RUNNER_ASSERT(cm.assertEqual(ret, ConfigurationManager::CM_REMOVE_NOT_EXISTING));
    RUNNER_ASSERT(cm.assertPolicyNOTinAList("policyTest12.xml"));
TESTSUITEEND

TESTSUITE07(11,CONFIGURATION_MGR_TEST_CONFIG)
    int ret = cm.removePolicyFile(cm.getCurrentPolicyFile());
    RUNNER_ASSERT(cm.assertFileExists(cm.getFullPathToCurrentPolicyFile().c_str()));
    RUNNER_ASSERT(cm.assertEqual(ret, ConfigurationManager::CM_REMOVE_CURRENT));
    RUNNER_ASSERT(cm.assertPolicyInAList(cm.getCurrentPolicyFile().c_str()));
TESTSUITEEND

TESTSUITE07(12,CONFIGURATION_MGR_TEST_CONFIG)
    std::string fullPathToCurrentPolicy(CONFIGURATION_MGR_TEST_PATH);
    fullPathToCurrentPolicy+="active/";
    fullPathToCurrentPolicy+= cm.getCurrentPolicyFile();
    RUNNER_ASSERT(cm.assertEqual(cm.getFullPathToCurrentPolicyFile(), fullPathToCurrentPolicy));
TESTSUITEEND

TESTSUITE07(13,CONFIGURATION_MGR_TEST_CONFIG)
    system("mv "CONFIGURATION_MGR_TEST_POLICY_STORAGE" "CONFIGURATION_MGR_TEST_POLICY_STORAGE_MOVED);
    int ret = cm.addPolicyFile(CONFIGURATION_MGR_TEST_PATH"policyTest3.xml");
    bool result = cm.assertEqual(ret, ConfigurationManager::CM_GENERAL_ERROR)
                  && cm.assertPolicyNOTinAList("policyTest3.xml");
    system("mv "CONFIGURATION_MGR_TEST_POLICY_STORAGE_MOVED" "CONFIGURATION_MGR_TEST_POLICY_STORAGE);
    RUNNER_ASSERT(result);
TESTSUITEEND

TESTSUITE07(14,CONFIGURATION_MGR_TEST_CONFIG)
    cm.addPolicyFile(CONFIGURATION_MGR_TEST_PATH"policyTest3.xml");

    system("mv "CONFIGURATION_MGR_TEST_POLICY_STORAGE" "CONFIGURATION_MGR_TEST_POLICY_STORAGE_MOVED);
    int ret = cm.removePolicyFile("policyTest3.xml");
    bool result = cm.assertEqual(ret, ConfigurationManager::CM_REMOVE_NOT_EXISTING)
                  && cm.assertPolicyNOTinAList("policyTest3.xml");
    system("mv "CONFIGURATION_MGR_TEST_POLICY_STORAGE_MOVED" "CONFIGURATION_MGR_TEST_POLICY_STORAGE);
    RUNNER_ASSERT(result);
TESTSUITEEND

/*
 *
 * Changing configuration
 *
 */
TESTSUITE07(15,CONFIGURATION_MGR_TEST_CONFIG)
    //Just make sure that policyTest1.xml has been added to the storage
    cm.removePolicyFile("policyTest1.xml");
    int ret = cm.addPolicyFile(CONFIGURATION_MGR_TEST_PATH"policyTest1.xml");
    bool result = cm.assertEqual(ret,ConfigurationManager::CM_OPERATION_SUCCESS);

    ret = cm.changeCurrentPolicyFile("policyTest1.xml");
    result &= cm.assertEqual(ret, ConfigurationManager::CM_OPERATION_SUCCESS);
    RUNNER_ASSERT(result);

    cm.parseTest(cm.getConfigFileTest());
    result = cm.assertEqual(cm.getCurrentPolicyFile(),"policyTest1.xml");
    RUNNER_ASSERT(result);

    ret = cm.changeCurrentPolicyFile("pms_general-test.xml");
    result = cm.assertEqual(ret, ConfigurationManager::CM_OPERATION_SUCCESS);
    RUNNER_ASSERT(result);

    cm.parseTest(cm.getConfigFileTest());
    result = cm.assertEqual(cm.getCurrentPolicyFile(),"pms_general-test.xml");
    RUNNER_ASSERT(result);
TESTSUITEEND

/*
 *===============
 *
 *END OF TESTS
 *
 *===============
 */

bool ConfigurationManagerTester::assertIsCurrentPolicy(const char * policy){

    return getCurrentPolicyFile() == std::string(policy);

}

bool ConfigurationManagerTester::assertEqual(const std::string & actual,const std::string & intended){
    LogDebug("Comparing "<<actual<<" with "<<intended);
    return actual == intended;
}

bool ConfigurationManagerTester::assertEqual(const int original,const int intended) const{

    bool tmp = original == intended;
    LogInfo("Results comparing with outcome "<<tmp);
    return tmp;
}

bool ConfigurationManagerTester::assertFileExists(const char * policyFile) const {
    bool tmp = checkIfFileExistst(std::string(policyFile));
    LogDebug("Assert File exists with result: "<<tmp);
    return tmp;
}

bool ConfigurationManagerTester::assertFileDoesntExist(const char * policyFile) const {

    bool temp = !checkIfFileExistst(std::string(policyFile));
    LogDebug("Assert file doesn't exists result: "<<temp);
    return temp;

}

bool ConfigurationManagerTester::assertPolicyInAList(const char * name)const{
    bool found = false;
    std::string expected(name);
    for(std::list<std::string>::const_iterator it = getPolicyFiles().begin(); it!=getPolicyFiles().end(); ++it){
        if( *it == expected ){
            found = true;
            break;
        }
    }

    LogDebug("assert Policy in a list result: "<<found);
    return found;
}


bool ConfigurationManagerTester::assertPolicyNOTinAList(const char * name) const{
    return !assertPolicyInAList(name);
}

void ConfigurationManagerTester::cleanUp(){
    DIR *dp;
    struct dirent *dirp;
    if( (dp = opendir(CONFIGURATION_MGR_TEST_POLICY_STORAGE) )== NULL ){
        LogError("Error while cleaning up");
        return;
    }

    while( (dirp = readdir(dp)) != NULL ) {
        std::string fileName(dirp->d_name);
        if( fileName == "pms_general-test.xml" || fileName == "bondixml.dtd" ||
            fileName == "." || fileName ==".." ){
            continue;
        }
        std::string fullPath(CONFIGURATION_MGR_TEST_POLICY_STORAGE"/");
        fullPath.append(dirp->d_name);
        if (removePolicyFile(fullPath) == CM_REMOVE_ERROR){
            LogError("Cannot clean up. Exiting");
            break;
        };
    }
    //Restore the backup of config file
     system("cp "CONFIGURATION_MGR_TEST_PATH"backup.xml "CONFIGURATION_MGR_TEST_CONFIG);
}

void ConfigurationManagerTester::setUp(){
    //Create backup of config file
    system("cp "CONFIGURATION_MGR_TEST_CONFIG" "CONFIGURATION_MGR_TEST_PATH"backup.xml");
}
