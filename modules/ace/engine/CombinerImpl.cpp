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
//  @ File Name : CombinerImpl.cpp
//  @ Date : 2009-05-06
//  @ Author : Samsung
//
//

#include <dpl/log/log.h>
#include <dpl/assert.h>

#include <dpl/ace/CombinerImpl.h>
#include <dpl/ace/Rule.h>
#include <dpl/ace/Policy.h>

const int CombinerImpl::DenyInt = 0,   //mb jk changed the numbers !!!!!!!!!!!!!!!!!
CombinerImpl::UndeterminedInt = 1,             //this one is added
CombinerImpl::PromptOneShotInt = 2,
CombinerImpl::PromptSessionInt = 3,
CombinerImpl::PromptBlanketInt = 4,
CombinerImpl::PermitInt = 5,
CombinerImpl::InapplicableInt = 6,
CombinerImpl::NotMatchingTargetInt = 7,
CombinerImpl::ErrorInt = 8;

std::list<int> * CombinerImpl::convertEffectsToInts(
        const std::list<Effect> * effects)
{
    std::list<int> * intList = new std::list<int>();
    for (
        std::list<Effect>::const_iterator it = effects->begin();
        it != effects->end();
        ++it
        ) {
        switch (*it) {
        case Deny:
            intList->push_back(CombinerImpl::DenyInt);
            break;
        case Undetermined:        //jk mb added
            intList->push_back(CombinerImpl::UndeterminedInt);
            break;
        case Permit:
            intList->push_back(CombinerImpl::PermitInt);
            break;
        case PromptBlanket:
            intList->push_back(CombinerImpl::PromptBlanketInt);
            break;
        case PromptOneShot:
            intList->push_back(CombinerImpl::PromptOneShotInt);
            break;
        case PromptSession:
            intList->push_back(CombinerImpl::PromptSessionInt);
            break;
        case Inapplicable:
            intList->push_back(CombinerImpl::InapplicableInt);
            break;
        case NotMatchingTarget:
            intList->push_back(CombinerImpl::NotMatchingTargetInt);
            break;
        case Error:
            intList->push_back(CombinerImpl::ErrorInt);
            break;
        default:
            Assert(false && "Wrong effect");
        }
    }
    return intList;
}

Effect CombinerImpl::convertIntToEffect(int intEffect)
{
    switch (intEffect) {
    case DenyInt:
        return Deny;

    case UndeterminedInt: //jk mb added
        return Undetermined;

    case PermitInt:
        return Permit;

    case PromptBlanketInt:
        return PromptBlanket;

    case PromptOneShotInt:
        return PromptOneShot;

    case PromptSessionInt:
        return PromptSession;

    case InapplicableInt:
        return Inapplicable;

    case NotMatchingTargetInt:
        return NotMatchingTarget;

    case ErrorInt:
        return Error;

    default:
        Assert(false && "Wrong integer in int to Effect conversion");
        return Error;
    }
}

Effect CombinerImpl::denyOverrides(const std::list<Effect> & effects)
{
    if (isError(effects)) {
        return Error;
    }

    std::list<int> * intList = convertEffectsToInts(&effects);

    int result = InapplicableInt;   //Deny has value 0, Undetermined value 1,  Inapplicable value 6

    std::list<int>::const_iterator it = intList->begin();

    while (it != intList->end()) {
        int effect = *it;
        //Use most restrictive policy always
        result = effect < result ? effect  : result;
        ++it;
    }
    delete intList;
    return convertIntToEffect(result);
}

