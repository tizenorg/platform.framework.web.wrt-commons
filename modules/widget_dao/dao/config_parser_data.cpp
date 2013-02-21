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
 * @file        config_parser_data.cpp
 * @author      Lukasz Wrzosek (l.wrzosek@samsung.com)
 * @version     0.1
 * @brief
 */
#include <stddef.h>
#include <dpl/wrt-dao-ro/config_parser_data.h>
#include <dpl/log/log.h>
#include <libxml/xmlreader.h>
#include <libxml/xmlstring.h>

namespace WrtDB {
bool IsSpace(const xmlChar* str);
bool CopyChar(xmlChar* out, xmlChar* in);

bool IsSpace(const xmlChar* str)
{
    int charlen = xmlUTF8Size(str);

    switch (charlen) {
    case 1:
        switch (*str) {
        case 0x09:
        case 0x0a:
        case 0x0b:
        case 0x0c:
        case 0x0d:
        case 0x20:
            return true;

        default:
            return false;
        }

    case 2:
        switch (*(str + 1)) {
        case 0x85:
        case 0xa0:
            return true;

        default:
            return false;
        }

    case 3:
        switch (*str) {
        case 0xe1:
        {
            unsigned char c2 = *(str + 1);
            unsigned char c3 = *(str + 2);
            if ((c2 == 0x9a && c3 == 0x80) || (c2 == 0xa0 && c3 == 0x8e)) {
                return true;
            } else {
                return false;
            }
        }

        case 0xe2:
            switch (*(str + 1)) {
            case 0x80:
                switch (*(str + 2)) {
                case 0x80:
                case 0x81:
                case 0x82:
                case 0x83:
                case 0x84:
                case 0x85:
                case 0x86:
                case 0x87:
                case 0x88:
                case 0x89:
                case 0x8a:
                case 0xa8:
                case 0xa9:
                case 0xaf:
                    return true;
                default:
                    return false;
                }
            case 0x81:
                if (*(str + 2) == 0x9f) {
                    return true;
                } else {
                    return false;
                }

            default:
                return false;
            }
        case 0xe3:
            if (*(str + 1) == 0x80 && *(str + 2) == 0x80) {
                return true;
            } else {
                return false;
            }

        default:
            return false;
        }

    default:
        return false;
    }
}

bool CopyChar(xmlChar* out,
              xmlChar* in)
{
    int size = xmlUTF8Size(in);
    switch (size) {
    case 6:
        out[5] = in[5];
    case 5:
        out[4] = in[4];
    case 4:
        out[3] = in[3];
    case 3:
        out[2] = in[2];
    case 2:
        out[1] = in[1];
    case 1:
        out[0] = in[0];
        return true;

    default:
        return false;
    }
}

//TODO temporary fix until commits the rewrite of this functionality.
void NormalizeString(DPL::String& str)
{
    DPL::Optional<DPL::String> opt = str;
    NormalizeString(opt);
    str = *opt;
}

void NormalizeString (DPL::Optional<DPL::String>& txt, bool isTrimSpace)
{
    if (!!txt) {
        std::string tmp = DPL::ToUTF8String(*txt);
        const xmlChar* str = reinterpret_cast<const xmlChar*>(tmp.c_str());
        if (!xmlCheckUTF8(str)) {
            LogError("Not valid UTF8");
            return;
        }

        int i = 0;
        xmlChar* c;
        while ((c = const_cast<xmlChar*>(xmlUTF8Strpos(str, i))) != NULL) {
            if (!IsSpace(c)) {
                break;
            }
            ++i;
        }

        xmlChar* tmpnew = xmlUTF8Strndup(c, xmlUTF8Strlen(c) + 1);
        bool first = false;
        xmlChar* s = tmpnew;
        while ((c = const_cast<xmlChar*>(xmlUTF8Strpos(str, i))) != NULL) {
            if (IsSpace(c)) {
                first = true;
                ++i;
            } else {
                if (c[0] == 0x0) {
                    break;
                }
                if (first && !isTrimSpace) {
                    xmlChar space[6] = { 0x20 };
                    CopyChar(s, space);
                    s += xmlUTF8Size(s);
                    first = false;
                }
                CopyChar(s, c);
                s += xmlUTF8Size(s);
                ++i;
            }
        }
        s[0] = 0x0;
        txt = DPL::FromUTF8String(reinterpret_cast<char*>(tmpnew));
        xmlFree(tmpnew);
    }
}

void NormalizeAndTrimSpaceString(DPL::OptionalString& txt)
{
    NormalizeString(txt, true);
}

bool ConfigParserData::Param::operator==(const Param& other) const
{
    return name == other.name && value == other.value;
}

bool ConfigParserData::Param::operator!=(const Param& other) const
{
    return name != other.name || value != other.value;
}

bool ConfigParserData::Param::operator >(const Param& other) const
{
    if (name == other.name) {
        return value > other.value;
    } else {
        return name > other.name;
    }
}

bool ConfigParserData::Param::operator>=(const Param& other) const
{
    if (name >= other.name) {
        return true;
    } else {
        return value >= other.value;
    }
}

bool ConfigParserData::Param::operator <(const Param& other) const
{
    if (name == other.name) {
        return value < other.value;
    } else {
        return name < other.name;
    }
}

bool ConfigParserData::Param::operator<=(const Param& other) const
{
    if (name <= other.name) {
        return true;
    } else {
        return value <= other.value;
    }
}

bool ConfigParserData::Feature::operator==(const Feature& other) const
{
    return name == other.name && paramsList == other.paramsList;
}

bool ConfigParserData::Feature::operator!=(const Feature& other) const
{
    return name != other.name || paramsList != other.paramsList;
}

bool ConfigParserData::Feature::operator >(const Feature& other) const
{
    if (name > other.name) {
        return true;
    }
    if (name < other.name) {
        return false;
    }
    return paramsList > other.paramsList;
}

bool ConfigParserData::Feature::operator>=(const Feature& other) const
{
    if (name > other.name) {
        return true;
    }
    if (name < other.name) {
        return false;
    }
    return paramsList >= other.paramsList;
}

bool ConfigParserData::Feature::operator <(const Feature& other) const
{
    if (name < other.name) {
        return true;
    }
    if (name > other.name) {
        return false;
    }
    return paramsList < other.paramsList;
}

bool ConfigParserData::Feature::operator<=(const Feature& other) const
{
    if (name < other.name) {
        return true;
    }
    if (name > other.name) {
        return false;
    }
    return paramsList <= other.paramsList;
}

bool ConfigParserData::Privilege::operator==(const Privilege& other) const
{
    return name == other.name;
}

bool ConfigParserData::Privilege::operator!=(const Privilege& other) const
{
    return name != other.name;
}

bool ConfigParserData::Privilege::operator >(const Privilege& other) const
{
    return name > other.name;
}

bool ConfigParserData::Privilege::operator>=(const Privilege& other) const
{
    return name >= other.name;
}

bool ConfigParserData::Privilege::operator <(const Privilege& other) const
{
    return name < other.name;
}

bool ConfigParserData::Privilege::operator<=(const Privilege& other) const
{
    return name < other.name;
}

bool ConfigParserData::Icon::operator==(const Icon& other) const
{
    return src == other.src;
}

bool ConfigParserData::Icon::operator!=(const Icon& other) const
{
    return src != other.src;
}

bool ConfigParserData::Icon::operator >(const Icon& other) const
{
    return src > other.src;
}

bool ConfigParserData::Icon::operator>=(const Icon& other) const
{
    return src >= other.src;
}

bool ConfigParserData::Icon::operator <(const Icon& other) const
{
    return src < other.src;
}

bool ConfigParserData::Icon::operator<=(const Icon& other) const
{
    return src <= other.src;
}

bool ConfigParserData::Preference::operator==(const Preference& other) const
{
    return name == other.name;
}

bool ConfigParserData::Preference::operator!=(const Preference& other) const
{
    return name != other.name;
}

bool ConfigParserData::Preference::operator >(const Preference& other) const
{
    return name > other.name;
}

bool ConfigParserData::Preference::operator>=(const Preference& other) const
{
    return name >= other.name;
}

bool ConfigParserData::Preference::operator <(const Preference& other) const
{
    return name < other.name;
}

bool ConfigParserData::Preference::operator<=(const Preference& other) const
{
    return name <= other.name;
}

bool ConfigParserData::AccessInfo::operator== (const AccessInfo& info) const
{
    return m_strIRI == info.m_strIRI && m_bSubDomainAccess ==
           info.m_bSubDomainAccess;
}

bool ConfigParserData::AccessInfo::operator!= (const AccessInfo& info) const
{
    return m_strIRI != info.m_strIRI || m_bSubDomainAccess !=
           info.m_bSubDomainAccess;
}

bool ConfigParserData::AccessInfo::operator <(const AccessInfo& info) const
{
    if (m_strIRI == info.m_strIRI) {
        return m_bSubDomainAccess < info.m_bSubDomainAccess;
    } else {
        return m_strIRI < info.m_strIRI;
    }
}

bool ConfigParserData::Setting::operator==(const Setting& other) const
{
    return m_name == other.m_name &&
           m_value == other.m_value;
}

bool ConfigParserData::Setting::operator!=(const Setting& other) const
{
    return m_name != other.m_name ||
           m_value != other.m_value;
}

bool ConfigParserData::Setting::operator >(const Setting& other) const
{
    return m_name > other.m_name;
}

bool ConfigParserData::Setting::operator>=(const Setting& other) const
{
    return m_name >= other.m_name;
}

bool ConfigParserData::Setting::operator <(const Setting& other) const
{
    return m_name < other.m_name;
}

bool ConfigParserData::Setting::operator<=(const Setting& other) const
{
    return m_name <= other.m_name;
}

bool ConfigParserData::ServiceInfo::operator== (const ServiceInfo& info) const
{
    return m_src == info.m_src &&
           m_operation == info.m_operation &&
           m_scheme == info.m_scheme &&
           m_mime == info.m_mime &&
           m_disposition == info.m_disposition;
}

bool ConfigParserData::ServiceInfo::operator!= (const ServiceInfo& info) const
{
    return m_src != info.m_src &&
           m_operation != info.m_operation &&
           m_scheme != info.m_scheme &&
           m_mime != info.m_mime &&
           m_disposition != info.m_disposition;
}

bool ConfigParserData::LiveboxInfo::operator==(const LiveboxInfo& other) const
{
    return m_liveboxId == other.m_liveboxId &&
           m_autoLaunch == other.m_autoLaunch &&
           m_updatePeriod == other.m_updatePeriod &&
           m_primary == other.m_primary &&
           m_label == other.m_label &&
           m_icon == other.m_icon;
}

bool ConfigParserData::LiveboxInfo::operator!=(const LiveboxInfo& other) const
{
    return m_liveboxId != other.m_liveboxId &&
           m_autoLaunch != other.m_autoLaunch &&
           m_updatePeriod != other.m_updatePeriod &&
           m_primary != other.m_primary &&
           m_label != other.m_label &&
           m_icon != other.m_icon;
}
} // namespace WrtDB
