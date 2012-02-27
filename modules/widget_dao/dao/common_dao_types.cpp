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
 *
 * @file    common_dao_types.h
 * @author  Michal Ciepielski (m.ciepielski@samsung.com)
 * @version 1.0
 * @brief   This file contains the implementation of common data types for wrtdb
 */

#include <dpl/wrt-dao-ro/common_dao_types.h>

#include <dpl/log/log.h>

namespace WrtDB {
namespace Powder {

Description::LevelEntry::LevelEntry(LevelEnum level) :
    level(level)
{
}

bool Description::LevelEntry::isContextValid(LevelEnum level,
        const DPL::OptionalString& aContext) const
{
    if (!aContext) {
        return level < level;
    } else {
        Context::const_iterator iter = context.find(*aContext);
        if (iter != context.end()) {
            return level < level;
        } else {
            return true;
        }
    }
}

bool Description::CategoryEntry::isCategoryValid(LevelEntry& aReason,
        LevelEnum aLevel,
        const DPL::OptionalString& aContext) const
{
    for (LevelsContainer::const_iterator iter = levels.begin();
         iter != levels.end(); ++iter) {
        if (!iter->isContextValid(aLevel, aContext)) {
            aReason = *iter;
            return false;
        }
    }
    return true;
}
}

namespace ChildProtection
{

PowderRules::CategoryRule::CategoryRule(const DPL::String& aCategory,
        Powder::Description::LevelEnum aLevel,
        const DPL::OptionalString& aContext) :
    category(aCategory),
    level(aLevel),
    context(aContext)
{
}

PowderRules::PowderResult::PowderResult(InvalidReason aReason,
        const Powder::Description::LevelEntry& anInvalidDescription,
        const CategoryRule& anInvalidRule) :
    invalidDescription(anInvalidDescription),
    invalidRule(anInvalidRule),
    reason(aReason)
{
}

//! Function checks if rule is fulfilled by description
//! \param[in] rule checked rule
//! \param[in] description
//! \retval true rule is valid
//! \retval false rule is invalid
PowderRules::ResultPair PowderRules::isRuleValidForDescription(
        const CategoryRule& aRule,
        const Powder::Description& aDescription) const
{
    Powder::Description::CategoryEntries::const_iterator
        iter = aDescription.categories.find(aRule.category);
    if (iter != aDescription.categories.end()) {
        Powder::Description::LevelEntry invalidDescription;
        if (!iter->second.isCategoryValid(invalidDescription, aRule.level,
                                          aRule.context)) {
            LogWarning("Widget forbidden for children detected");
            return std::make_pair(false,
                                  PowderResult(PowderResult::InvalidRule,
                                               invalidDescription,
                                               aRule));
        } else {
            return std::make_pair(true, PowderResult());
        }
    } else {
        return std::make_pair(true, PowderResult());
    }
}

//! Function checks if age limit is fulfilled by description
//! \param[in] description
//! \retval true age is valid
//! \retval false age is invalid
PowderRules::ResultPair PowderRules::isAgeValidForDescription(
        const Powder::Description& aDescription) const
{
    if (!ageLimit) {
        return std::make_pair(true, PowderResult());
    } else {
        if (!!aDescription.ageRating) {
            if (*aDescription.ageRating <= *ageLimit) {
                return std::make_pair(true, PowderResult());
            } else {
                return std::make_pair(false,
                                      PowderResult(PowderResult::InvalidAge));
            }
        } else {
            if (!isAgeRatingRequired) {
                return std::make_pair(true, PowderResult());
            } else {
                return std::make_pair(
                           false,
                           PowderResult(PowderResult::AgeRatingNotSet));
            }
        }
    }
}

//! Function check if Widget description is valid for ChildProtection
//! configuration
//! \param description widget description
//! \retval true widget is valid
//! \retval false widget is invalid
PowderRules::ResultPair PowderRules::isDescriptionValid(
        const Powder::Description& aDescription) const
{
    ResultPair powderResult;
    for (RulesContainer::const_iterator iter = rules.begin();
         iter != rules.end(); ++iter) {
        powderResult = isRuleValidForDescription(*iter, aDescription);
        if (!powderResult.first) {
            return powderResult;
        }
    }
    return isAgeValidForDescription(aDescription);
}

}
} // namespace WrtDB