Effect CombinerImpl::permitOverrides(const std::list<Effect> & effects)   //mb jk modified
{ if (isError(effects)) {
      return Error;
  }

  int result = DenyInt;   //Deny has value 0, Inapplicable value 5
  std::list<int> * intList = convertEffectsToInts(&effects);
  std::list<int>::const_iterator it = intList->begin();

  //Flag used to indicate that any of Deny,prompt-*,permit options appear
  //Consequently if flag is true then result should be return, otherwise inapplicable should be returned
  bool flag = false;
  bool flagUndetermined = false;

  while (it != intList->end()) {
      int effect = *it;

      if (effect == PermitInt) {
          result = effect;
          delete intList;
          return convertIntToEffect(result);
      } // no need for further check if "permit" found
      if (effect == UndeterminedInt) { flagUndetermined = true; } //check for undetermined

      //Set the flag and the result even if effect is equal to result
      //It is done to mark if any "Deny" effect occured
      if (effect >= result && effect != InapplicableInt && effect !=
          UndeterminedInt) {                                                                 //check for other results
          result = effect;
          flag = true;
      }

      ++it;
  }
  delete intList;

  if (flagUndetermined) {
      return Undetermined;
  }

  if (!flag) {
      return Inapplicable;
  }
  return convertIntToEffect(result);
}

Effect CombinerImpl::firstApplicable(const std::list<Effect> & effects)  //jk mb modified to comply with BONDI 1.0 ("undetermined" handling)
{ std::list<Effect>::const_iterator it = effects.begin();

  if (isError(effects)) {
      return Error;
  }

  for (; it != effects.end(); ++it) {
      if (*it != InapplicableInt) {
          return *it;
      }
  }
  return Inapplicable;
}

Effect CombinerImpl::firstMatchingTarget(const std::list<Effect> & effects)
{
    if (isError(effects)) {
        return Error;
    }
    // effect list constains result of policies which target has been matched.
    //
    // If target does not match policy result is NotMatchingTarget
    // NotMatchingTarget values are not stored on the effects list
    // (you can check it in combinePolicies function).
    //
    // So we are intrested in first value on the list.
    return effects.empty() ? Inapplicable : effects.front();
}

bool CombinerImpl::isError(const std::list<Effect> & effects)
{
    FOREACH(it, effects)
    {
        if (Error == *it) {
            return true;
        }
    }
    return false;
}

Effect CombinerImpl::combineRules(const TreeNode * policy)
{
    const Policy * policyObj = dynamic_cast<const Policy *>(policy->getElement());
    if (!policyObj) {
        LogError("dynamic_cast failed. PolicyObj is null.");
        return Error;
    }

    Policy::CombineAlgorithm algorithm = policyObj->getCombineAlgorithm();

    Assert(
        algorithm != Policy::FirstTargetMatching &&
        "Policy cannot have algorithm first target matching");

    bool isUndetermined = false;

    if (!checkIfTargetMatches(policyObj->getSubjects(), isUndetermined)) {
        if (isUndetermined) {
            //TODO Target is undetermined what should we do now ??
            //Right now simply return NotMatchingTarget
        }
        //Target doesn't match
        return NotMatchingTarget;
    }
    //Get all rules
    const ChildrenSet & children = policy->getChildrenSet();
    ChildrenConstIterator it = children.begin();
    std::list<Effect> effects;

    while (it != children.end()) {
        const Rule * rule = dynamic_cast<const Rule *>((*it)->getElement());

        if (!rule) {
            LogError("Error in dynamic_cast. rule is null");
            return Error;
        }

        Effect effect = rule->evaluateRule(this->getAttributeSet());
        effects.push_back(effect);
        if (algorithm == Policy::FirstApplicable && effect != Inapplicable) {
            //For first applicable algorithm we may stop after evaluating first policy
            //which has effect other than inapplicable
            break;
        }
        ++it;
    } //end policy children iteration

    //Use combining algorithm
    Effect ef = combine(policyObj->getCombineAlgorithm(), effects);
    return ef;
}

