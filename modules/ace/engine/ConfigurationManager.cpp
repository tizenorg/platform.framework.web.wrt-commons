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
#include <dpl/assert.h>
#include <fcntl.h>
#include <errno.h>
#include <error.h>
#include <malloc.h>
#include <sys/stat.h>
#include <dpl/ace/ConfigurationManager.h>

using namespace std;

namespace {
const string currentXMLSchema("bondixml.xsd");
}

ConfigurationManager * ConfigurationManager::instance = NULL;

string ConfigurationManager::getCurrentPolicyFile(void) const
{
    return currentPolicyFile;
}

string ConfigurationManager::getFullPathToCurrentPolicyFile(void) const
{
    if (*(storagePath.rbegin()) == '/') {
        return storagePath + currentPolicyFile;
    }
    return storagePath + "/" + currentPolicyFile;
}

string ConfigurationManager::getStoragePath(void) const
{
    return storagePath;
}

string ConfigurationManager::getFullPathToCurrentPolicyXMLSchema() const
{
    if (*(storagePath.rbegin()) == '/')
    {
        return storagePath + currentXMLSchema;
    }
    return storagePath + "/" + currentXMLSchema;
}

int ConfigurationManager::parse(const string &configFileName)
{
    configFile = configFileName;
    int error = PARSER_SUCCESS;
    policyFiles.clear();

    reader = xmlNewTextReaderFilename(configFile.c_str());

    if (reader == NULL) {
        LogError("Parser does not exist");
        error = PARSER_ERROR;
        goto cleanUp;
    }

    if (xmlTextReaderSetParserProp(reader, XML_PARSER_VALIDATE, 1)) {
        LogError("Error while setting parser validating.");
        error = PARSER_ERROR;
        Assert(false && "Cannot make XML parser validating");
        goto cleanUp;
    }

    if (xmlTextReaderSetParserProp(reader, XML_PARSER_DEFAULTATTRS, 1)) {
        LogError("Error while attribute defaulting.");
        error = PARSER_ERROR;
        Assert(false && "XML Parser cannot default xml attributes");
        goto cleanUp;
    }

    int ret;
    while (1 == (ret = xmlTextReaderRead(reader))) {
        processNode();
        if (xmlTextReaderIsValid(reader) == 0) {
            LogError(
                "Parser error while reading file " << configFile <<
                ". File is not valid.");
            error = PARSER_ERROR;
            Assert(false && "Configuration Manager error");
            goto cleanUp;
        }
    }
    if (ret != 0) {
        LogError("There were some errors while parsing XML file.");
    }

cleanUp:
    if (reader != NULL) {
        xmlFreeTextReader(reader);
    }
    return error;
}

void ConfigurationManager::extractFileAttributes()
{
    xmlChar *active = xmlTextReaderGetAttribute(reader, ATTR_ACTIVE_POLICY);
    xmlActive = false;

    if (active && active[0] == 't') {
        xmlActive = true;
    }

    xmlFree(active);
}

void ConfigurationManager::startNodeHandler(void)
{
    xmlChar *name = xmlTextReaderName(reader);
    switch (*name) {
    case 'f':     // file
        extractFileAttributes();
        break;
    default:
        break;
    }
    xmlFree(name);
}

void ConfigurationManager::endNodeHandler(void)
{
    xmlChar *name = xmlTextReaderName(reader);
    switch (*name) {
    case 'f':
        policyFiles.push_back(currentText);
        if (xmlActive) {
            currentPolicyFile = currentText;
            xmlActive = false;
        }
        currentText.clear();
        break;
    case 's':
        storagePath = currentText;
        currentText.clear();
        break;
    default:
        break;
    }
    xmlFree(name);
}

void ConfigurationManager::textNodeHandler(void)
{
    xmlChar *text = xmlTextReaderValue(reader);
    currentText = reinterpret_cast<const char*>(text);
    xmlFree(text);
}

void ConfigurationManager::processNode(void)
{
    xmlReaderTypes type =
        static_cast<xmlReaderTypes>(xmlTextReaderNodeType(reader));
    switch (type) {
    case XML_READER_TYPE_ELEMENT: startNodeHandler();
        break;
    case XML_READER_TYPE_END_ELEMENT: endNodeHandler();
        break;
    case XML_READER_TYPE_TEXT: textNodeHandler();
        break;
    default:
        break;
    }
}

int ConfigurationManager::addPolicyFile(const string & sourcePath)
{
    FILE * sourceFd = NULL, *destFd = NULL;
    int flag = CM_OPERATION_SUCCESS;
    int sourceLength;
    string fileName;

    string newFilePath(getStoragePath());
    newFilePath.append("/");

    if (sourcePath.empty()) {
        LogError("Filename empty");
        flag = CM_GENERAL_ERROR;
        goto cleanup;
    }

    fileName = extractFilename(sourcePath);
    newFilePath.append(fileName);

    LogDebug("Adding new file " << newFilePath);
    if (checkIfFileExistst(newFilePath)) {
        LogError("Destination file already exists");
        flag = CM_FILE_EXISTS;
        goto cleanup;
    }

    sourceFd = fopen(sourcePath.c_str(), "r");
    if (sourceFd == NULL) {
        LogError("Source file opening failed");
        flag = CM_GENERAL_ERROR;
        goto cleanup;
    }

    destFd = fopen(newFilePath.c_str(), "w");
    if (destFd == NULL) {
        LogError("Destination file creation failed");
        flag = CM_GENERAL_ERROR;
        goto cleanup;
    }

    if (0 > (sourceLength = getFileSize(sourcePath))) {
        LogError("getFileSize error");
        flag = CM_GENERAL_ERROR;
        goto cleanup;
    }

    if (!copyFile(sourceFd, destFd, sourceLength)) {
        //Copying failed, we have to remove corrupted file
        flag = CM_GENERAL_ERROR;
        if (removePolicyFile(newFilePath) != CM_OPERATION_SUCCESS) {
            flag = CM_REMOVE_ERROR;
        }
        goto cleanup;
    }
    policyFiles.push_back(fileName);

    if (saveConfig() != CM_OPERATION_SUCCESS) {
        LogError("Error while saving policy file");
        //TODO HOW TO ROLLBACK save config?
    }

cleanup:

    if (sourceFd) {
        fclose(sourceFd);
    }
    if (destFd) {
        fclose(destFd);
    }

    return flag;
}

