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
 * @file        DeveloperModeValidator.cpp
 * @author      Bartosz Janiak (b.janiak@samsung.com)
 * @version     1.0
 * @brief       DeveloperModeValidatorValidator - implementing WAC 2.0 spec, including TargetRestriction
 */

#include "DeveloperModeValidator.h"
#include <algorithm>
#include <vconf.h>
#include <ITapiMisc.h>
#include <dpl/log/log.h>
#include <dpl/scoped_free.h>

namespace ValidationCore {

DeveloperModeValidator::DeveloperModeValidator(bool complianceMode,
                                               const std::string& fakeIMEI,
                                               const std::string& fakeMEID):
    m_complianceModeEnabled(complianceMode),
    m_fakeIMEI(fakeIMEI),
    m_fakeMEID(fakeMEID)
{
}

void DeveloperModeValidator::check(const SignatureData &data)
{
    LogDebug("entered");
    const SignatureData::IMEIList& IMEIList = data.getIMEIList();
    const SignatureData::MEIDList& MEIDList = data.getMEIDList();

    if (IMEIList.empty() && MEIDList.empty()) {
        LogDebug("No TargetRestriction in signature.");
        return;
    }

    if (!IMEIList.empty()) {
        std::string phoneIMEIString = m_fakeIMEI;
        if (!m_complianceModeEnabled) {
            LogDebug("Compilance Mode is not enabled");
            DPL::ScopedFree<char> phoneIMEI(
                vconf_get_str(VCONFKEY_TELEPHONY_IMEI));
            if (!phoneIMEI.Get()) {
                ThrowMsg(Exception::NoTargetRestrictionSatisfied,
                         "Unable to get phone IMEI from vconf.");
            }
            phoneIMEIString = phoneIMEI.Get();
        }

        LogDebug("Phone IMEI: " << phoneIMEIString);
        if (IMEIList.end() ==
            std::find(IMEIList.begin(), IMEIList.end(), phoneIMEIString))
        {
            Throw(Exception::NoTargetRestrictionSatisfied);
        }
    }

    if (!MEIDList.empty()) {
        std::string phoneMEIDString = m_fakeMEID;
        if (!m_complianceModeEnabled)
        {
            TelMiscSNInformation phoneInfo;
            if (TAPI_API_SUCCESS !=
                tel_get_misc_me_sn(TAPI_MISC_ME_MEID, &phoneInfo))
            {
                ThrowMsg(Exception::NoTargetRestrictionSatisfied,
                         "Unable to get phone MEID from Tapi service.");
            }
            phoneMEIDString = reinterpret_cast<char*>(phoneInfo.szNumber);
        }

        LogDebug("Phone MEID: " << phoneMEIDString);
        if (MEIDList.end() ==
            std::find(MEIDList.begin(), MEIDList.end(), phoneMEIDString))
        {
            Throw(Exception::NoTargetRestrictionSatisfied);
        }
    }
    LogDebug("exit: ok");
}

} //ValidationCore
