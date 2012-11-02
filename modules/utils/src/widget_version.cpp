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
 * @file    widget_version.cpp
 * @author  Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version 1.0
 * @brief   Implementation file for widget version
 */
#include <stddef.h>
#include <dpl/utils/widget_version.h>
#include <dpl/assert.h>
#include <dpl/log/log.h>
#include <ctype.h>
#include <list>

namespace // anonymous
{
size_t WAC_CERTIFY_MANDATORY_PART_LOW_COUNT = 2;
size_t WAC_CERTIFY_MANDATORY_PART_HIGH_COUNT = 3;
size_t WAC_CERTIFY_MANDATORY_PART_MAJOR_INDEX = 0;
size_t WAC_CERTIFY_MANDATORY_PART_MINOR_INDEX = 1;
size_t WAC_CERTIFY_MANDATORY_PART_MICRO_INDEX = 2;
DPL::String::value_type WAC_CERTIFY_MANDATORY_VS_OPTIONAL_SPLIT_CHAR = L' ';
DPL::String::value_type WAC_CERTIFY_MANDATORY_NUMBER_PART_SPLIT_CHAR = L'.';
DPL::String::value_type LEADING_ZERO_CHAR = L'0';

//
// [ABNF]
// Augmented BNF for Syntax Specifications: ABNF. RFC5234. D. Crocker and P. Overell. January 2008.
//
// ALPHA          =  %x41-5A / %x61-7A
inline bool IsAlpha(int c)
{
    return (c >= 0x41 && c <= 0x5A) ||
           (c >= 0x61 && c <= 0x7A);
}

// DIGIT          =  %x30-39
inline bool IsDigit(int c)
{
    return c >= 0x30 && c <= 0x39;
}

// SP             =  %x20
inline bool IsSp(int c)
{
    return c == 0x20;
}

DPL::String RemoveLeadingZeroes(const DPL::String &str)
{
    Assert(!str.empty());

    if (str[0] != LEADING_ZERO_CHAR) {
        return str;
    }

    size_t pos = 0;

    while (pos + 1 < str.size() && str[pos + 1] == LEADING_ZERO_CHAR) {
        ++pos;
    }

    return str.substr(pos);
}

// operator <
bool NumberLessOperator(const DPL::String &left,
        const DPL::String &right)
{
    // Assume: No leading zeroes
    if (left.size() < right.size()) {
        return true;
    }

    if (left.size() > right.size()) {
        return false;
    }

    // Now: left.size() == right.size()
    for (ssize_t i = static_cast<ssize_t>(left.size()) - 1; i >= 0; --i) {
        if (left[i] < right[i]) {
            return true;
        }

        if (left[i] > right[i]) {
            return false;
        }
    }

    // Equal
    return false;
}

bool WacCertifyNumberString(const DPL::String &str)
{
    for (DPL::String::const_iterator i = str.begin(); i != str.end(); ++i) {
        if (!IsDigit(*i)) {
            return false;
        }
    }

    return true;
}

bool WacCertifyAlphaNumberStringSpace(const DPL::String &str)
{
    for (DPL::String::const_iterator i = str.begin(); i != str.end(); ++i) {
        if (!IsDigit(*i) && !IsAlpha(*i) && !IsSp(*i)) {
            return false;
        }
    }

    return true;
}
} // anonymous

WidgetVersion::WidgetVersion(const DPL::String &str) :
    m_isWac(false),
    m_raw(str)
{
    LogDebug("Parsing version string: " << str);

    // Split optional an mandatory parts
    size_t optionalPartPosition = str.find(
            WAC_CERTIFY_MANDATORY_VS_OPTIONAL_SPLIT_CHAR);

    DPL::String mandatoryPart;
    DPL::Optional<DPL::String> optionalPart;

    if (optionalPartPosition == DPL::String::npos) {
        mandatoryPart = str;
    } else {
        mandatoryPart = str.substr(0, optionalPartPosition);
        optionalPart = str.substr(optionalPartPosition + 1, DPL::String::npos);
    }

    LogDebug("Mandatory part is: " << mandatoryPart);
    LogDebug("Optional part is: " << optionalPart);

    // Split string and construct version
    std::vector<DPL::String> parts;
    DPL::Tokenize(mandatoryPart,
                  WAC_CERTIFY_MANDATORY_NUMBER_PART_SPLIT_CHAR,
                  std::back_inserter(parts),
                  false);

    LogDebug("Tokenized mandatory parts: " << parts.size());

    if (parts.size() != WAC_CERTIFY_MANDATORY_PART_LOW_COUNT &&
        parts.size() != WAC_CERTIFY_MANDATORY_PART_HIGH_COUNT) {
        return;
    }

    DPL::String major;
    DPL::String minor;
    DPL::Optional<DPL::String> micro;

    // Certify for Wac
    major = parts[WAC_CERTIFY_MANDATORY_PART_MAJOR_INDEX];
    minor = parts[WAC_CERTIFY_MANDATORY_PART_MINOR_INDEX];

    if (parts.size() == WAC_CERTIFY_MANDATORY_PART_HIGH_COUNT) {
        micro = parts[WAC_CERTIFY_MANDATORY_PART_MICRO_INDEX];
    }

    WacCertify(major, minor, micro, optionalPart);
}