int ConfigurationManager::removePolicyFile(const string& file)
{
    LogDebug("Trying to remove policy " << file);
    int errorFlag = CM_OPERATION_SUCCESS;
    string fileName = extractFilename(file);
    string filePath(CONFIGURATION_MGR_TEST_POLICY_STORAGE);

    if (fileName == currentPolicyFile) {
        errorFlag = CM_REMOVE_CURRENT;
        LogError("Cannot remove current policy");
        goto end;
    }

    filePath.append("/").append(fileName);

    if (remove(filePath.c_str()) != 0) {
        if (checkIfFileExistst(filePath)) {
            errorFlag = CM_REMOVE_ERROR;
            LogError("Cannot delete file" << filePath);
            goto end;
        }
        LogError("Cannot delete file" << filePath << " the file doesn't exist");
        errorFlag = CM_REMOVE_NOT_EXISTING;
    } else {
        errorFlag = CM_OPERATION_SUCCESS;
    }
    //If remove was successful or unsuccessful but file doesn't exists then it
    //should be removed from the list of available policies
    policyFiles.remove(fileName);

end:
    return errorFlag;
}

string ConfigurationManager::extractFilename(const string & sourcePath) const
{
    //begining of filename without path
    size_t filenamePos = sourcePath.rfind('/');

    string tmp;
    if (filenamePos == string::npos) {
        tmp = sourcePath;
    } else {
        tmp = sourcePath.substr(filenamePos + 1);
    }
    LogDebug("Extracted filename " << tmp);
    return tmp;
}

int ConfigurationManager::getFileSize(const string & path) const
{
    //get source file size
    struct stat sourceStat;
    int sourceLength;
    if (stat(path.c_str(), &sourceStat) == -1) {
        LogError("Reading file properties failed");
        sourceLength = -1;
    } else {
        sourceLength = sourceStat.st_size;
    }
    return sourceLength;
}

bool ConfigurationManager::copyFile(FILE * sourceDesc,
        FILE * destinationDesc,
        int lenght) const
{
    int rv;
    char *buffer = static_cast<char *>(malloc(lenght));
    bool result = true;

    if (!buffer) {
        return false;
    }

    while ((rv = fread(buffer, sizeof (char), lenght, sourceDesc)) > 0) {
        if (rv != lenght) {
            if (!feof(sourceDesc)) {
                LogError("Error while reading file ");
                result = false;
                break;
            }
        }

        rv = fwrite(buffer, sizeof (char), lenght, destinationDesc);
        if (rv != lenght) {
            LogError("Write file failed ");
            result = false;
            break;
        }
    }
    free(buffer);
    return result;
}

bool ConfigurationManager::checkIfFileExistst(const string & newFilePath) const
{
    bool exists = false;
    struct stat stats;
    if (stat(newFilePath.c_str(), &stats) == 0) {
        exists = true;
    }
    return exists;
}

int ConfigurationManager::changeCurrentPolicyFile(const std::string & fileName)
{
    int result = CM_OPERATION_SUCCESS;
    string oldPolicyFile = currentPolicyFile;

    string filePath(getStoragePath() + "/");
    filePath += fileName;

    if (!checkIfFileExistst(filePath)) {
        //Policy file doesn't exist
        result = CM_GENERAL_ERROR;
        LogError("Error: policy file " << filePath << " doesn't exist");
        goto end;
    }
    if (result == CM_OPERATION_SUCCESS) {
        //Adding file to storage succeeded
        currentPolicyFile = extractFilename(filePath);
        //Try to save the configuration file
        result = saveConfig();
    }
    if (result != CM_OPERATION_SUCCESS) {
        //rollback changes
        currentPolicyFile = oldPolicyFile;
        LogError("Error while saving policy file");
        //TODO HOW TO ROLLBACK save config?
    }

end:
    return result;
}

int ConfigurationManager::saveConfig()
{
    std::ofstream output(configFile.c_str());
    int errorFlag = CM_OPERATION_SUCCESS;

    output << "<?xml version=\"1.0\" ?>" << std::endl;
    output <<
    "<!DOCTYPE manager-settings SYSTEM \"" ACE_CONFIGURATION_DTD "\">" <<
    std::endl;
    output << "<manager-settings>" << std::endl;
    output << "  <storage-path>" << storagePath << "</storage-path>" <<
    std::endl;
    output << "  <policy-files>" << std::endl;
    for (list<string>::const_iterator it = policyFiles.begin();
         it != policyFiles.end();
         ++it) {
        output << "\t<file ";
        if (*it == currentPolicyFile) {
            output << "active=\"true\"";
        }
        output << ">" << *it << "</file>" << std::endl;
    }
    output << "  </policy-files>\n";
    output << " </manager-settings>";
    output.close();

    //TODO should we add 'eof' here?
    if (output.bad() || output.fail()) {
        //There were some errors while writing to file
        errorFlag = CM_GENERAL_ERROR;
    }

    return errorFlag;
}
