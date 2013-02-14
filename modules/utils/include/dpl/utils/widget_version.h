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
 * @file    widget_version.h
 * @author  Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version 1.0
 * @brief   Header file for widget version
 */
#ifndef WIDGET_VERSION_H
#define WIDGET_VERSION_H

#include <dpl/string.h>
#include <dpl/optional.h>
#include <ostream>

/*
 * Note: This class also support non-WAC compliant version numbers
 *
 * WAC Waikiki Beta Release Core Specification: Widget Runtime
 * 10 Dec 2010
 *
 * WL-3370 The WRT MUST process widget packages as an update when received under
 * the following conditions:
 *
 *  - the Widget Id matches the Widget Id of an installed widget
 *  - the Widget version number is greater (as a compared string) than that of
 * the installed widget, or no version
 *    information was provided for the installed widget
 *
 * To ensure that a string comparison of widget versions can reliably determine
 * which version is an updated widget,
 * WAC will mandate a specific version string format for WAC widgets. All
 * widgets coming through the WAC channel
 * will be required to have version strings in this format. Side-loaded widgets
 * may have any format and, in this
 * case, there is no requirement that the WRT support version detection for
 * update of these widgets.
 *
 * The widget version format is the rec-version-tag grammar as described in
 * [Widget Packaging]:
 *
 *  rec-version-tag = 1*DIGIT "." 1*DIGIT [ "." 1*DIGIT] *[ 1*ALPHA / SP /
 * 1*DIGIT ]
 *
 * Examples of rec-version-tag:
 *
 *    1.0
 *    1.10.1 beta1
 *    1.02.12 RC1
 *
 * WL-3371 The WRT MUST use the following widget version comparison algorithm to
 * compare WAC widget version strings:
 *
 *  - prepare the version strings for comparison:
 *     - all leading zeros are discarded
 *     - the optional *[ 1*ALPHA / SP / 1*DIGIT ] part, if present, is discarded
 *     - the resulting numbers are then in the format major.minor[.micro]
 *  - Version A = Amajor.Aminor[.Amicro] is equal to Version B =
 * Bmajor.Bminor[.Bmicro] if and only if:
 *     - Amajor Bmajor
 *     - Aminor Bminor
 *     - both Amicro and Bmicro are present and Amicro == Bmicro; or both Amicro
 * and Bmicro are absent.
 *  - Version A = Amajor.Aminor[.Amicro] is greater than Version B =
 * Bmajor.Bminor[.Bmicro] if and only if:
 *     - Amajor > Bmajor; or
 *     - Amajor Bmajor && Aminor > Bminor; or
 *     - Amajor Bmajor && Aminor == Bminor && both Amicro and Bmicro are present
 * and Amicro > Bmicro; or Bmicro is absent.
 */
class WidgetVersion
{
  private:
    bool m_isWac;
    DPL::String m_raw;

    DPL::String m_major;
    DPL::String m_minor;
    DPL::Optional<DPL::String> m_micro;
    DPL::Optional<DPL::String> m_optional;

    void WacCertify(const DPL::String &major,
                    const DPL::String &minor,
                    const DPL::Optional<DPL::String> &micro,
                    const DPL::Optional<DPL::String> &optional);

  public:
    explicit WidgetVersion(const DPL::String &str = DPL::String());
    WidgetVersion(const DPL::String &major,
                  const DPL::String &minor,
                  const DPL::Optional<DPL::String> &micro,
                  const DPL::Optional<DPL::String> &optional);

    bool IsWac() const;
    const DPL::String &Raw() const;

    const DPL::String &Major() const;
    const DPL::String &Minor() const;
    const DPL::Optional<DPL::String> &Micro() const;
    const DPL::Optional<DPL::String> &Optional() const;
};

bool operator<(const WidgetVersion &left,
               const WidgetVersion &right);
bool operator<=(const WidgetVersion &left,
                const WidgetVersion &right);
bool operator>(const WidgetVersion &left,
               const WidgetVersion &right);
bool operator>=(const WidgetVersion &left,
                const WidgetVersion &right);
bool operator==(const WidgetVersion &left,
                const WidgetVersion &right);
bool operator!=(const WidgetVersion &left,
                const WidgetVersion &right);
std::ostream & operator<<(std::ostream& stream,
                          const WidgetVersion& version);

#endif // WIDGET_VERSION_H
