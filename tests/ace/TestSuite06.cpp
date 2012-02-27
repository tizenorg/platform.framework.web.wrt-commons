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
 * @file    TestSuite06.cpp
 * @author  unknown
 * @version 1.0
 * @brief   Test cases for AceDAO implementation.
 */
#include <iostream>
#include <string>

#include <dpl/foreach.h>
#include <dpl/test/test_runner.h>

#include <dpl/ace-dao-rw/AceDAO.h>
#include <dpl/ace/Attribute.h>
#include <dpl/ace/Preference.h>
#include <dpl/ace/SettingsLogic.h>

#define TESTSUITE06(n) \
RUNNER_TEST(ts06_ace_dao_ ## n)

#define CLEANENV \
    do{                                              \
        AceDB::AceDAO::clearWidgetDevCapSettings();  \
        AceDB::AceDAO::clearDevCapSettings();        \
    }while(0)

using namespace AceDB;

int operator==(const Attribute& left, const Attribute& right)
{
   return ( (*left.getName() == *right.getName()));
}

bool UserSettingsAgreed(std::list<PermissionTriple>* expected,
                        std::list<PermissionTriple>* allRules)
{
    if (allRules->empty() && expected->empty())
    {
        return true;
    }

    std::list<PermissionTriple>::iterator itA;
    FOREACH(itE, *expected)
    {
        if (itE->devCap.empty()){
            return false;  //some error occur
        }

        for (itA = allRules->begin(); itA !=  allRules->end(); ++itA){
            if (itA->appId == itE->appId &&
                itA->devCap == itE->devCap)
            {
                if (itA->access == itE->access){
                    break;
                }
                else{
                    return false; //optimization
                }
            }
        }

        if (itA == allRules->end())
        {
            if (itE->access == Preference::PREFERENCE_DEFAULT)
            {  // this value is not set in DB
                continue;
            }
            //if we are here -it means that expected values doesnt exist in DB
            return false;
        }
        else
        {
            continue;   //go to next expected attr
        }
    }

    //it means that all required values exist in DB
    return true;
}


bool UserSettingNotStored(std::list<PermissionTriple>* userSettings,
                          std::string res,
                          WidgetHandle handler,
                          Preference pref)
{
    if (userSettings->empty())
    {
        return true;
    }

    FOREACH(iter, *userSettings)
    {
        if (iter->appId == handler && iter->devCap == res){
            if (iter->access == pref){
                return false;  //this value has been saved
            }
            else{
                continue;  //value has been saved
            }
        }
    }

    return true;
}



bool ResourceSettingsAgreed(
        std::list< std::pair<const std::string*,Preference> > *expected,
        std::map<std::string, Preference>* allRules)
{
    std::list< std::pair<const std::string*,Preference> >::iterator itE;
    std::map<std::string, Preference>::iterator itA;

    FOREACH(itE, *expected)
    {
        itA = allRules->find(*(itE->first));
        if (itA != allRules->end() && itA->second == itE->second)
        {
            continue;
        }
        else
        {
            return false;
        }
    }

    return true;
}

bool ResourceSettingsEqual(
        std::list< std::pair<const std::string*,Preference> > * actual,
        std::map<std::string, Preference>* expected)
{
    bool flag = false;
    FOREACH(iterA, *actual)
    {
        FOREACH(iterE, *expected)
        {
            if (*(iterA->first) == iterE->first &&
                iterA->second == iterE->second)
            {
                flag = true;
            }
        }
        if (!flag)
        {
            return false;
        }
        flag = false;
    }

    return true;
}

//bool VSPAssertEqual(Verdict actual, Verdict expected){
//    return (actual == expected);
//}


bool AttributeEqual(AttributeSet  actual, AttributeSet expected){
    return (actual == expected);
}

bool ResourceSettingEqual(std::map<std::string, Preference>*  rSettings,
                          std::string res,
                          Preference pref)
{
    std::map<std::string, Preference>::iterator iter = rSettings->find(res);

    if (iter != rSettings->end() && iter->second ==pref)
    {
        return true;
    }

    return false;
}


bool  UserSettingEqual(std::list<PermissionTriple>* userSettings,
                       std::string res,
                       WidgetHandle handler,
                       Preference pref)
{
        if (userSettings->empty() && pref == Preference::PREFERENCE_DEFAULT)
        {
            return true;
        }
        FOREACH(iter, *userSettings)
        {
            if (iter->devCap.empty())
            {
                return false; //some error occurred
            }
            if (iter->appId == handler && iter->devCap == res){
                if (iter->access == pref)
                {
                    return true;
                }
                else
                {
                    return false;
                }
            }
        }

    return false;
}

//Test does  verdict PERMIT is saved when validity is once
TESTSUITE06(01){
    CLEANENV;
    AttributeSet attributeSet;
    std::string tmp("atrS");
    std::string tmpValue("Buu");
    BaseAttributePtr atr(new Attribute(&tmp,
                                       Attribute::Match::Equal,
                                       Attribute::Type::Subject));
    DPL::StaticPointerCast<Attribute>(atr)->addValue(&tmpValue);
    attributeSet.insert(atr);

    AceDAO::setPolicyResult(attributeSet, PolicyEffect::PERMIT);
    OptionalPolicyResult opt = AceDAO::getPolicyResult(attributeSet);

    PolicyResult effect(PolicyEffect::DENY);
    if (!opt.IsNull()) {
        effect = *opt;
    }

    RUNNER_ASSERT(effect == PolicyEffect::PERMIT);
}

TESTSUITE06(02){
    CLEANENV;
    AttributeSet attributeSet;
    std::string tmp("atrS");
    std::string tmpValue("Buu");
    BaseAttributePtr atr(new Attribute(&tmp,
                                       Attribute::Match::Equal,
                                       Attribute::Type::Subject));
    DPL::StaticPointerCast<Attribute>(atr)->addValue(&tmpValue);
    attributeSet.insert(atr);

    AceDAO::setPolicyResult(attributeSet, PolicyEffect::DENY);
    OptionalPolicyResult opt = AceDAO::getPolicyResult(attributeSet);

    PolicyResult effect(PolicyEffect::PERMIT);
    if (!opt.IsNull()) {
        effect = *opt;
    }

    RUNNER_ASSERT(effect == PolicyEffect::DENY);
}

TESTSUITE06(03){
    CLEANENV;
    AttributeSet attributeSet;
    std::string tmp("atrS");
    std::string tmpValue("Buu");
    BaseAttributePtr atr(new Attribute(&tmp,
                                       Attribute::Match::Equal,
                                       Attribute::Type::Subject));
    DPL::StaticPointerCast<Attribute>(atr)->addValue(&tmpValue);
    attributeSet.insert(atr);

    AceDAO::setPolicyResult(attributeSet, PolicyEffect::PROMPT_ONESHOT);
    OptionalPolicyResult opt = AceDAO::getPolicyResult(attributeSet);

    PolicyResult effect(PolicyEffect::DENY);
    if (!opt.IsNull()) {
        effect = *opt;
    }

    RUNNER_ASSERT(effect == PolicyEffect::PROMPT_ONESHOT);
}

TESTSUITE06(04){
    CLEANENV;
    AttributeSet attributeSet;
    std::string tmp("atrS");
    std::string tmpValue("Buu");
    BaseAttributePtr atr(new Attribute(&tmp,
                                       Attribute::Match::Equal,
                                       Attribute::Type::Subject));
    DPL::StaticPointerCast<Attribute>(atr)->addValue(&tmpValue);
    attributeSet.insert(atr);

    AceDAO::setPolicyResult(attributeSet, PolicyEffect::PROMPT_SESSION);
    OptionalPolicyResult opt = AceDAO::getPolicyResult(attributeSet);

    PolicyResult effect(PolicyEffect::DENY);
    if (!opt.IsNull()) {
        effect = *opt;
    }

    RUNNER_ASSERT(effect == PolicyEffect::PROMPT_SESSION);
}

TESTSUITE06(05){
    CLEANENV;
    AttributeSet attributeSet;
    std::string tmp("atrS");
    std::string tmpValue("Buu");
    BaseAttributePtr atr(new Attribute(&tmp,
                                       Attribute::Match::Equal,
                                       Attribute::Type::Subject));
    DPL::StaticPointerCast<Attribute>(atr)->addValue(&tmpValue);
    attributeSet.insert(atr);

    AceDAO::setPolicyResult(attributeSet, PolicyEffect::PROMPT_BLANKET);
    OptionalPolicyResult opt = AceDAO::getPolicyResult(attributeSet);

    PolicyResult effect(PolicyEffect::DENY);
    if (!opt.IsNull()) {
        effect = *opt;
    }

    RUNNER_ASSERT(effect == PolicyEffect::PROMPT_BLANKET);
}

TESTSUITE06(06){
    CLEANENV;
    AttributeSet attributeSet;
    std::string tmp("atrS");
    std::string tmpValue("Buu");
    BaseAttributePtr atr(new Attribute(&tmp,
                                       Attribute::Match::Equal,
                                       Attribute::Type::Subject));
    DPL::StaticPointerCast<Attribute>(atr)->addValue(&tmpValue);
    attributeSet.insert(atr);

    AceDAO::setPolicyResult(attributeSet, PolicyDecision::NOT_APPLICABLE);
    OptionalPolicyResult opt = AceDAO::getPolicyResult(attributeSet);

    PolicyResult effect(PolicyEffect::DENY);
    if (!opt.IsNull()) {
        effect = *opt;
    }

    RUNNER_ASSERT(effect == PolicyDecision::NOT_APPLICABLE);
}

TESTSUITE06(07){
    CLEANENV;
    AttributeSet attributeSet;
    std::string tmp("atrS");
    std::string tmpValue("Buu");
    BaseAttributePtr atr(new Attribute(&tmp,
                                       Attribute::Match::Equal,
                                       Attribute::Type::Subject));
    DPL::StaticPointerCast<Attribute>(atr)->addValue(&tmpValue);
    attributeSet.insert(atr);

    AceDAO::setPolicyResult(attributeSet, PolicyResult::UNDETERMINED);
    OptionalPolicyResult opt = AceDAO::getPolicyResult(attributeSet);

    PolicyResult effect(PolicyEffect::DENY);
    if (!opt.IsNull()) {
        effect = *opt;
    }

    RUNNER_ASSERT(effect == PolicyResult::UNDETERMINED);
}

TESTSUITE06(08){
    CLEANENV;
    AttributeSet attributeSet;
    std::string tmp("atrS2");
    std::string tmpValue("Buu2");
    BaseAttributePtr atr(new Attribute(&tmp,
                                       Attribute::Match::Equal,
                                       Attribute::Type::Subject));
    DPL::StaticPointerCast<Attribute>(atr)->addValue(&tmpValue);
    attributeSet.insert(atr);

    std::string session = "jlakjhqrwebn234324987";
    std::string param = "xxx";

    AceDAO::setPromptDecision(attributeSet,
                              DPL::FromUTF8String(param),
                              DPL::FromUTF8String(session),
                              PromptDecision::ALLOW_ALWAYS);

    OptionalCachedPromptDecision decision =
        AceDAO::getPromptDecision(attributeSet, param);

    DPL::String session1;
    PromptDecision dec = PromptDecision::DENY_FOR_SESSION;
    if (!decision.IsNull()){
        dec = (*decision).decision;
        if (!decision.IsNull()) {
            session1 = *((*decision).session);
        }
    }
    RUNNER_ASSERT(dec == PromptDecision::ALLOW_ALWAYS);
    RUNNER_ASSERT(DPL::ToUTF8String(session1) == session);
}

TESTSUITE06(09){
    CLEANENV;
    AttributeSet attributeSet;
    std::string tmp("atrS2");
    std::string tmpValue("Buu2");
    BaseAttributePtr atr(new Attribute(&tmp,
                                       Attribute::Match::Equal,
                                       Attribute::Type::Subject));
    DPL::StaticPointerCast<Attribute>(atr)->addValue(&tmpValue);
    attributeSet.insert(atr);

    std::string param = "xxx";

    AceDAO::setPromptDecision(attributeSet,
                              DPL::FromUTF8String(param),
                              DPL::OptionalString(),
                              PromptDecision::ALLOW_FOR_SESSION);

    OptionalCachedPromptDecision decision =
        AceDAO::getPromptDecision(attributeSet, param);

    std::string session1;
    PromptDecision dec = PromptDecision::DENY_FOR_SESSION;
    if (!decision.IsNull()){
        dec = (*decision).decision;
        RUNNER_ASSERT((*decision).session.IsNull());
    }
    RUNNER_ASSERT(dec == PromptDecision::ALLOW_FOR_SESSION);
}

///*findVerdict*/
//
//void AceDAOTester::test3()
//{
//// Tests does verdict is stored for validity always
//    Request  request("subject", "resource");
//    AttributeSet attributeSet;
//    std::string tmp("atrS3");
//    std::string tmpValue("Buu3");
//    BaseAttributePtr atr(
//        new Attribute(&tmp, Attribute::Match::Equal, Attribute::Type::Subject));
//    DPL::StaticPointerCast<Attribute>(atr)->addValue(&tmpValue);
//    attributeSet.insert(atr);
//    Verdict ver = Verdict::VERDICT_PERMIT;
//    Validity val = Validity::ALWAYS;
//    std::string session = getSession();
//    VerdictLogic::addVerdict(request, session, attributeSet, ver, val);
//    Verdict verdict = VerdictLogic::findVerdict(request, session, attributeSet);
//    bool result = VSPAssertEqual(verdict, Verdict::VERDICT_PERMIT);
//    handleResult(result, "find Verdict test 3");
//}
///*findVerdict*/
//
//void AceDAOTester::test4()
//{
////tests does verdict is propertly stored (NOT saved)when session is Validity::ONCE
////and verdict is DENY
//    Request  request("subject", "resource");
//    AttributeSet attributeSet;
//    std::string tmp("atrS4");
//    std::string tmpValue("Buu4");
//    BaseAttributePtr atr(
//        new Attribute(&tmp, Attribute::Match::Equal, Attribute::Type::Subject));
//    DPL::StaticPointerCast<Attribute>(atr)->addValue(&tmpValue);
//    attributeSet.insert(atr);
//    Verdict ver = Verdict::VERDICT_DENY;
//    Validity val = Validity::ONCE;
//    std::string session = getSession();
//    VerdictLogic::addVerdict(request, session, attributeSet, ver, val);
//    Verdict verdict = VerdictLogic::findVerdict(request, session, attributeSet);
//    bool result = VSPAssertEqual(verdict, Verdict::VERDICT_UNKNOWN);
//    handleResult(result, "find Verdict test 4");
//}
///*findVerdict*/
//
//void AceDAOTester::test5()
//{
////tests does verdict is properly set when validity is session
////and verdict is deny
//    Request  request("subject", "resource");
//    AttributeSet attributeSet;
//    std::string tmp("atrS5");
//    std::string tmpValue("Buu5");
//    BaseAttributePtr atr(
//        new Attribute(&tmp, Attribute::Match::Equal, Attribute::Type::Subject));
//    DPL::StaticPointerCast<Attribute>(atr)->addValue(&tmpValue);
//    attributeSet.insert(atr);
//    Verdict ver = Verdict::VERDICT_DENY;
//    Validity val = Validity::SESSION;
//    pip.setSessionId("5");
//    std::string session = getSession();
//    VerdictLogic::addVerdict(request, session, attributeSet, ver, val);
//    pip.setSessionId("5");
//    session = getSession();
//    Verdict verdict = VerdictLogic::findVerdict(request, session, attributeSet);
//    bool result = VSPAssertEqual(verdict, Verdict::VERDICT_DENY);
//    handleResult(result, "find Verdict test 5");
//}
///*findVerdict*/
//
//void AceDAOTester::test6()
//{
////tests does verdict is properly stored for DENY Validity::ALWAYS
//    Request  request("subject", "resource");
//    AttributeSet attributeSet;
//    std::string tmp("atrS6");
//    std::string tmpValue("Buu6");
//    BaseAttributePtr atr(
//        new Attribute(&tmp, Attribute::Match::Equal, Attribute::Type::Subject));
//    DPL::StaticPointerCast<Attribute>(atr)->addValue(&tmpValue);
//    attributeSet.insert(atr);
//    Verdict ver = Verdict::VERDICT_DENY;
//    Validity val = Validity::ALWAYS;
//    std::string session = getSession();
//    VerdictLogic::addVerdict(request, session, attributeSet, ver, val);
//    Verdict verdict = VerdictLogic::findVerdict(request, session, attributeSet);
//    bool result = VSPAssertEqual(verdict, Verdict::VERDICT_DENY);
//    handleResult(result, "find Verdict test 6");
//}
//
//
//void AceDAOTester::test7()
//{
////tests stored version fo INAPPLICABLE Validity::ONCE
//
//    Request  request("subject", "resource");
//    AttributeSet attributeSet;
//    attributeSet.insert(constructAttribute("atrS7","Buu7"));
//    Verdict ver = Verdict::VERDICT_INAPPLICABLE;
//    Validity val = Validity::ONCE;
//    std::string session = getSession();
//    VerdictLogic::addVerdict(request, session, attributeSet, ver, val);
//    Verdict verdict = VerdictLogic::findVerdict(request, session, attributeSet);
//    //Verdict with 'Once' validity should not be stored, therefore
//    //the outcome should be UNKNOWN
//    bool result = VSPAssertEqual(verdict, Verdict::VERDICT_UNKNOWN);
//    handleResult(result, "find Verdict test 7");
//}
//
//
//void AceDAOTester::test8()
//{
////tests storing data for verdict INAPLICABLE and Validity::SESSION
//
//    Request  request("subject", "resource");
//    AttributeSet attributeSet;
//    attributeSet.insert(constructAttribute("atrS8","Buuu8"));
//    Verdict ver = Verdict::VERDICT_INAPPLICABLE;
//    Validity val = Validity::SESSION;
//    pip.setSessionId("8");
//    std::string session = getSession();
//    VerdictLogic::addVerdict(request, session, attributeSet, ver, val);
//    pip.setSessionId("9");
//    session = getSession();
//    Verdict verdict = VerdictLogic::findVerdict(request, session, attributeSet);
//    bool result = VSPAssertEqual(verdict, Verdict::VERDICT_UNKNOWN);
//    handleResult(result, "find Verdict test 8");
//}
///*findVerdict*/
//
//void AceDAOTester::test9()
//{
//    Request  request("subject", "resource");
//    AttributeSet attributeSet;
//    std::string tmp("atrS9");
//    std::string tmpValue("Buu9");
//    BaseAttributePtr atr(
//        new Attribute(&tmp, Attribute::Match::Equal, Attribute::Type::Subject));
//    DPL::StaticPointerCast<Attribute>(atr)->addValue(&tmpValue);
//    attributeSet.insert(atr);
//    Verdict ver = Verdict::VERDICT_INAPPLICABLE;
//    Validity val = Validity::ALWAYS;
//    std::string session = getSession();
//    VerdictLogic::addVerdict(request, session, attributeSet, ver, val);
//    Verdict verdict = VerdictLogic::findVerdict(request, session, attributeSet);
//    bool result = VSPAssertEqual(verdict, Verdict::VERDICT_INAPPLICABLE);
//    handleResult(result, "find Verdict test 9");
//}
///*findVerdict*/
//
//void AceDAOTester::test10()
//{
////tests does verdict is properly stored for verdict undetermined
////and validity Validity::ONCE - thats why verdict is UNKNOWN
//    Request  request("subject", "resource");
//    AttributeSet attributeSet;
//    std::string tmp("atrS10");
//    std::string tmpValue("Buu10");
//    BaseAttributePtr atr(
//        new Attribute(&tmp, Attribute::Match::Equal, Attribute::Type::Subject));
//    DPL::StaticPointerCast<Attribute>(atr)->addValue(&tmpValue);
//    attributeSet.insert(atr);
//    Verdict ver = Verdict::VERDICT_UNDETERMINED;
//    Validity val = Validity::ONCE;
//    std::string session = getSession();
//    VerdictLogic::addVerdict(request, session, attributeSet, ver, val);
//    Verdict verdict = VerdictLogic::findVerdict(request, session, attributeSet);
//    bool result = VSPAssertEqual(verdict, Verdict::VERDICT_UNKNOWN);
//    handleResult(result, "find Verdict test 10");
//}
///*findVerdict*/
//
//void AceDAOTester::test11()
//{
////tests does verdict is properly stored for verdict undetermined
////and validity Validity::SESSION - TODO Verdicts for undetermined are not stored??
//    Request  request("subject", "resource");
//    AttributeSet attributeSet;
//    std::string tmp("atrS11");
//    std::string tmpValue("Buu11");
//    BaseAttributePtr atr(
//        new Attribute(&tmp, Attribute::Match::Equal, Attribute::Type::Subject));
//    DPL::StaticPointerCast<Attribute>(atr)->addValue(&tmpValue);
//    attributeSet.insert(atr);
//    Verdict ver = Verdict::VERDICT_UNDETERMINED;
//    pip.setSessionId("8");
//    Validity val = Validity::SESSION;
//    pip.setSessionId("8");
//    std::string session = getSession();
//    VerdictLogic::addVerdict(request, session, attributeSet, ver, val);
//    Verdict verdict = VerdictLogic::findVerdict(request, session, attributeSet);
//    bool result = VSPAssertEqual(verdict, Verdict::VERDICT_UNKNOWN);
//    handleResult(result, "find Verdict test 11");
//}
///*findVerdict*/
//
//void AceDAOTester::test12()
//{
////tests does verdict is properly stored for verdict undetermined
////and validity Validity::ALWAYS - TODO Verdicts for undetermined are not stored??
//    Request  request("subject", "resource");
//    AttributeSet attributeSet;
//    std::string tmp("atrS12");
//    std::string tmpValue("Buu12");
//    BaseAttributePtr atr(
//        new Attribute(&tmp, Attribute::Match::Equal, Attribute::Type::Subject));
//    DPL::StaticPointerCast<Attribute>(atr)->addValue(&tmpValue);
//    attributeSet.insert(atr);
//    Verdict ver = Verdict::VERDICT_UNDETERMINED;
//    Validity val = Validity::ALWAYS;
//    std::string session = getSession();
//    VerdictLogic::addVerdict(request, session, attributeSet, ver, val);
//    Verdict verdict = VerdictLogic::findVerdict(request, session, attributeSet);
//    bool result = VSPAssertEqual(verdict, Verdict::VERDICT_UNKNOWN);
//    handleResult(result, "find Verdict test 12");
//}
///*findVerdict*/
//
//void AceDAOTester::test13()
//{
////tests does verdict is properly stored for verdict unknown
////and validity Validity::ONCE-thats why  verdict is not stored
//    Request  request("subject", "resource");
//    AttributeSet attributeSet;
//    std::string tmp("atrS13");
//    std::string tmpValue("Buu13");
//    BaseAttributePtr atr(
//        new Attribute(&tmp, Attribute::Match::Equal, Attribute::Type::Subject));
//    DPL::StaticPointerCast<Attribute>(atr)->addValue(&tmpValue);
//    attributeSet.insert(atr);
//    Verdict ver = Verdict::VERDICT_UNKNOWN;
//    Validity val = Validity::ONCE;
//    std::string session = getSession();
//    VerdictLogic::addVerdict(request, session, attributeSet, ver, val);
//    Verdict verdict = VerdictLogic::findVerdict(request, session, attributeSet);
//    bool result = VSPAssertEqual(verdict, Verdict::VERDICT_UNKNOWN);
//    handleResult(result, "find Verdict test 13");
//}
///*findVerdict*/
//
//void AceDAOTester::test14()
//{
////tests does verdict is properly stored for verdict unknown
////and validity session- TODO why is the verdict not stored?
//    Request  request("subject", "resource");
//    AttributeSet attributeSet;
//    std::string tmp("atrS14");
//    std::string tmpValue("Buu14");
//    BaseAttributePtr atr(
//        new Attribute(&tmp, Attribute::Match::Equal, Attribute::Type::Subject));
//    DPL::StaticPointerCast<Attribute>(atr)->addValue(&tmpValue);
//    attributeSet.insert(atr);
//    Verdict ver = Verdict::VERDICT_UNKNOWN;
//    Validity val = Validity::SESSION;
//    pip.setSessionId("8");
//    std::string session = getSession();
//    VerdictLogic::addVerdict(request, session, attributeSet, ver, val);
//    pip.setSessionId("8");
//    session = getSession();
//    Verdict verdict = VerdictLogic::findVerdict(request, session, attributeSet);
//    bool result = VSPAssertEqual(verdict, Verdict::VERDICT_UNKNOWN);
//    handleResult(result, "find Verdict test 14");
//}
///*findVerdict*/
//
//void AceDAOTester::test15()
//{
////tests does verdict is properly stored for verdict unknown and
////validity Validity::ALWAYS - TODO should we save Unknown verdict?
//    Request  request("subject", "resource");
//    AttributeSet attributeSet;
//    std::string tmp("atrS15");
//    std::string tmpValue("Buu15");
//    BaseAttributePtr atr(
//        new Attribute(&tmp, Attribute::Match::Equal, Attribute::Type::Subject));
//    DPL::StaticPointerCast<Attribute>(atr)->addValue(&tmpValue);
//    attributeSet.insert(atr);
//    Verdict ver = Verdict::VERDICT_UNKNOWN;
//    Validity val = Validity::ALWAYS;
//    std::string session = getSession();
//    VerdictLogic::addVerdict(request, session, attributeSet, ver, val);
//    Verdict verdict = VerdictLogic::findVerdict(request, session, attributeSet);
//    bool result = VSPAssertEqual(verdict, Verdict::VERDICT_UNKNOWN);
//    handleResult(result, "find Verdict test 15");
//}
//
///*removeVerdict*/         //tests does verdict is properly removed
//void AceDAOTester::test16()
//{
//    Request  request("subject", "resource");
//    AttributeSet attributeSet;
//    std::string tmp("atrS15");
//    std::string tmpValue("Buu15");
//    BaseAttributePtr atr(
//        new Attribute(&tmp, Attribute::Match::Equal, Attribute::Type::Subject));
//    DPL::StaticPointerCast<Attribute>(atr)->addValue(&tmpValue);
//    attributeSet.insert(atr);
//
//    Verdict ver = Verdict::VERDICT_PERMIT;
//    Validity val = Validity::ALWAYS;
//    //add verdict
//    std::string session = getSession();
//    VerdictLogic::addVerdict(request, session, attributeSet, ver, val);
//    //check has verdict been stored?
//    Verdict verdict = VerdictLogic::findVerdict(request, session, attributeSet);
//    bool result = VSPAssertEqual(verdict, Verdict::VERDICT_PERMIT);
//    handleResult(result, "find Verdict test 16: partA");
//
//    //remove verdict
//    VerdictLogic::removeVerdict(request, attributeSet);
//    //verified removed
//    verdict = VerdictLogic::findVerdict(request, session, attributeSet);
//    result = VSPAssertEqual(verdict, Verdict::VERDICT_UNKNOWN);
//    handleResult(result, "find Verdict test 16: partB");
//}
//
///*findVerdict*/
//void AceDAOTester::test18()
//{
////verdict for 3 elements on the set of attributes
//    Request  request("subject18","resource");
//    AttributeSet attributeSet;
//    std::string tmp("atrS60");
//    std::string tmpValue("Buu60");
//    BaseAttributePtr atr(
//        new Attribute(&tmp, Attribute::Match::Equal, Attribute::Type::Subject));
//    DPL::StaticPointerCast<Attribute>(atr)->addValue(&tmpValue);
//    attributeSet.insert(atr);
//
//    std::string tmp1("atrS61");
//    std::string tmpValue1("Buu61");
//    BaseAttributePtr atr1(
//        new Attribute(&tmp1,Attribute::Match::Equal, Attribute::Type::Subject));
//    DPL::StaticPointerCast<Attribute>(atr1)->addValue(&tmpValue1);
//    attributeSet.insert(atr1);
//
//    std::string tmp2("atrS62");
//    std::string tmpValue2("Buu62");
//    BaseAttributePtr atr2(
//        new Attribute(&tmp2,Attribute::Match::Equal, Attribute::Type::Subject));
//    DPL::StaticPointerCast<Attribute>(atr2)->addValue(&tmpValue2);
//    attributeSet.insert(atr2);
//
//    Verdict ver = Verdict::VERDICT_DENY;
//    Validity val = Validity::ALWAYS;
//    std::string session = getSession();
//    VerdictLogic::addVerdict(request, session, attributeSet, ver, val);
//    Verdict verdict = VerdictLogic::findVerdict(request, session, attributeSet);
//    bool result = VSPAssertEqual(verdict, Verdict::VERDICT_DENY);
//    handleResult(result, "find Verdict test 18");
//}
//
///*resetDatabase*/
//void AceDAOTester::test19()
//{
//    //tests reset DB
//    Request  request("subject", "resource");
//    AttributeSet attributeSet;
//    std::string tmp("atrS15");
//    std::string tmpValue("Buu15");
//    BaseAttributePtr atr(
//        new Attribute(&tmp, Attribute::Match::Equal, Attribute::Type::Subject));
//    DPL::StaticPointerCast<Attribute>(atr)->addValue(&tmpValue);
//    attributeSet.insert(atr);
//
//    Verdict ver = Verdict::VERDICT_PERMIT;
//    Validity val = Validity::ALWAYS;
//    //add verdict
//    std::string session = getSession();
//    VerdictLogic::addVerdict(request, session, attributeSet, ver, val);
//    //check has verdict been stored?
//    Verdict verdict = VerdictLogic::findVerdict(request, session, attributeSet);
//    bool result = VSPAssertEqual(verdict, Verdict::VERDICT_PERMIT);
//    handleResult(result, "reset database test 19: partA");
//
//    AceDAO::resetDatabase();
//
//    verdict = VerdictLogic::findVerdict(request, session, attributeSet);
//    result = VSPAssertEqual(verdict, Verdict::VERDICT_UNKNOWN);
//    handleResult(result, "reset database test 19: partB");
//}


/*setWidgetDevCapSetting*/

TESTSUITE06(26){
    CLEANENV;
    std::string res("res20");
    WidgetHandle sub = 20;

    SettingsLogic::setWidgetDevCapSetting(res, sub, Preference::PREFERENCE_PERMIT);
    std::list<PermissionTriple> userSettings;
    SettingsLogic::getWidgetDevCapSettings(&userSettings);
    RUNNER_ASSERT(
        UserSettingEqual(
            &userSettings, res, sub, Preference::PREFERENCE_PERMIT));
}

/*setWidgetDevCapSetting*/

TESTSUITE06(27){
    CLEANENV;
    std::string res("res21");
    WidgetHandle sub = 21;

    SettingsLogic::setWidgetDevCapSetting(res, sub, Preference::PREFERENCE_DENY);
    std::list<PermissionTriple> userSettings;
    SettingsLogic::getWidgetDevCapSettings(&userSettings);
    RUNNER_ASSERT(
        UserSettingEqual(
            &userSettings, res, sub, Preference::PREFERENCE_DENY));
}

/*setWidgetDevCapSetting*/

TESTSUITE06(28){
    CLEANENV;
    std::string res("res22");
    WidgetHandle sub = 22;
    AceDAO::clearAllSettings();
    SettingsLogic::setWidgetDevCapSetting(res, sub, Preference::PREFERENCE_DEFAULT);
    std::list<PermissionTriple> userSettings;
    SettingsLogic::getWidgetDevCapSettings(&userSettings);
    RUNNER_ASSERT(
        UserSettingEqual(
            &userSettings, res, sub, Preference::PREFERENCE_DEFAULT));
}

TESTSUITE06(29){
    CLEANENV;
    std::string res("res23");
    WidgetHandle sub = 23;

    SettingsLogic::setWidgetDevCapSetting(res, sub, Preference::PREFERENCE_DEFAULT);
    SettingsLogic::setWidgetDevCapSetting(res, sub, Preference::PREFERENCE_DENY);
    std::list<PermissionTriple> userSettings;
    SettingsLogic::getWidgetDevCapSettings(&userSettings);
    RUNNER_ASSERT(
        UserSettingEqual(
            &userSettings, res, sub, Preference::PREFERENCE_DENY));
}

/*setWidgetDevCapSettings*/

TESTSUITE06(30){
    CLEANENV;
    std::string resa("res24a");
    WidgetHandle suba = 241;
    std::string resb("res24b");
    WidgetHandle subb = 242;

    std::list<PermissionTriple> permissionsL;

    permissionsL.push_back(
        PermissionTriple(suba, resa, Preference::PREFERENCE_DENY));
    permissionsL.push_back(
        PermissionTriple(subb, resb, Preference::PREFERENCE_PERMIT));

    SettingsLogic::setWidgetDevCapSettings(permissionsL);

    std::list<PermissionTriple> userSettings;
    SettingsLogic::getWidgetDevCapSettings(&userSettings);
    RUNNER_ASSERT(
        UserSettingsAgreed(&permissionsL, &userSettings));
}



TESTSUITE06(31){
    CLEANENV;
//multi set - if value is not set in DB or has DEFAUL value it is not
//send by getWidgetDevCapSettings - optimization

    std::string resa("res25a");
    WidgetHandle suba = 251;
    std::string resb("res25b");
    WidgetHandle subb = 252;
    std::string resc("res25c");
    WidgetHandle subc = 253;

    std::list<PermissionTriple> permissionsL;

    permissionsL.push_back(
        PermissionTriple(suba, resa, Preference::PREFERENCE_DENY));
    permissionsL.push_back(
        PermissionTriple(subb, resb, Preference::PREFERENCE_PERMIT));
    permissionsL.push_back(
        PermissionTriple(subc, resc, Preference::PREFERENCE_DEFAULT));


    SettingsLogic::setWidgetDevCapSettings(permissionsL);

    std::list<PermissionTriple> userSettings;
    SettingsLogic::getWidgetDevCapSettings(&userSettings);
    RUNNER_ASSERT(UserSettingsAgreed(&permissionsL, &userSettings));
}

TESTSUITE06(32){
    CLEANENV;
    //empty list  -- TODO what is it testing?
    std::list<PermissionTriple> permissionsL;

    std::list<PermissionTriple> userSettings;
    SettingsLogic::getWidgetDevCapSettings(&userSettings);
    RUNNER_ASSERT(UserSettingsAgreed(&permissionsL, &userSettings));
}

TESTSUITE06(33){
    CLEANENV;
    //test resource setting PERMIT
    std::string res("res27");
    SettingsLogic::setDevCapSetting(res, Preference::PREFERENCE_PERMIT);
    std::map<std::string, Preference> resourceSettings;
    SettingsLogic::getDevCapSettings(&resourceSettings);

    RUNNER_ASSERT(
         ResourceSettingEqual(&resourceSettings,
                              res,
                              Preference::PREFERENCE_PERMIT));
}

TESTSUITE06(34){
    CLEANENV;
    //test resource setting DENY
    std::string res("res28");
    SettingsLogic::setDevCapSetting(res, Preference::PREFERENCE_DENY);
    std::map<std::string, Preference> resourceSettings;
    SettingsLogic::getDevCapSettings(&resourceSettings);

    RUNNER_ASSERT(
        ResourceSettingEqual(&resourceSettings,
                             res,
                             Preference::PREFERENCE_DENY));
}

TESTSUITE06(35){
    CLEANENV;
    //test resource setting Preference::PREFERENCE_DEFAULT

    std::string res("res29");
    SettingsLogic::setDevCapSetting(res, Preference::PREFERENCE_DEFAULT);
    std::map<std::string, Preference> resourceSettings;
    SettingsLogic::getDevCapSettings(&resourceSettings);

    RUNNER_ASSERT(
        ResourceSettingEqual(
            &resourceSettings,
            res,
            Preference::PREFERENCE_DEFAULT));
}

TESTSUITE06(36){
    CLEANENV;
    //multi set for Resource settings

    std::string resa("res30a");
    std::string resb("res30b");

    std::list< std::pair<const std::string*,Preference> >* resourcesL =
        new std::list< std::pair<const std::string*, Preference> >();

    resourcesL->push_back(make_pair(&resa, Preference::PREFERENCE_DENY));
    resourcesL->push_back(make_pair(&resb, Preference::PREFERENCE_PERMIT));

    SettingsLogic::setAllDevCapSettings (*resourcesL);

    std::map<std::string, Preference> resourceSettings;
    SettingsLogic::getDevCapSettings(&resourceSettings);
    RUNNER_ASSERT(ResourceSettingsAgreed(resourcesL, &resourceSettings));
    delete resourcesL;
}


//void AceDAOTester::test31()
//{
//    // session has changed (PERMIT).
//    //What is the verdict now?
//
//    Request  request("subject", "resource");
//    AttributeSet attributeSet;
//    std::string tmp("atrS31");
//    std::string tmpValue("Buu31");
//    BaseAttributePtr atr(
//        new Attribute(&tmp, Attribute::Match::Equal, Attribute::Type::Subject));
//    DPL::StaticPointerCast<Attribute>(atr)->addValue(&tmpValue);
//    attributeSet.insert(atr);
//    Verdict ver = Verdict::VERDICT_PERMIT;
//    Validity val = Validity::SESSION;
//    pip.setSessionId("aaa");
//
//    std::string session = getSession();
//    VerdictLogic::addVerdict(request, session, attributeSet, ver, val);
//
//    Verdict verdictA =
//        VerdictLogic::findVerdict(request, session, attributeSet);
//    bool resultA = VSPAssertEqual(verdictA, Verdict::VERDICT_PERMIT);
//    handleResult(resultA, "Verdict session test 31 session a");
//
//    pip.setSessionId("bbb");
//    session = getSession();
//    Verdict verdictB =
//        VerdictLogic::findVerdict(request, session, attributeSet);
//    bool resultB = VSPAssertEqual(verdictB, Verdict::VERDICT_UNKNOWN);
//    handleResult(resultB, "Verdict session test 31 session b");
//}


//void AceDAOTester::test32()
//{
//    //session has changed (DENY).
//    //what is the verdict?
//
//    Request  request("subject", "resource");
//    AttributeSet attributeSet;
//    std::string tmp("atrS32");
//    std::string tmpValue("Buu32");
//    BaseAttributePtr atr(
//        new Attribute(&tmp, Attribute::Match::Equal, Attribute::Type::Subject));
//    DPL::StaticPointerCast<Attribute>(atr)->addValue(&tmpValue);
//    attributeSet.insert(atr);
//    Verdict ver = Verdict::VERDICT_DENY;
//    Validity val = Validity::SESSION;
//    pip.setSessionId("aaa");
//    std::string session = getSession();
//    VerdictLogic::addVerdict(request, session, attributeSet, ver, val);
//
//    Verdict verdictA =
//        VerdictLogic::findVerdict(request, session, attributeSet);
//    bool resultA = VSPAssertEqual(verdictA, Verdict::VERDICT_DENY);
//    handleResult(resultA, "Verdict session test 32 session a");
//
//    pip.setSessionId("ccc");
//
//    session = getSession();
//    Verdict verdictB =
//        VerdictLogic::findVerdict(request, session, attributeSet);
//    bool resultB = VSPAssertEqual(verdictB, Verdict::VERDICT_UNKNOWN);
//    handleResult(resultB, "Verdict session test 32 session b");
//}

//void AceDAOTester::test33(){
//
//    Request  request("subject", "resource");
//    AttributeSet attributeSet;
//    std::string tmp("atrS33");
//    std::string tmpValue("Buu33");
//    BaseAttributePtr atr(new Attribute(&tmp, Attribute::Match::Equal, Attribute::Type::Subject));
//    DPL::StaticPointerCast<Attribute>(atr)->addValue(&tmpValue);
//    attributeSet.insert(atr);
//    Verdict ver =  Verdict::VERDICT_INAPPLICABLE;
//    Validity val = Validity::SESSION;
//    pip.setSessionId("aaa");
//    std::string session = getSession();
//    VerdictLogic::addVerdict(request, session, attributeSet, ver, val);
//
//    Verdict verdictA = VerdictLogic::findVerdict(request, session, attributeSet);
//    bool resultA = VSPAssertEqual(verdictA, Verdict::VERDICT_INAPPLICABLE);
//    handleResult(resultA, "Verdict session test 33 session a");
//
//    pip.setSessionId("bbb");
//
//    session = getSession();
//    Verdict verdictB = VerdictLogic::findVerdict(request, session, attributeSet);
//    bool resultB = VSPAssertEqual(verdictB, Verdict::VERDICT_UNKNOWN);
//    handleResult(resultB, "Verdict session test 33 session b");
//}

//void AceDAOTester::test34(){ //session has changed (UNDETERMINED).
////what is the verdict?
//
//    Request  request("subject", "resource");
//    AttributeSet attributeSet;
//    std::string tmp("atrS34");
//    std::string tmpValue("Buu34");
//    BaseAttributePtr atr(new Attribute(&tmp, Attribute::Match::Equal, Attribute::Type::Subject));
//    DPL::StaticPointerCast<Attribute>(atr)->addValue(&tmpValue);
//    attributeSet.insert(atr);
//    Verdict ver =  Verdict::VERDICT_UNDETERMINED;
//    Validity val = Validity::SESSION;
//    pip.setSessionId("aaa");
//    std::string session  = getSession();
//    VerdictLogic::addVerdict(request, session, attributeSet, ver, val);
//
//    Verdict verdictA = VerdictLogic::findVerdict(request, session, attributeSet);
//    bool resultA = VSPAssertEqual(verdictA, Verdict::VERDICT_UNKNOWN);
//    handleResult(resultA, "Verdict session test 34 session a");
//
//    pip.setSessionId("bbb");
//
//    session = getSession();
//    Verdict verdictB = VerdictLogic::findVerdict(request, session, attributeSet);
//    bool resultB = VSPAssertEqual(verdictB, Verdict::VERDICT_UNKNOWN);
//    handleResult(resultB, "Verdict session test 34 session b");
//}

//void AceDAOTester::test35(){  //session has changed(UNKNOWN),
////what is the verdict?
//
//    Request  request("subject", "resource");
//    AttributeSet attributeSet;
//    std::string tmp("atrS35");
//    std::string tmpValue("Buu35");
//    BaseAttributePtr atr(new Attribute(&tmp, Attribute::Match::Equal, Attribute::Type::Subject));
//    DPL::StaticPointerCast<Attribute>(atr)->addValue(&tmpValue);
//    attributeSet.insert(atr);
//    Verdict ver =  Verdict::VERDICT_UNKNOWN;
//    Validity val = Validity::SESSION;
//    pip.setSessionId("aaa");
//    std::string session = getSession();
//    VerdictLogic::addVerdict(request, session, attributeSet, ver, val);
//
//    Verdict verdictA = VerdictLogic::findVerdict(request, session, attributeSet);
//    bool resultA = VSPAssertEqual(verdictA, Verdict::VERDICT_UNKNOWN);
//    handleResult(resultA, "Verdict session test 35 session a");
//    pip.setSessionId("bbb");
//
//    session = getSession();
//    Verdict verdictB = VerdictLogic::findVerdict(request, session,attributeSet);
//    bool resultB = VSPAssertEqual(verdictB, Verdict::VERDICT_UNKNOWN);
//    handleResult(resultB, "Verdict session test 35 session b");
//}

//void AceDAOTester::test36(){ //changed verdict in the same session
//
//    Request  request("subject", "resource");
//    AttributeSet attributeSet;
//    std::string tmp("atrS36");
//    std::string tmpValue("Buu36");
//    BaseAttributePtr atr(new Attribute(&tmp, Attribute::Match::Equal, Attribute::Type::Subject));
//    DPL::StaticPointerCast<Attribute>(atr)->addValue(&tmpValue);
//    attributeSet.insert(atr);
//    Verdict ver =  Verdict::VERDICT_DENY;
//    Validity val = Validity::SESSION;
//    pip.setSessionId("aaa");
//    std::string session = getSession();
//    VerdictLogic::addVerdict(request, session, attributeSet, ver, val);
//
//    Verdict verdictA = VerdictLogic::findVerdict(request, session, attributeSet);
//    bool resultA = VSPAssertEqual(verdictA ,Verdict::VERDICT_DENY);
//    handleResult(resultA, "Verdict session test 36 session a");
//
//    ver =  Verdict::VERDICT_PERMIT;
//    VerdictLogic::addVerdict(request, session, attributeSet, ver, val);
//
//    Verdict verdictB = VerdictLogic::findVerdict(request, session, attributeSet);
//    bool resultB = VSPAssertEqual(verdictB ,Verdict::VERDICT_PERMIT);
//    handleResult(resultB, "Verdict session test 36 session b");
//}
//
///* User settings crash tests */
//
//
//void AceDAOTester::test37(){ //empty attribute and verdict permit always
//    Request  request("subject", "resource");
//    AttributeSet attributeSet;
//
//    Verdict ver = Verdict::VERDICT_PERMIT;
//    Validity val = Validity::ALWAYS;
//
//    std::string session = getSession();
//    VerdictLogic::addVerdict(request, session, attributeSet, ver, val);
//
//    Verdict verdict = VerdictLogic::findVerdict(request, session, attributeSet);
//    bool result = VSPAssertEqual(verdict,Verdict::VERDICT_PERMIT);
//    handleResult(result, "find Verdict empty attribute set test 37 ");
//}
//
//void AceDAOTester::test38(){ //empty attribute and verdict deny always
//    Request  request("subject", "resource");
//    AttributeSet attributeSet;
//
//    Verdict ver = Verdict::VERDICT_DENY;
//    Validity val = Validity::ALWAYS;
//
//    std::string session = getSession();
//    VerdictLogic::addVerdict(request, session, attributeSet, ver, val);
//
//    Verdict verdict = VerdictLogic::findVerdict(request, session, attributeSet);
//    bool result = VSPAssertEqual(verdict,Verdict::VERDICT_DENY);
//    handleResult(result, "find Verdict empty attribute set test 38 ");
//}
//
//void AceDAOTester::test39(){
//
//    Request  request("subject", "resource");
//    AttributeSet attributeSet;
//
//    Verdict ver = Verdict::VERDICT_INAPPLICABLE;
//    Validity val = Validity::ALWAYS;
//
//    std::string session = getSession();
//    VerdictLogic::addVerdict(request, session, attributeSet, ver, val);
//
//    Verdict verdict = VerdictLogic::findVerdict(request, session, attributeSet);
//    bool result = VSPAssertEqual(verdict,Verdict::VERDICT_INAPPLICABLE);
//    handleResult(result, "find Verdict empty attribute set test 39 ");
//}
//
//void AceDAOTester::test40(){ //empty attribute and verdict unknown
//    Request  request("subject", "resource");
//    AttributeSet attributeSet;
//
//    Verdict ver = Verdict::VERDICT_UNKNOWN;
//    Validity val = Validity::ALWAYS;
//
//    std::string session = getSession();
//    VerdictLogic::addVerdict(request, session, attributeSet, ver, val);
//
//    Verdict verdict = VerdictLogic::findVerdict(request, session, attributeSet);
//    bool result = VSPAssertEqual(verdict,Verdict::VERDICT_UNKNOWN);
//    handleResult(result, "find Verdict empty attribute set test 40 ");
//}
//
//void AceDAOTester::test41()
//{
//    //empty string as subject and resource,
//    //without attributes and verdit DENY Validity::ALWAYS
//    Request  request("", "");
//    //TODO is it OK to store Verdict::VERDICT for empty request?
//    AttributeSet attributeSet;
//
//    Verdict ver = Verdict::VERDICT_DENY;
//    Validity val = Validity::ALWAYS;
//
//    std::string session = getSession();
//    VerdictLogic::addVerdict(request, session, attributeSet, ver, val);
//
//    Verdict verdict = VerdictLogic::findVerdict(request, session, attributeSet);
//    bool result = VSPAssertEqual(verdict,Verdict::VERDICT_DENY);
//    //bool result = VSPAssertEqual(verdict,Verdict::VERDICT_UNKNOWN);
//    handleResult(result, "find Verdict empty request empty "
//                         "attribute set test 41 ");
//}
//
//void AceDAOTester::test42(){
//    //empty string as subject and resource, without attributes
//    //and verdict is PERMIT Validity::ALWAYS
//    Request  request("", "");
//
//    AttributeSet attributeSet;
//    std::string tmp("atrS42");
//    std::string tmpValue("Buu342");
//    BaseAttributePtr atr(new Attribute(&tmp, Attribute::Match::Equal, Attribute::Type::Subject));
//    DPL::StaticPointerCast<Attribute>(atr)->addValue(&tmpValue);
//    attributeSet.insert(atr);
//
//    Verdict ver = Verdict::VERDICT_PERMIT;
//    Validity val = Validity::ALWAYS;
//
//    std::string session = getSession();
//    VerdictLogic::addVerdict(request, session, attributeSet, ver, val);
//
//    Verdict verdict = VerdictLogic::findVerdict(request, session, attributeSet);
//    bool result = VSPAssertEqual(verdict,Verdict::VERDICT_PERMIT);
//    handleResult(result, "find Verdict empty request test 42 ");
//}
//
//void AceDAOTester::test43(){
//    //TODO I have changed it! verdict
//    //has changed and stored in DB. does current verdict is the last one?
//
//    Request  request("ac", "ca");
//    AttributeSet attributeSet;
//
//    Verdict ver = Verdict::VERDICT_DENY;
//    Verdict verp = Verdict::VERDICT_PERMIT;
//    Validity val = Validity::ALWAYS;
//
//    std::string session = getSession();
//    VerdictLogic::addVerdict(request, session, attributeSet, ver, val);
//    VerdictLogic::addVerdict(request, session, attributeSet, verp, val);
//
//    Verdict verdict = VerdictLogic::findVerdict(request, session, attributeSet);
//    bool result = VSPAssertEqual(verdict,Verdict::VERDICT_PERMIT);
//    handleResult(result, "find Verdict empty request empty "
//                         "attribute set test 43 ");
//}
//
//void AceDAOTester::test44(){
//    //empty subject and resource name - get Atributes list
//    Request  request("","");
//
//    AttributeSet attributeSet;
//    std::string tmp("atrS44");
//    std::string tmpValue("Buu44");
//    BaseAttributePtr atr(new Attribute(&tmp, Attribute::Match::Equal, Attribute::Type::Subject));
//    DPL::StaticPointerCast<Attribute>(atr)->addValue(&tmpValue);
//    attributeSet.insert(atr);
//
//    Verdict ver = Verdict::VERDICT_PERMIT;
//    Validity val = Validity::ALWAYS;
//    std::string session = getSession();
//    VerdictLogic::addVerdict(request, session, attributeSet, ver, val);
//
//    AttributeSet attributeS;
//    AceDAO::getAttributes(&attributeS);
//
//    bool result = false;
//
//    result = AttributeEqual(attributeSet,attributeS);
//
//    handleResult(!result,"find Attributes empty request  test 44");
//}
//
//void AceDAOTester::test45(){
//    //empty subject and resource name - what is the verdict? - is it OK??
//    Request  request("", "");
//    AttributeSet attributeSet;
//    std::string tmp("atrS15");
//    std::string tmpValue("Buu15");
//    BaseAttributePtr atr(new Attribute(&tmp, Attribute::Match::Equal, Attribute::Type::Subject));
//    DPL::StaticPointerCast<Attribute>(atr)->addValue(&tmpValue);
//    attributeSet.insert(atr);
//
//    Verdict ver = Verdict::VERDICT_PERMIT;
//    Validity val = Validity::ALWAYS;
//    std::string session = getSession();
//    VerdictLogic::addVerdict(request, session, attributeSet, ver, val);
//
//    VerdictLogic::removeVerdict(request, attributeSet);
//
//    Verdict verdict = VerdictLogic::findVerdict(request, session, attributeSet);
//    bool result = VSPAssertEqual(verdict,Verdict::VERDICT_UNKNOWN);
//    handleResult(result, "find Verdict test 45");
//}
//
//void AceDAOTester::test46(){
//    // TODO is it ok that for empty subject and resource name
//    // the verdict is not stored
//    Request  requestemp("", "");
//    Request  request("aa","bb");
//
//    AttributeSet attributeSet;
//    std::string tmp("atrS46");
//    std::string tmpValue("Buu146");
//    BaseAttributePtr atr(new Attribute(&tmp, Attribute::Match::Equal, Attribute::Type::Subject));
//    DPL::StaticPointerCast<Attribute>(atr)->addValue(&tmpValue);
//    attributeSet.insert(atr);
//
//    Verdict verAdd = Verdict::VERDICT_PERMIT;
//    Validity val = Validity::ALWAYS;
//    std::string session = getSession();
//    VerdictLogic::addVerdict(requestemp, session, attributeSet, verAdd, val);
//    VerdictLogic::addVerdict(request, session, attributeSet, verAdd, val);
//
//    VerdictLogic::removeVerdict(requestemp, attributeSet);
//
//    Verdict verdictA = VerdictLogic::findVerdict(requestemp, session, attributeSet);
//    bool resultA = VSPAssertEqual(verdictA,Verdict::VERDICT_UNKNOWN);
//    handleResult(resultA,"find Verdict test 46 A ");
//
//    Verdict verdictB = VerdictLogic::findVerdict(request, session, attributeSet);
//    bool resultB = VSPAssertEqual(verdictB, Verdict::VERDICT_PERMIT);
//    handleResult(resultB,"find Verdict test 46 B ");
//
//}

TESTSUITE06(53){
    CLEANENV;
    //Empty resource name
    std::string res("");
    SettingsLogic::setDevCapSetting(res, Preference::PREFERENCE_PERMIT);
    std::map<std::string, Preference> resourceSettings;
    SettingsLogic::getDevCapSettings(&resourceSettings);

    RUNNER_ASSERT(
        ResourceSettingEqual(
            &resourceSettings,
            res,
            Preference::PREFERENCE_PERMIT));
}

//void AceDAOTester::test48(){
//    SettingsLogic::setResourceSetting(NULL, PERMIT);
//    std::map<std::string, Preference>*  resourceSettings =
//        SettingsLogic::getResourceSettings();
//
//    bool result = ResourceSettingEqual(resourceSettings, NULL,PERMIT );
//    handleResult(result, "get Resource Settings empty resource test 48");
//    printf("get Resource Settings empty resource test 48 -----> "
//           "Segmentation fault\n");
//}

TESTSUITE06(55){
    CLEANENV;
    //resource settings list with empty elemen

    std::string resb("");

    std::list< std::pair<const std::string*,Preference> > *resourcesL = new
            std::list< std::pair<const std::string*, Preference> >();

        resourcesL->push_back(make_pair(&resb, Preference::PREFERENCE_PERMIT));

    SettingsLogic::setAllDevCapSettings (*resourcesL);

    std::map<std::string, Preference> resourceSettings;
    SettingsLogic::getDevCapSettings(&resourceSettings);
    RUNNER_ASSERT(ResourceSettingsEqual(resourcesL, &resourceSettings));
    delete resourcesL;
}

TESTSUITE06(56){
    CLEANENV;
    //user settings with empty subject and resource
    //if resource or subject name is empty, values are not saved in DB
    std::string resb("fake-resource");
    WidgetHandle subb = 0;

    std::list<PermissionTriple> permissionsL;

    permissionsL.push_back(PermissionTriple(subb, resb, Preference::PREFERENCE_PERMIT));

    SettingsLogic::setWidgetDevCapSettings(permissionsL);

    std::list<PermissionTriple> userSettings;
    SettingsLogic::getWidgetDevCapSettings(&userSettings);
    RUNNER_ASSERT(UserSettingsAgreed(&permissionsL, &userSettings));
}

TESTSUITE06(57){
    CLEANENV;
    //user setting equal
    // settings with at least one empty value is not set
    std::string res("");
    WidgetHandle sub = 0;

    SettingsLogic::setWidgetDevCapSetting(res, sub, Preference::PREFERENCE_PERMIT);
    std::list<PermissionTriple> userSettings;
    SettingsLogic::getWidgetDevCapSettings(&userSettings);
    RUNNER_ASSERT(
        UserSettingNotStored(
            &userSettings,
            res,
            sub,
            Preference::PREFERENCE_PERMIT));
}

TESTSUITE06(58){
    CLEANENV;
    //user settings empty values and Default access

    std::string res("");
    WidgetHandle sub = 0;

    SettingsLogic::setWidgetDevCapSetting(res, sub, Preference::PREFERENCE_DEFAULT);
    std::list<PermissionTriple> userSettings;
    SettingsLogic::getWidgetDevCapSettings(&userSettings);
    RUNNER_ASSERT(
        UserSettingNotStored(
            &userSettings,
            res,
            sub,
            Preference::PREFERENCE_DEFAULT));
}

//void AceDAOTester::test53(){
//    Request  request("1 OR sv.verdict=1 --", "r53");
//    AttributeSet attributeSet;
//    std::string tmp("atrS53");
//    std::string tmpValue("Buu15");
//    BaseAttributePtr atr(new Attribute(&tmp, Attribute::Match::Equal, Attribute::Type::Subject));
//    DPL::StaticPointerCast<Attribute>(atr)->addValue(&tmpValue);
//    attributeSet.insert(atr);
//
////    Verdict ver = Verdict::VERDICT_PERMIT;
////    Validity val = Validity::ALWAYS;
//
//    std::string session = getSession();
//    Verdict verdict = VerdictLogic::findVerdict(request, session, attributeSet);
//    bool result = VSPAssertEqual(verdict, Verdict::VERDICT_UNKNOWN);
//    handleResult(result, "find Verdict test 53 sql injection");
//}

//void AceDAOTester::test54()
//{
//    Request  request("1' OR sv.verdict=1 --", "r53");
//    AttributeSet attributeSet;
//    std::string tmp("atrS54");
//    std::string tmpValue("Buu15");
//    BaseAttributePtr atr(new Attribute(&tmp, Attribute::Match::Equal, Attribute::Type::Subject));
//    DPL::StaticPointerCast<Attribute>(atr)->addValue(&tmpValue);
//    attributeSet.insert(atr);
//
//    std::string session = getSession();
//    Verdict verdict = VerdictLogic::findVerdict(request, session, attributeSet);
//    bool result = VSPAssertEqual(verdict, Verdict::VERDICT_UNKNOWN);
//    handleResult(result, "find Verdict test 54 sql injection");
//}
//
//void AceDAOTester::test55()
//{
//    Request request("s55","r55");
//    AttributeSet attributeSet;
//    std::string session = getSession();
//    Verdict verdict = Verdict::VERDICT_PERMIT;
//    Validity validity = Validity::ALWAYS;
//
//    VerdictLogic::addVerdict(request, session, attributeSet, verdict, validity);
//
//    verdict = VerdictLogic::findVerdict(request, session, attributeSet);
//    bool result = VSPAssertEqual(verdict, Verdict::VERDICT_PERMIT);
//    handleResult(result, "Test 55a - clean database");
//
//    SettingsLogic::setWidgetDevCapSetting(request.getResourceId(),
//                        request.getSubjectId(),
//                        Preference::PREFERENCE_DENY);
//
//    Preference preference = SettingsLogic::findGlobalUserSettings(request);
//    handleResult(preference == Preference::PREFERENCE_DENY, "Test 55b - clean database");
//
//    AceDAO::resetDatabase();
//
//    verdict = VerdictLogic::findVerdict(request, session, attributeSet);
//    result = VSPAssertEqual(verdict,  Verdict::VERDICT_UNKNOWN);
//    handleResult(result, "Test 55c - clean database");
//
//    preference = SettingsLogic::findGlobalUserSettings(request);
//    handleResult(preference == Preference::PREFERENCE_DEFAULT,
//                 "Test 55d - clean database");
//}
//
//void AceDAOTester::test56(){
//    Request request("s56","r56");
//    AttributeSet attributeSet;
//    std::string session = getSession();
//    Verdict verdict = Verdict::VERDICT_PERMIT;
//    Validity validity = Validity::ALWAYS;
//
//    std::string aName("atrS15");
//    std::string aValue1("a;");
//    std::string aValue2("b");
//    BaseAttributePtr atr(new Attribute(&aName, Attribute::Match::Equal, Attribute::Type::Subject));
//    DPL::StaticPointerCast<Attribute>(atr)->addValue(&aValue1);
//    DPL::StaticPointerCast<Attribute>(atr)->addValue(&aValue2);
//    attributeSet.insert(atr);
//
//    VerdictLogic::addVerdict(request, session, attributeSet, verdict, validity);
//
//    attributeSet.clear();
//    BaseAttributePtr atr1(new Attribute(&aName, Attribute::Match::Equal, Attribute::Type::Subject));
//    aValue1 = "a";
//    aValue2 = ";b";
//    DPL::StaticPointerCast<Attribute>(atr1)->addValue(&aValue1);
//    DPL::StaticPointerCast<Attribute>(atr1)->addValue(&aValue2);
//    attributeSet.insert(atr1);
//
//    verdict = VerdictLogic::findVerdict(request, session, attributeSet);
//    bool result = VSPAssertEqual(verdict, Verdict::VERDICT_UNKNOWN);
//    handleResult(result, "Test 56 - attribute hash calculation check");
//}


//BaseAttributePtr AceDAOTester::constructAttribute(
//        const char * name,
//        const char * value,
//        Attribute::Match match,
//        Attribute::Type type)
//{
//    std::string tmp(name);
//    std::string tmpValue(value);
//    BaseAttributePtr attr(new Attribute(&tmp, match, type));
//    DPL::StaticPointerCast<Attribute>(attr)->addValue(&tmpValue);
//    return attr;
//}

