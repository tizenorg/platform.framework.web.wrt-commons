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
 * @file    TestSuite04.cpp
 * @author  unknown
 * @version 1.0
 * @brief   Test cases for Combiner.
 */

#include <list>
#include <string>

#include <dpl/log/log.h>
#include <dpl/test/test_runner.h>

#include <dpl/ace/CombinerImpl.h>
#include <dpl/ace/PolicySet.h>
#include <dpl/ace/Effect.h>

#define TESTSUITE04(n) \
RUNNER_TEST(ts04_combiner_tests_ ## n)

bool assertEffectEqual(Effect actual, Effect expected){
    return (actual == expected);
}

class WTF : public CombinerImpl{
public:
    // the evaluation functions should be static and public
    // but they are protected methods!
    Effect denyOverrides(std::list<Effect> &lst){
        return CombinerImpl::denyOverrides(lst);
    }
    Effect permitOverrides(std::list<Effect> &lst){
        return CombinerImpl::permitOverrides(lst);
    }
    Effect firstApplicable(std::list<Effect> &lst){
        return CombinerImpl::firstApplicable(lst);
    }
    Effect firstMatchingTarget(std::list<Effect> &lst){
        return CombinerImpl::firstMatchingTarget(lst);
    }
};

Effect denyOverridesTest(std::list<Effect> &lst) {
    WTF impl;
    return impl.denyOverrides(lst);
}

Effect permitOverridesTest(std::list<Effect> &lst) {
    WTF impl;
    return impl.permitOverrides(lst);
}

Effect firstApplicableTest(std::list<Effect> &lst) {
    WTF impl;
    return impl.firstApplicable(lst);
}

Effect firstMatchingTargetTest(std::list<Effect> &lst) {
    WTF impl;
    return impl.firstMatchingTarget(lst);
}

TESTSUITE04(00_denyOverrides){
    std::list<Effect> * effectList = new std::list<Effect > ();
    effectList->push_back(Deny);
    effectList->push_back(Undetermined);
    effectList->push_back(Permit);
    effectList->push_back(Inapplicable);

    Effect eff = denyOverridesTest(*effectList);
    RUNNER_ASSERT(assertEffectEqual(eff, Deny));
}

TESTSUITE04(01_denyOverrides){
    std::list<Effect> * effectList = new std::list<Effect > ();
    effectList->push_back(Undetermined);
    effectList->push_back(Permit);
    effectList->push_back(Inapplicable);
    effectList->push_back(Deny);
    effectList->push_back(Permit);

    Effect eff = denyOverridesTest(*effectList);
    RUNNER_ASSERT(assertEffectEqual(eff, Deny));
}

TESTSUITE04(02_denyOverrides){
    std::list<Effect> * effectList = new std::list<Effect > ();
    effectList->push_back(Undetermined);
    effectList->push_back(Permit);
    effectList->push_back(Inapplicable);
    effectList->push_back(Permit);

    Effect eff = denyOverridesTest(*effectList);
    RUNNER_ASSERT(assertEffectEqual(eff, Undetermined));
}

TESTSUITE04(03_denyOverrides){
    std::list<Effect> * effectList = new std::list<Effect > ();

    effectList->push_back(Permit);
    effectList->push_back(Inapplicable);
    effectList->push_back(Permit);
    effectList->push_back(Undetermined);
    effectList->push_back(PromptSession);

    Effect eff = denyOverridesTest(*effectList);
    RUNNER_ASSERT(assertEffectEqual(eff, Undetermined));
}

TESTSUITE04(04_denyOverrides){
    std::list<Effect> * effectList = new std::list<Effect > ();
    effectList->push_back(Permit);
    effectList->push_back(Inapplicable);
    effectList->push_back(Permit);
    effectList->push_back(PromptOneShot);
    effectList->push_back(PromptSession);
    effectList->push_back(PromptBlanket);

    Effect eff = denyOverridesTest(*effectList);
    RUNNER_ASSERT(assertEffectEqual(eff, PromptOneShot));
}

TESTSUITE04(05_denyOverrides){
    std::list<Effect> * effectList = new std::list<Effect > ();
    effectList->push_back(Permit);
    effectList->push_back(Inapplicable);
    effectList->push_back(Permit);
    effectList->push_back(PromptSession);
    effectList->push_back(PromptBlanket);

    Effect eff = denyOverridesTest(*effectList);
    RUNNER_ASSERT(assertEffectEqual(eff, PromptSession));
}

TESTSUITE04(06_denyOverrides){
    std::list<Effect> * effectList = new std::list<Effect > ();

    effectList->push_back(Permit);
    effectList->push_back(Inapplicable);
    effectList->push_back(Permit);
    effectList->push_back(PromptBlanket);

    Effect eff = denyOverridesTest(*effectList);
    RUNNER_ASSERT(assertEffectEqual(eff, PromptBlanket));
}

TESTSUITE04(07_denyOverrides){
    std::list<Effect> * effectList = new std::list<Effect > ();
    effectList->push_back(Permit);
    effectList->push_back(Inapplicable);
    effectList->push_back(Permit);

    Effect eff = denyOverridesTest(*effectList);
    RUNNER_ASSERT(assertEffectEqual(eff, Permit));
}

TESTSUITE04(08_denyOverrides){
    std::list<Effect> * effectList = new std::list<Effect > ();
    effectList->push_back(Inapplicable);

    Effect eff = denyOverridesTest(*effectList);
    RUNNER_ASSERT(assertEffectEqual(eff, Inapplicable));
}

TESTSUITE04(09_denyOverrides){
    std::list<Effect> * effectList = new std::list<Effect > ();
    Effect eff = denyOverridesTest(*effectList);
    RUNNER_ASSERT(assertEffectEqual(eff, Inapplicable));
}

TESTSUITE04(10_denyOverrides){
    std::list<Effect> * effectList = new std::list<Effect > ();
    effectList->push_back(Deny);
    effectList->push_back(Undetermined);
    effectList->push_back(Permit);
    effectList->push_back(Error);
    effectList->push_back(Inapplicable);

    Effect eff = denyOverridesTest(*effectList);
    RUNNER_ASSERT(assertEffectEqual(eff, Error));
}

TESTSUITE04(11_denyOverrides){
    std::list<Effect> * effectList = new std::list<Effect > ();
    effectList->push_back(Error);
    effectList->push_back(Undetermined);
    effectList->push_back(Error);
    effectList->push_back(Inapplicable);
    effectList->push_back(Permit);

    Effect eff = denyOverridesTest(*effectList);
    RUNNER_ASSERT(assertEffectEqual(eff, Error));
}

TESTSUITE04(12_denyOverrides){
    std::list<Effect> * effectList = new std::list<Effect > ();
    effectList->push_back(Permit);
    effectList->push_back(Inapplicable);
    effectList->push_back(Permit);
    effectList->push_back(Error);
    effectList->push_back(PromptOneShot);
    effectList->push_back(PromptSession);
    effectList->push_back(PromptBlanket);


    Effect eff = denyOverridesTest(*effectList);
    RUNNER_ASSERT(assertEffectEqual(eff, Error));
}

TESTSUITE04(13_denyOverrides){
    std::list<Effect> * effectList = new std::list<Effect > ();
    effectList->push_back(Permit);
    effectList->push_back(Inapplicable);
    effectList->push_back(Permit);
    effectList->push_back(Error);
    effectList->push_back(PromptBlanket);
    effectList->push_back(PromptSession);

    Effect eff = denyOverridesTest(*effectList);
    RUNNER_ASSERT(assertEffectEqual(eff, Error));
}

TESTSUITE04(14_denyOverrides){
    std::list<Effect> * effectList = new std::list<Effect > ();
    effectList->push_back(Error);
    effectList->push_back(Inapplicable);

    Effect eff = denyOverridesTest(*effectList);
    RUNNER_ASSERT(assertEffectEqual(eff, Error));
}

TESTSUITE04(15_denyOverrides){
    std::list<Effect> * effectList = new std::list<Effect > ();
    effectList->push_back(Inapplicable);
    effectList->push_back(Error);

    Effect eff = denyOverridesTest(*effectList);
    RUNNER_ASSERT(assertEffectEqual(eff, Error));
}

TESTSUITE04(16_permitOverrides){
    std::list<Effect> * effectList = new std::list<Effect > ();
    effectList->push_back(Deny);
    effectList->push_back(Undetermined);
    effectList->push_back(Permit);
    effectList->push_back(Inapplicable);
    effectList->push_back(PromptOneShot);
    effectList->push_back(PromptSession);
    effectList->push_back(PromptBlanket);

    Effect eff = permitOverridesTest(*effectList);
    RUNNER_ASSERT(assertEffectEqual(eff, Permit));
}

TESTSUITE04(17_permitOverrides){
    std::list<Effect> * effectList = new std::list<Effect > ();
    effectList->push_back(Deny);
    effectList->push_back(Inapplicable);
    effectList->push_back(PromptOneShot);
    effectList->push_back(PromptSession);
    effectList->push_back(PromptBlanket);
    effectList->push_back(Undetermined);

    Effect eff = permitOverridesTest(*effectList);
    RUNNER_ASSERT(assertEffectEqual(eff, Undetermined));
}

TESTSUITE04(18_permitOverrides){
    std::list<Effect> * effectList = new std::list<Effect > ();
    effectList->push_back(Deny);
    effectList->push_back(Inapplicable);
    effectList->push_back(PromptOneShot);
    effectList->push_back(PromptSession);
    effectList->push_back(PromptBlanket);


    Effect eff = permitOverridesTest(*effectList);
    RUNNER_ASSERT(assertEffectEqual(eff, PromptBlanket));
}

TESTSUITE04(19_permitOverrides){
    std::list<Effect> * effectList = new std::list<Effect > ();
    effectList->push_back(Deny);
    effectList->push_back(PromptOneShot);
    effectList->push_back(PromptSession);
    effectList->push_back(Inapplicable);

    Effect eff = permitOverridesTest(*effectList);
    RUNNER_ASSERT(assertEffectEqual(eff, PromptSession));
}

TESTSUITE04(20_permitOverrides){
    std::list<Effect> * effectList = new std::list<Effect > ();
    effectList->push_back(Deny);
    effectList->push_back(Inapplicable);
    effectList->push_back(PromptOneShot);

    Effect eff = permitOverridesTest(*effectList);
    RUNNER_ASSERT(assertEffectEqual(eff, PromptOneShot));
}

TESTSUITE04(21_permitOverrides){
    std::list<Effect> * effectList = new std::list<Effect > ();
    effectList->push_back(Deny);
    effectList->push_back(Inapplicable);

    Effect eff = permitOverridesTest(*effectList);
    RUNNER_ASSERT(assertEffectEqual(eff, Deny));
}

TESTSUITE04(22_permitOverrides){
    std::list<Effect> * effectList = new std::list<Effect > ();
    effectList->push_back(Inapplicable);

    Effect eff = permitOverridesTest(*effectList);
    RUNNER_ASSERT(assertEffectEqual(eff, Inapplicable));
}

TESTSUITE04(23_permitOverrides){
    std::list<Effect> * effectList = new std::list<Effect > ();

    Effect eff = permitOverridesTest(*effectList);
    RUNNER_ASSERT(assertEffectEqual(eff, Inapplicable));
}

TESTSUITE04(24_permitOverrides){
    std::list<Effect> * effectList = new std::list<Effect > ();
    effectList->push_back(Deny);
    effectList->push_back(Undetermined);
    effectList->push_back(Permit);
    effectList->push_back(Inapplicable);
    effectList->push_back(Error);
    effectList->push_back(PromptSession);
    effectList->push_back(PromptOneShot);
    effectList->push_back(PromptBlanket);

    Effect eff = permitOverridesTest(*effectList);
    RUNNER_ASSERT(assertEffectEqual(eff, Error));
}

TESTSUITE04(25_permitOverrides){
    std::list<Effect> * effectList = new std::list<Effect > ();
    effectList->push_back(Deny);
    effectList->push_back(Inapplicable);
    effectList->push_back(PromptOneShot);
    effectList->push_back(PromptSession);
    effectList->push_back(PromptBlanket);
    effectList->push_back(Undetermined);
    effectList->push_back(Error);

    Effect eff = permitOverridesTest(*effectList);
    RUNNER_ASSERT(assertEffectEqual(eff, Error));
}

TESTSUITE04(26_permitOverrides){
    std::list<Effect> * effectList = new std::list<Effect > ();
    effectList->push_back(Deny);
    effectList->push_back(Error);
    effectList->push_back(Inapplicable);
    effectList->push_back(PromptOneShot);

    Effect eff = permitOverridesTest(*effectList);
    RUNNER_ASSERT(assertEffectEqual(eff, Error));
}

TESTSUITE04(27_permitOverrides){
    std::list<Effect> * effectList = new std::list<Effect > ();
    effectList->push_back(Error);
    Effect eff = permitOverridesTest(*effectList);
    RUNNER_ASSERT(assertEffectEqual(eff, Error));
}

TESTSUITE04(28_firstApplicable){
    std::list<Effect> * effectList = new std::list<Effect > ();
    effectList->push_back(Deny);
    effectList->push_back(Inapplicable);
    effectList->push_back(PromptOneShot);
    effectList->push_back(PromptSession);
    effectList->push_back(PromptBlanket);
    effectList->push_back(Undetermined);

    Effect eff = firstApplicableTest(*effectList);
    RUNNER_ASSERT(assertEffectEqual(eff, Deny));
}

TESTSUITE04(29_firstApplicable){
    std::list<Effect> * effectList = new std::list<Effect > ();
    effectList->push_back(Undetermined);
    effectList->push_back(Deny);
    effectList->push_back(PromptOneShot);
    effectList->push_back(Inapplicable);
    effectList->push_back(PromptOneShot);
    effectList->push_back(PromptSession);
    effectList->push_back(PromptBlanket);
    effectList->push_back(Undetermined);

    Effect eff = firstApplicableTest(*effectList);
    RUNNER_ASSERT(assertEffectEqual(eff, Undetermined));
}

TESTSUITE04(30_firstApplicable){
    std::list<Effect> * effectList = new std::list<Effect > ();
    effectList->push_back(Inapplicable);
    effectList->push_back(Deny);
    effectList->push_back(PromptOneShot);
    effectList->push_back(Inapplicable);
    effectList->push_back(PromptOneShot);
    effectList->push_back(PromptSession);
    effectList->push_back(PromptBlanket);
    effectList->push_back(Undetermined);

    Effect eff = firstApplicableTest(*effectList);
    RUNNER_ASSERT(assertEffectEqual(eff, Deny));
}

TESTSUITE04(31_firstApplicable){
    std::list<Effect> * effectList = new std::list<Effect > ();
    effectList->push_back(Inapplicable);
    effectList->push_back(Undetermined);
    effectList->push_back(PromptOneShot);
    effectList->push_back(Inapplicable);
    effectList->push_back(PromptOneShot);
    effectList->push_back(PromptSession);
    effectList->push_back(PromptBlanket);
    effectList->push_back(Undetermined);

    Effect eff = firstApplicableTest(*effectList);
    RUNNER_ASSERT(assertEffectEqual(eff, Undetermined));
}

TESTSUITE04(32_firstApplicable){
    std::list<Effect> * effectList = new std::list<Effect > ();
    effectList->push_back(Inapplicable);
    effectList->push_back(Inapplicable);
    effectList->push_back(Inapplicable);

    Effect eff = firstApplicableTest(*effectList);
    RUNNER_ASSERT(assertEffectEqual(eff, Inapplicable));
}

TESTSUITE04(33_firstApplicable){
    std::list<Effect> * effectList = new std::list<Effect > ();
    effectList->push_back(Inapplicable);
    effectList->push_back(Inapplicable);
    effectList->push_back(Inapplicable);
    effectList->push_back(Permit);

    Effect eff = firstApplicableTest(*effectList);
    RUNNER_ASSERT(assertEffectEqual(eff, Permit));
}

TESTSUITE04(34_firstApplicable){
    std::list<Effect> * effectList = new std::list<Effect > ();

    Effect eff = firstApplicableTest(*effectList);
    RUNNER_ASSERT(assertEffectEqual(eff, Inapplicable));
}

TESTSUITE04(35_firstApplicable){
    std::list<Effect> * effectList = new std::list<Effect > ();
    effectList->push_back(Deny);
    effectList->push_back(Inapplicable);
    effectList->push_back(PromptOneShot);
    effectList->push_back(Error);
    effectList->push_back(PromptSession);
    effectList->push_back(PromptBlanket);
    effectList->push_back(Undetermined);

    Effect eff = firstApplicableTest(*effectList);
    RUNNER_ASSERT(assertEffectEqual(eff, Error));
}

TESTSUITE04(36_firstApplicable){
    std::list<Effect> * effectList = new std::list<Effect > ();
    effectList->push_back(Inapplicable);
    effectList->push_back(Inapplicable);
    effectList->push_back(Inapplicable);
    effectList->push_back(Error);
    effectList->push_back(Permit);

    Effect eff = firstApplicableTest(*effectList);
    RUNNER_ASSERT(assertEffectEqual(eff, Error));
}

TESTSUITE04(37_firstApplicable){
    std::list<Effect> * effectList = new std::list<Effect > ();
    effectList->push_back(Error);

    Effect eff = firstApplicableTest(*effectList);
    RUNNER_ASSERT(assertEffectEqual(eff, Error));
}

TESTSUITE04(38_firstMatching){
    std::list<Effect> * effectList = new std::list<Effect > ();
    effectList->push_back(Inapplicable);
    effectList->push_back(Deny);
    effectList->push_back(PromptOneShot);
    effectList->push_back(Inapplicable);

    Effect eff = firstMatchingTargetTest(*effectList);
    RUNNER_ASSERT(assertEffectEqual(eff, Inapplicable));
}

TESTSUITE04(39_firstMatching){
    std::list<Effect> * effectList = new std::list<Effect > ();
    effectList->push_back(Undetermined);
    effectList->push_back(Deny);
    effectList->push_back(PromptOneShot);
    effectList->push_back(Inapplicable);

    Effect eff = firstMatchingTargetTest(*effectList);
    RUNNER_ASSERT(assertEffectEqual(eff, Undetermined));
}

TESTSUITE04(39a_firstMatching){
    std::list<Effect> * effectList = new std::list<Effect > ();
    effectList->push_back(Deny);
    effectList->push_back(Inapplicable);

    Effect eff = firstMatchingTargetTest(*effectList);
    RUNNER_ASSERT(assertEffectEqual(eff, Deny));
}

TESTSUITE04(39b_firstMatching){
    std::list<Effect> * effectList = new std::list<Effect > ();
    effectList->push_back(Permit);

    Effect eff = firstMatchingTargetTest(*effectList);
    RUNNER_ASSERT(assertEffectEqual(eff, Permit));
}

TESTSUITE04(40_firstMatching){
    std::list<Effect> * effectList = new std::list<Effect > ();
    effectList->push_back(Undetermined);
    effectList->push_back(Undetermined);
    effectList->push_back(Undetermined);

    Effect eff = firstMatchingTargetTest(*effectList);
    RUNNER_ASSERT(assertEffectEqual(eff, Undetermined));
}

TESTSUITE04(41_firstMatching){
    std::list<Effect> * effectList = new std::list<Effect > ();

    Effect eff = firstMatchingTargetTest(*effectList);
    RUNNER_ASSERT(assertEffectEqual(eff, Inapplicable));
}

TESTSUITE04(42_firstMatching){
    std::list<Effect> * effectList = new std::list<Effect > ();
    effectList->push_back(Inapplicable);

    Effect eff = firstMatchingTargetTest(*effectList);
    RUNNER_ASSERT(assertEffectEqual(eff, Inapplicable));
}

TESTSUITE04(43_firstMatching){

    std::list<Effect> * effectList = new std::list<Effect > ();
    effectList->push_back(Undetermined);
    effectList->push_back(Undetermined);
    effectList->push_back(Undetermined);
    effectList->push_back(Deny);
    effectList->push_back(Inapplicable);
    effectList->push_back(Undetermined);
    effectList->push_back(Deny);
    effectList->push_back(PromptOneShot);
    effectList->push_back(Inapplicable);

    Effect eff = firstMatchingTargetTest(*effectList);
    RUNNER_ASSERT(assertEffectEqual(eff, Undetermined));
}

TESTSUITE04(44_firstMatching){

    std::list<Effect> * effectList = new std::list<Effect > ();
    effectList->push_back(Inapplicable);
    effectList->push_back(Deny);
    effectList->push_back(PromptOneShot);
    effectList->push_back(Error);
    effectList->push_back(Inapplicable);

    Effect eff = firstMatchingTargetTest(*effectList);
    RUNNER_ASSERT(assertEffectEqual(eff, Error));
}

TESTSUITE04(45_firstMatching){

    std::list<Effect> * effectList = new std::list<Effect > ();
    effectList->push_back(Undetermined);
    effectList->push_back(Undetermined);
    effectList->push_back(Error);
    effectList->push_back(Undetermined);
    effectList->push_back(Deny);
    effectList->push_back(Inapplicable);
    effectList->push_back(Undetermined);
    effectList->push_back(Deny);
    effectList->push_back(PromptOneShot);
    effectList->push_back(Inapplicable);

    Effect eff = firstMatchingTargetTest(*effectList);
    RUNNER_ASSERT(assertEffectEqual(eff, Error));
}

TESTSUITE04(46_firstMatching){

    std::list<Effect> * effectList = new std::list<Effect > ();
    effectList->push_back(Error);

    Effect eff = firstMatchingTargetTest(*effectList);
    RUNNER_ASSERT(assertEffectEqual(eff, Error));
}