//WARNING this method makes an assumption that Policy target is a policy child
Effect CombinerImpl::combinePolicies(const TreeNode * policy)
{
    const Policy * policySet = dynamic_cast<const Policy *>(policy->getElement());

    if (!policySet) {
        LogError("dynamic_cast failed. Policy set is null.");
        return Error;
    }

    bool isUndetermined = false;
    Policy::CombineAlgorithm algorithm = policySet->getCombineAlgorithm();

    if (!checkIfTargetMatches(policySet->getSubjects(), isUndetermined)) {
        /*   I can't explain this...
        if (isUndetermined) {
            if (algorithm == Policy::FirstTargetMatching) {
                return Undetermined;
            }
        }
        */
        //Target doesn't match
        return NotMatchingTarget;
    }

    const ChildrenSet & children = policy->getChildrenSet();

    ChildrenConstIterator it = children.begin();

    std::list<Effect> effects;

    while (it != children.end()) {
        Effect effect;

        if ((*it)->getTypeID() == TreeNode::PolicySet) {
            effect = combinePolicies(*it);
            if (effect != NotMatchingTarget) {
                effects.push_back(effect);
            }
        } else if ((*it)->getTypeID() == TreeNode::Policy) {
            effect = combineRules(*it);
            if (effect != NotMatchingTarget) {
                effects.push_back(effect);
            }
        } else {
            // [CR] fix it
            LogError("effect value is not initialized!");
            return Error;
        }

        if (algorithm == Policy::FirstTargetMatching && effect !=
            NotMatchingTarget) {
            //In First matching target algorithm we may return when first result is found
            break;
        }
        ++it;
    }

    //Use combining algorithm
    Effect ef = combine(policySet->getCombineAlgorithm(), effects);
    return ef;
}

Effect CombinerImpl::combine(Policy::CombineAlgorithm algorithm,
        std::list<Effect> & effects)
{
    LogDebug("Effects to be combined with algorithm: " << toString(algorithm));
    showEffectList(effects);

    switch (algorithm) {
    case Policy::DenyOverride:
        return denyOverrides(effects);
        break;
    case Policy::PermitOverride:
        return permitOverrides(effects);
        break;
    case Policy::FirstApplicable:
        return firstApplicable(effects);
        break;
    case Policy::FirstTargetMatching:
        return firstMatchingTarget(effects);
        break;
    default:
        Assert(false && "Wrong combining algorithm used");
        return Error;
    }
}

/**
 *
 * @param attrSet set of Subject attributes in policy that identifies target
 * @return true if target  is determined and matches, false and isUndertmined is set to true if the target is undetermined
 * false and isUndetermined set to false if target is determined but doesn't match
 */
bool CombinerImpl::checkIfTargetMatches(
        const std::list<const Subject *> * subjectsList,
        bool &isUndetermined)
{
    if (subjectsList->empty()) {
        return true;
    }

    std::list<const Subject *>::const_iterator it = subjectsList->begin();
    bool match = false;
    //According to BONDI 1.0 at least one target must match
    while (it != subjectsList->end()) {
        match = (*it)->matchSubject(this->getAttributeSet(), isUndetermined);
        if (match) { //at least one match
            break;
        }
        ++it;
    }

    #ifdef _DEBUG
    if (match == Attribute::MRTrue) {
        LogDebug("Target matches ");
    } else if (match == Attribute::MRUndetermined) {
        LogDebug("Target match undetermined ");
    } else {
        LogDebug("Target doesn't match");
    }
    #endif
    return match;
}

const char * toString(Effect effect)
{
    const char * temp = "";

    switch (effect) {
    case Deny:
        temp = "Deny";
        break;
    case Undetermined:
        temp = "Undetermined";
        break;
    case PromptOneShot:
        temp = "PromptOneShot";
        break;
    case PromptSession:
        temp = "PromptSession";
        break;
    case PromptBlanket:
        temp = "PromptBlanket";
        break;
    case Permit:
        temp = "Permit";
        break;
    case Inapplicable:
        temp = "Inapplicable";
        break;
    case NotMatchingTarget:
        temp = "NotMatchingTarget";
        break;
    case Error:
        temp = "Error";
        break;
    default:
        Assert(false && "Wrong effect");
    }
    return temp;
}
