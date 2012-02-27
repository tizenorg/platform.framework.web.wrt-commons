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
#ifndef _CONFIGURATIONMANAGER_H_
#define _CONFIGURATIONMANAGER_H_

#include <list>
#include <string.h>
#include <string>
#include <libxml/xmlreader.h>
#include "Constants.h"
#include <fstream>
#include <iostream>
#include <dpl/log/log.h>

#define ATTR_ACTIVE_POLICY BAD_CAST("active")

#define PARSER_ERROR   1
#define PARSER_SUCCESS  0

class ConfigurationManager
{
  public:
    enum ConfigurationManagerResult
    {
        CM_OPERATION_SUCCESS = 0,
        CM_GENERAL_ERROR = -1,
        CM_FILE_EXISTS = -2,
        CM_REMOVE_ERROR = -3,
        CM_REMOVE_CURRENT = -4,
        CM_REMOVE_NOT_EXISTING = -5
    };

    /**
     * Current policy file getter
     * @return Name of the current ACE policy file
     */
    std::string getCurrentPolicyFile(void) const;

    /**
     * ACE policy file path getter
     * @return Full path to ACE current policy file
     */
    std::string getFullPathToCurrentPolicyFile(void) const;

    /**
     * ACE policy dtd file path getter
     * @return Full path to ACE current policy file
     */
    std::string getFullPathToCurrentPolicyXMLSchema(void) const;

    /**
     * ACE policy storage path getter
     * @return Full path to ACE policy file storage
     */
    std::string getStoragePath(void) const;

    /**
     * Adds file to ACE policy storage
     * @param filePath full path to policy to be added
     * @return CM_OPERATION_SUCCESS on success,
     *         CM_FILE_EXISTS if file with the same name already exists in the storage (the new file is not added),
     *         other error code on other error
     *
     */
    int addPolicyFile(const std::string & filePath);

    /**
     * Removes file with a given filename from ACE policy storage
     * @param fileName name of the policy to be removed
     * @return CM_OPERATION_SUCCESS on success,
     *         CM_REMOVE_CURRENT if file to be removed is a current file (it cannot be removed)
     *         CM_REMOVE_NOT_EXISTING if file already doesn't exists
     *         other error code on other error
     *
     */
    int removePolicyFile(const std::string& fileName);

    //change current PolicyFile
    /**
     * @param filePath Name of the policy file in the policy storage that should be made
     * a current policy file
     * @param CM_OPERATION_SUCCESS on success or error code on error
     */
    int changeCurrentPolicyFile(const std::string& filePath);

    //TODO this getInstance() method is a little bit to complicated
    /**
     * Method to obtain instance of configuration manager
     * @return retuns pointer to configuration manager or NULL in case of error
     */
    static ConfigurationManager * getInstance()
    {
        if (!instance) {
            instance = new ConfigurationManager();
            if (instance->parse(std::string(ACE_CONFIGURATION_PATH)) !=
                PARSER_SUCCESS) {
                delete instance;
                LogError(
                    "Couldn't parse configuration file " ACE_CONFIGURATION_PATH);
                return NULL;
            }
        }
        return instance;
    }

    /**
     * Extracts filename from full path
     * @param path Full path from which filename should be extracted
     * @return returns extracted filename
     */
    std::string extractFilename(const std::string& path) const;

  protected:

    /**
     * Parse given ACE configuration file
     * @param configFileName full path to configuration file to be parsed
     * @return PARSER_SUCCESS on succes, PARSER_ERROR on error
     */
    int parse(const std::string &configFileName);
    /**
     * @param path full path to the file which size should be obtained
     * @return size of a given file in bytes
     */
    int getFileSize(const std::string & path) const;
    bool copyFile(FILE * source,
            FILE * destination,
            int lenght = 1024) const;
    bool checkIfFileExistst(const std::string & newFilePath) const;

    const std::list<std::string> & getPolicyFiles() const
    {
        return policyFiles;
    }

    const std::string & getConfigFile() const
    {
        return configFile;
    }

    ConfigurationManager() : xmlActive(false)
    {
    }
    virtual ~ConfigurationManager()
    {
    }

  private:

    /**
     * Internal parser methods
     */
    void extractFileAttributes(void);
    void startNodeHandler(void);
    void endNodeHandler(void);
    void textNodeHandler(void);
    void processNode(void);
    //Save configuration file
    int saveConfig();

    //Private fields for parser state representation
    xmlTextReaderPtr reader;
    std::string currentText;
    bool xmlActive;

    static ConfigurationManager * instance;

    /**
     * Full path to ACE configuration file
     */
    std::string configFile;
    /**
     * ACE policy file storage path
     */
    std::string storagePath;
    /**
     * Name of the current ACE policy
     */
    std::string currentPolicyFile;
    /**
     * List of available ACE policy files
     */
    std::list<std::string> policyFiles;

    //////////NEW
};

#endif

