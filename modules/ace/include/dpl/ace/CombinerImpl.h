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
//  @ File Name : CombinerImpl.h
//  @ Date : 2009-05-06
//  @ Author : Samsung
//
//

#ifndef _COMBINER_IMPL_H
#define _COMBINER_IMPL_H

#include <list>
#include <dpl/log/log.h>

#include "Combiner.h"
#include "Effect.h"
#include "Policy.h"
#include "Subject.h"

class CombinerImpl : public Combiner
{
  public:

    virtual Effect combineRules(const TreeNode * rule);
    virtual Effect combinePolicies(const TreeNode * policy);

    virtual ~CombinerImpl()
    {
    }

  protected:

    bool checkIfTargetMatches(const std::list<const Subject *> * subjectsSet,
            bool &isUndetermined);

    Effect combine(Policy::CombineAlgorithm algorithm,
            std::list<Effect> & effects);

    Effect denyOverrides(const std::list<Effect> & effects);
    Effect permitOverrides(const std::list<Effect> & effects);
    Effect firstApplicable(const std::list<Effect> & effects);
    Effect firstMatchingTarget(const std::list<Effect> & effects);

    std::list<int> * convertEffectsToInts(const std::list<Effect> * effects);
    Effect convertIntToEffect(int intEffect);

    void showEffectList(std::list<Effect> & effects)
    {
        std::list<Effect>::iterator it = effects.begin();
        for (; it != effects.end(); ++it) {
            LogDebug(toString(*it));
        }
    }

  private:
    bool isError(const std::list<Effect> & effects);
    static const int
        DenyInt,
        UndeterminedInt,
        PromptOneShotInt,
        PromptSessionInt,
        PromptBlanketInt,
        PermitInt,
        InapplicableInt,
        NotMatchingTargetInt,
        ErrorInt;
};

#endif  //_COMBINERIMPL_H