WidgetVersion::WidgetVersion(const DPL::String &major,
        const DPL::String &minor,
        const DPL::Optional<DPL::String> &micro,
        const DPL::Optional<DPL::String> &optional) :
    m_isWac(false)
{
    // Create Raw version
    m_raw += major;
    m_raw += WAC_CERTIFY_MANDATORY_NUMBER_PART_SPLIT_CHAR;
    m_raw += minor;
    m_raw += WAC_CERTIFY_MANDATORY_NUMBER_PART_SPLIT_CHAR;

    if (!!micro) {
        m_raw += *micro;
    }

    if (!!optional) {
        m_raw += WAC_CERTIFY_MANDATORY_VS_OPTIONAL_SPLIT_CHAR;
        m_raw += *optional;
    }

    // Certify for Wac
    WacCertify(major, minor, micro, optional);
}

void WidgetVersion::WacCertify(const DPL::String &major,
        const DPL::String &minor,
        const DPL::Optional<DPL::String> &micro,
        const DPL::Optional<DPL::String> &optional)
{
    LogDebug("Certyfing...");

    LogDebug("Major candidate: " << major);
    LogDebug("Minor candidate: " << minor);
    LogDebug("Micro candidate: " << micro);
    LogDebug("Optional candidate: " << optional);

    // Check strings
    if (major.empty() || !WacCertifyNumberString(major)) {
        LogDebug("Major version not certified!");
        return;
    }

    if (minor.empty() || !WacCertifyNumberString(minor)) {
        LogDebug("Minor version not certified!");
        return;
    }

    if (!!micro && (micro->empty() || !WacCertifyNumberString(*micro))) {
        LogDebug("Microversion not certified!");
        return;
    }

    if (!!optional &&
        (optional->empty() || !WacCertifyAlphaNumberStringSpace(*optional))) {
        LogDebug("Optional version not certified!");
        return;
    }

    // OK
    m_major = major;
    m_minor = minor;
    m_micro = micro;
    m_optional = optional;

    m_isWac = true;

    LogDebug("Certified.");
}

bool WidgetVersion::IsWac() const
{
    return m_isWac;
}

const DPL::String &WidgetVersion::Raw() const
{
    return m_raw;
}

const DPL::String &WidgetVersion::Major() const
{
    return m_major;
}

const DPL::String &WidgetVersion::Minor() const
{
    return m_minor;
}

const DPL::Optional<DPL::String> &WidgetVersion::Micro() const
{
    return m_micro;
}

const DPL::Optional<DPL::String> &WidgetVersion::Optional() const
{
    return m_optional;
}

bool operator<(const WidgetVersion &left,
        const WidgetVersion &right)
{
    Assert(
        left.IsWac() && right.IsWac() &&
        "Only WAC version strings are comparable!");

    if (NumberLessOperator(RemoveLeadingZeroes(left.Major()),
                           RemoveLeadingZeroes(right.Major()))) { return true; }
    if (NumberLessOperator(RemoveLeadingZeroes(right.Major()),
                           RemoveLeadingZeroes(left.Major()))) { return false; }

    if (NumberLessOperator(RemoveLeadingZeroes(left.Minor()),
                           RemoveLeadingZeroes(right.Minor()))) { return true; }
    if (NumberLessOperator(RemoveLeadingZeroes(right.Minor()),
                           RemoveLeadingZeroes(left.Minor()))) { return false; }

    if (!!left.Micro() && !!right.Micro() &&
        NumberLessOperator(RemoveLeadingZeroes(*left.Micro()),
                           RemoveLeadingZeroes(*right.Micro()))) { return true; }
    if (!left.Micro() && !!right.Micro()) { return true; }

    return false;
}

bool operator<=(const WidgetVersion &left,
        const WidgetVersion &right)
{
    Assert(
        left.IsWac() && right.IsWac() &&
        "Only WAC version strings are comparable!");

    return (left == right) || (left < right);
}

bool operator>(const WidgetVersion &left,
        const WidgetVersion &right)
{
    Assert(
        left.IsWac() && right.IsWac() &&
        "Only WAC version strings are comparable!");

    return !(left <= right);
}

bool operator>=(const WidgetVersion &left,
        const WidgetVersion &right)
{
    Assert(
        left.IsWac() && right.IsWac() &&
        "Only WAC version strings are comparable!");

    return (left == right) || (left > right);
}

bool operator==(const WidgetVersion &left,
        const WidgetVersion &right)
{
    Assert(
        left.IsWac() && right.IsWac() &&
        "Only WAC version strings are comparable!");

    return RemoveLeadingZeroes(left.Major()) ==
           RemoveLeadingZeroes(right.Major()) &&                                       // Major are equal
           RemoveLeadingZeroes(left.Minor()) ==
           RemoveLeadingZeroes(right.Minor()) &&                                       // and Minor are equal
           (                                                                           // and ...
               (!!left.Micro() && !!right.Micro() &&
                RemoveLeadingZeroes(*left.Micro()) ==
                RemoveLeadingZeroes(*right.Micro())) ||                                                                              // Both Micro exist and are equal
               (!left.Micro() && !right.Micro())                                                                                     // or both Micro do not exist
           );
}

bool operator!=(const WidgetVersion &left,
        const WidgetVersion &right)
{
    Assert(
        left.IsWac() && right.IsWac() &&
        "Only WAC version strings are comparable!");

    return !(left == right);
}

std::ostream & operator<<(std::ostream& stream,
        const WidgetVersion& version)
{
    stream << version.Raw();
    return stream;
}
