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
#include <stddef.h>
#include <set>
#include <dpl/assert.h>
#include <vector>

#include <map>

#include <dpl/utils/mime_type_utils.h>

const std::set<DPL::String>& MimeTypeUtils::getMimeTypesSupportedForIcon()
{
    static std::set<DPL::String> set;
    DPL::String (*s)(const std::string&) = DPL::FromASCIIString;
    if (set.empty()) {
        set.insert(s("image/gif"));
        set.insert(s("image/png"));
        set.insert(s("image/vnd.microsoft.icon"));
        set.insert(s("image/svg+xml"));
        set.insert(s("image/jpeg"));
    }
    return set;
}

const std::set<DPL::String>& MimeTypeUtils::getMimeTypesSupportedForStartFile()
{
    static std::set<DPL::String> set;
    DPL::String (*s)(const std::string&) = DPL::FromASCIIString;
    if (set.empty()) {
        set.insert(s("text/html"));
        set.insert(s("application/xhtml+xml"));
    }
    return set;
}

bool MimeTypeUtils::isMimeTypeSupportedForStartFile(const DPL::String& mimeType)
{
    return getMimeTypesSupportedForStartFile().count(stripMimeParameters(
                                                         mimeType)) > 0;
}

const MimeTypeUtils::FileIdentificationMap& MimeTypeUtils::
    getFileIdentificationMap()
{
    static FileIdentificationMap map;
    DPL::String (*s)(const std::string&) = DPL::FromASCIIString;
    if (map.empty()) {
        map[s(".html")] = s("text/html");
        map[s(".htm")] = s("text/html");
        map[s(".css")] = s("text/css");
        map[s(".js")] = s("application/javascript");
        map[s(".xml")] = s("application/xml");
        map[s(".txt")] = s("text/plain");
        map[s(".wav")] = s("audio/x-wav");
        map[s(".xhtml")] = s("application/xhtml+xml");
        map[s(".xht")] = s("application/xhtml+xml");
        map[s(".gif")] = s("image/gif");
        map[s(".png")] = s("image/png");
        map[s(".ico")] = s("image/vnd.microsoft.icon");
        map[s(".svg")] = s("image/svg+xml");
        map[s(".jpg")] = s("image/jpeg");
    }
    return map;
}

bool MimeTypeUtils::isMimeTypeSupportedForIcon(const DPL::String& mimeType)
{
    return getMimeTypesSupportedForIcon().count(stripMimeParameters(mimeType))
           > 0;
}

DPL::String MimeTypeUtils::stripMimeParameters(const DPL::String& mimeType)
{
    size_t parametersStart = mimeType.find_first_of(L';');
    if (parametersStart != DPL::String::npos) {
        return mimeType.substr(0, parametersStart);
    } else {
        return mimeType;
    }
}

MimeTypeUtils::MimeAttributes MimeTypeUtils::getMimeAttributes(
        const DPL::String& mimeType)
{
    MimeAttributes attributes;
    std::vector<DPL::String> tokens;
    DPL::Tokenize(mimeType, L";=", std::back_inserter(tokens));
    for (unsigned int i = 1; i < tokens.size(); i += 2) {
        attributes[tokens[i]] = tokens[i + 1];
    }
    return attributes;
}

bool MimeTypeUtils::isValidIcon(const DPL::String& path)
{
    return getMimeTypesSupportedForIcon().count(identifyFileMimeType(path)) > 0;
}

bool MimeTypeUtils::isValidStartFile(const DPL::String& path,
        const DPL::OptionalString& providedMimeType)
{
    DPL::String mimeType = (!!providedMimeType) ? stripMimeParameters(
            *providedMimeType) : identifyFileMimeType(path);
    return getMimeTypesSupportedForStartFile().count(mimeType) > 0;
}

DPL::String MimeTypeUtils::getFileNameFromPath(const DPL::String& path)
{
    size_t lastSlashPos = path.find_last_of(L'/');
    return path.substr(lastSlashPos + 1);
}

DPL::String MimeTypeUtils::identifyFileMimeType(const DPL::String& path)
{
    DPL::String name = getFileNameFromPath(path); //step 4

    if (name.size() == 0) {
        ThrowMsg(Exception::InvalidFileName, "Path should contain a file name.");
    }

    size_t lastFullStop = name.find_last_of(L'.');
    if (lastFullStop != 0 && lastFullStop != DPL::String::npos) { //step 5
        DPL::String extension = name.substr(lastFullStop); //step 6 & 7
        if (extension.size() > 0) { //step 8
            //step 9
            std::transform(extension.begin(), extension.end(),
                           extension.begin(), ::towlower);
            FileIdentificationMap::const_iterator it =
                getFileIdentificationMap().find(extension);
            if (it != getFileIdentificationMap().end()) {
                return it->second;
            }
        }
    }
    //TODO step 10 - sniff
    return DPL::FromASCIIString("application/sniff");
}
