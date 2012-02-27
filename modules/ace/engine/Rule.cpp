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
//
//
//
//  @ Project : Access Control Engine
//  @ File Name : Rule.h
//  @ Date : 2009-05-06
//  @ Author : Samsung
//
//

#include <iostream>
#include <dpl/log/log.h>

#include <dpl/ace/Rule.h>
#include <dpl/ace/Serializer.h>

void Rule::printData()
{
    std::cout << "Rule: effect: " << printEffect(this->effect) <<
    " condition: " << this->condition;
}

std::string Rule::printEffect(const Effect effect) const
{
    switch (effect) {
    case Deny:
        return "Deny";
    case PromptBlanket:
        return "PromptBlanket";
    case PromptOneShot:
        return "PromptOneShot";
    case PromptSession:
        return "PromptSession";
    case Permit:
        return "Permit";
    case Inapplicable:
        return "Inapplicable";
    case Error:
        return "Error";
    default:
        return "ERROR";
    }
}

Effect Rule::evaluateRule(const AttributeSet * attrSet) const
{
    Attribute::MatchResult result = condition.evaluateCondition(attrSet);

    if (result == Attribute::MatchResult::MRUndetermined) {
        //        LogInfo("Rule is undetermined");
        return Undetermined;
    } else if (result == Attribute::MatchResult::MRTrue) {
        //       LogInfo("Rule effect "<<printEffect(effect));
        return effect;
    }
    // LogInfo("Rule is inapplicable");
    return Inapplicable;
}

Rule::Rule(std::istream& is)
{
    Serializer* serializer = Serializer::getInstance();

    condition = serializer->deserializeCondition(is);
    effect = serializer->deserializeEffect(is);
}

void Rule::serialize(std::ostream& os)
{
    Serializer* serializer = Serializer::getInstance();

    serializer->serializeCondition(os, condition);
    serializer->serializeEffect(os, effect);
}

