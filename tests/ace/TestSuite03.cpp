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
 * @file    TestSuite03.cpp
 * @author  Piotr Fatyga (p.fatyga@samsung.com)
 * @version 0.1
 * @brief   Test cases for Condition class.
 */

#include <string>

#include <dpl/log/log.h>
#include <dpl/test/test_runner.h>

#include <dpl/ace/Attribute.h>
#include <dpl/ace-dao-ro/BaseAttribute.h>
#include <dpl/ace/Condition.h>

#define TESTSUITE03(n) \
RUNNER_TEST(ts03_condtitions_tests_ ## n)

using namespace AceDB;

BaseAttributePtr createAttribute(
        const char *name,
        const char * value,
        const Attribute::Type type = Attribute::Type::Subject)
{
    std::string aName(name);
    BaseAttributePtr attr(new Attribute(&aName, Attribute::Match::Equal, type));
    if(value != NULL){
        std::string aValue(value);
        DPL::StaticPointerCast<Attribute>(attr)->addValue(&aValue);
    }
    return attr;
}

BaseAttributePtr createAttribute(
        const char *name,
        const char *value,
        const char *value2,
        const Attribute::Type type = Attribute::Type::Subject)
{
    std::string aName(name);
    BaseAttributePtr attr(new Attribute(&aName, Attribute::Match::Equal, type));
    if(value != NULL && value2 != NULL ){
        std::string aValue(value), aValue2(value2);
        DPL::StaticPointerCast<Attribute>(attr)->addValue(&aValue);
        DPL::StaticPointerCast<Attribute>(attr)->addValue(&aValue2);
    }
    return attr;
}


bool MatchResultEqual(
        Attribute::MatchResult actual,
        Attribute::MatchResult  expected)
{
    return actual == expected;
}

bool evaluateResult(
        Condition con,
        AttributeSet & attrs,
        Attribute::MatchResult expectedResult)
{
    Attribute::MatchResult matchResult = con.evaluateCondition(&attrs);
    return MatchResultEqual(matchResult ,expectedResult);
}

TESTSUITE03(01){
    Condition con(Condition::OR);
    AttributeSet attrSet;

    BaseAttributePtr a1 = createAttribute("version","var1");
    BaseAttributePtr a2 = createAttribute("r8v2","var2");
    BaseAttributePtr a3 = createAttribute("author","va3");

    con.addAttribute(*DPL::StaticPointerCast<Attribute>(a1));
    con.addAttribute(*DPL::StaticPointerCast<Attribute>(a2));
    con.addAttribute(*DPL::StaticPointerCast<Attribute>(a3));

    attrSet.insert(a1);
    attrSet.insert(a2);
    attrSet.insert(a3);

    RUNNER_ASSERT(
        evaluateResult(con,attrSet,Attribute::MatchResult::MRTrue));
}

TESTSUITE03(02){
    Condition con(Condition::AND);
    AttributeSet attrSet;

    BaseAttributePtr a1 = createAttribute("version","var1");
    BaseAttributePtr a2 = createAttribute("r8v2","var2");
    BaseAttributePtr a3 = createAttribute("author","va3");

    con.addAttribute(*DPL::StaticPointerCast<Attribute>(a1));
    con.addAttribute(*DPL::StaticPointerCast<Attribute>(a2));
    con.addAttribute(*DPL::StaticPointerCast<Attribute>(a3));

    attrSet.insert(a1);
    attrSet.insert(a2);
    attrSet.insert(a3);

    RUNNER_ASSERT(
        evaluateResult(con,attrSet,Attribute::MatchResult::MRTrue));
}

TESTSUITE03(03){
    Condition con(Condition::OR);
    AttributeSet attrSet;

    BaseAttributePtr a1 = createAttribute("version","var1");
    BaseAttributePtr a2 = createAttribute("r8v2","var2");
    BaseAttributePtr a3 = createAttribute("author","va3");
    BaseAttributePtr a4 = createAttribute("version","wrongValue");

    con.addAttribute(*DPL::StaticPointerCast<Attribute>(a1));
    con.addAttribute(*DPL::StaticPointerCast<Attribute>(a2));
    con.addAttribute(*DPL::StaticPointerCast<Attribute>(a3));

    attrSet.insert(a2);
    attrSet.insert(a3);
    attrSet.insert(a4);

    RUNNER_ASSERT(
        evaluateResult(con,attrSet,Attribute::MatchResult::MRTrue));
}

TESTSUITE03(04){
    Condition con(Condition::AND);
    AttributeSet attrSet;

    BaseAttributePtr a1 = createAttribute("version","var1");
    BaseAttributePtr a2 = createAttribute("r8v2","var2");
    BaseAttributePtr a3 = createAttribute("author","va3");
    BaseAttributePtr a4 = createAttribute("version","bad");
    BaseAttributePtr a5 = createAttribute("version","var1", Attribute::Type::Resource);

    con.addAttribute(*DPL::StaticPointerCast<Attribute>(a1));
    con.addAttribute(*DPL::StaticPointerCast<Attribute>(a2));
    con.addAttribute(*DPL::StaticPointerCast<Attribute>(a3));

    attrSet.insert(a2);
    attrSet.insert(a3);
    attrSet.insert(a4);
    attrSet.insert(a5);

    RUNNER_ASSERT(
        evaluateResult(con,attrSet,Attribute::MatchResult::MRFalse));
}

TESTSUITE03(05){
    Condition con(Condition::OR);
    AttributeSet attrSet;

    BaseAttributePtr a1 = createAttribute("version","var1");
    BaseAttributePtr a2 = createAttribute("r8v2","var2");
    BaseAttributePtr a3 = createAttribute("author","va3");

    BaseAttributePtr a4 = createAttribute("version","var1");
    BaseAttributePtr a5 = createAttribute("r8v2","var2");
    BaseAttributePtr a6 = createAttribute("author","va3");

    con.addAttribute(*DPL::StaticPointerCast<Attribute>(a1));
    con.addAttribute(*DPL::StaticPointerCast<Attribute>(a2));
    con.addAttribute(*DPL::StaticPointerCast<Attribute>(a3));

    attrSet.insert(a4);
    attrSet.insert(a5);
    attrSet.insert(a6);

    RUNNER_ASSERT(
        evaluateResult(con,attrSet,Attribute::MatchResult::MRTrue));
}

TESTSUITE03(06){
    Condition con(Condition::OR);
    AttributeSet attrSet;

    BaseAttributePtr a1 = createAttribute("version","var1");
    BaseAttributePtr a2 = createAttribute("r8v2","var2");
    BaseAttributePtr a3 = createAttribute("author","va3");

    BaseAttributePtr a4 = createAttribute("version","var2");
    BaseAttributePtr a5 = createAttribute("r8v2","var3");
    BaseAttributePtr a6 = createAttribute("author","va4");

    con.addAttribute(*DPL::StaticPointerCast<Attribute>(a1));
    con.addAttribute(*DPL::StaticPointerCast<Attribute>(a2));
    con.addAttribute(*DPL::StaticPointerCast<Attribute>(a3));

    attrSet.insert(a4);
    attrSet.insert(a5);
    attrSet.insert(a6);

    RUNNER_ASSERT(
        evaluateResult(con,attrSet,Attribute::MatchResult::MRFalse));
}

TESTSUITE03(07){
    Condition con(Condition::OR);
    AttributeSet attrSet;

    BaseAttributePtr a1 = createAttribute("version","var1");
    BaseAttributePtr a2 = createAttribute("r8v2","var2");
    BaseAttributePtr a3 = createAttribute("author","va3");

    BaseAttributePtr a4 = createAttribute("version","var1", Attribute::Type::Resource );
    BaseAttributePtr a5 = createAttribute("r8v2","var2", Attribute::Type::Resource);
    BaseAttributePtr a6 = createAttribute("author","va3", Attribute::Type::Resource);

    BaseAttributePtr a7 = createAttribute("version","var2" );
    BaseAttributePtr a8 = createAttribute("r8v2","var3");
    BaseAttributePtr a9 = createAttribute("author","va4");

    con.addAttribute(*DPL::StaticPointerCast<Attribute>(a1));
    con.addAttribute(*DPL::StaticPointerCast<Attribute>(a2));
    con.addAttribute(*DPL::StaticPointerCast<Attribute>(a3));

    attrSet.insert(a4);
    attrSet.insert(a5);
    attrSet.insert(a6);
    attrSet.insert(a7);
    attrSet.insert(a8);
    attrSet.insert(a9);

    //Despite that there are attribute a4,a5,a6 that matches the value of attributes
    //a1,a2,a3 the type of the attributes is different
    RUNNER_ASSERT(
        evaluateResult(con,attrSet,Attribute::MatchResult::MRFalse));
}

//Test if the same attributes (but other instances) passes the test

TESTSUITE03(08){
    Condition con(Condition::OR);
    AttributeSet attrSet;

    BaseAttributePtr a1 = createAttribute("version","var1");
    BaseAttributePtr a2 = createAttribute("r8v2","var2");
    BaseAttributePtr a3 = createAttribute("author","va3");

    BaseAttributePtr a4 = createAttribute("version","var1");
    BaseAttributePtr a5 = createAttribute("r8v2","var2", Attribute::Type::Resource);
    BaseAttributePtr a6 = createAttribute("author","va3", Attribute::Type::Resource);
    BaseAttributePtr a7 = createAttribute("r8v2","aaa");
    BaseAttributePtr a8 = createAttribute("author","aaa");

    con.addAttribute(*DPL::StaticPointerCast<Attribute>(a1));
    con.addAttribute(*DPL::StaticPointerCast<Attribute>(a2));
    con.addAttribute(*DPL::StaticPointerCast<Attribute>(a3));

    attrSet.insert(a4);
    attrSet.insert(a5);
    attrSet.insert(a6);
    attrSet.insert(a7);
    attrSet.insert(a8);

    RUNNER_ASSERT(
        evaluateResult(con,attrSet,Attribute::MatchResult::MRTrue));
}

TESTSUITE03(09){
    Condition con(Condition::OR);
    AttributeSet attrSet;

    BaseAttributePtr a1 = createAttribute("version", NULL); //empty bag
    con.addAttribute(*DPL::StaticPointerCast<Attribute>(a1));
    attrSet.insert(a1);

    RUNNER_ASSERT(
        evaluateResult(con,attrSet,Attribute::MatchResult::MRFalse));
}

TESTSUITE03(10){
    Condition con(Condition::OR);
    AttributeSet attrSet;

    BaseAttributePtr a1 =  createAttribute("version","var1","var2");

    BaseAttributePtr a4 =  createAttribute("version","var1");

    con.addAttribute(*DPL::StaticPointerCast<Attribute>(a1));
    attrSet.insert(a4);
    //It's enough that one value from string bag matches
    RUNNER_ASSERT(
        evaluateResult(con,attrSet,Attribute::MatchResult::MRTrue));
}

TESTSUITE03(11){
    Condition con(Condition::AND);
    AttributeSet attrSet;

    BaseAttributePtr a1 =  createAttribute("version","var1","var2");
    BaseAttributePtr a2 =  createAttribute("version","var3","var1");

    BaseAttributePtr a4 =  createAttribute("version","var1");

    con.addAttribute(*DPL::StaticPointerCast<Attribute>(a1));
    con.addAttribute(*DPL::StaticPointerCast<Attribute>(a2));
    attrSet.insert(a4);
    //It's enough that one value from string bag matches
    RUNNER_ASSERT(
        evaluateResult(con,attrSet,Attribute::MatchResult::MRTrue));
}


TESTSUITE03(12){
    Condition con(Condition::OR);
    AttributeSet attrSet;

    BaseAttributePtr a1 =  createAttribute("version","var1");
    BaseAttributePtr a2 = createAttribute("r8v2","var2");
    BaseAttributePtr a3 = createAttribute("author","va3");

    BaseAttributePtr a4 =  createAttribute("version","var1");
    BaseAttributePtr a5 = createAttribute("r8v2","var2", Attribute::Type::Resource);
    BaseAttributePtr a6 = createAttribute("author","va3", Attribute::Type::Resource);

    con.addAttribute(*DPL::StaticPointerCast<Attribute>(a1));
    con.addAttribute(*DPL::StaticPointerCast<Attribute>(a2));
    con.addAttribute(*DPL::StaticPointerCast<Attribute>(a3));

    attrSet.insert(a4);
    attrSet.insert(a5);
    attrSet.insert(a6);
    RUNNER_ASSERT(
        evaluateResult(con,attrSet,Attribute::MatchResult::MRTrue));
}

TESTSUITE03(13){
    Condition con(Condition::AND);
    AttributeSet attrSet;

    BaseAttributePtr a1 =  createAttribute("version",NULL);
    a1->setUndetermind(true);
    BaseAttributePtr a4 =  createAttribute("version","var1");
    con.addAttribute(*DPL::StaticPointerCast<Attribute>(a4));
    attrSet.insert(a1);

    RUNNER_ASSERT(
        evaluateResult(con,attrSet,Attribute::MatchResult::MRUndetermined));
}

TESTSUITE03(14){
    Condition con(Condition::OR);
    AttributeSet attrSet;

    BaseAttributePtr a1 =  createAttribute("version",NULL);
    a1->setUndetermind(true);
    BaseAttributePtr a2 =  createAttribute("author","good");
    BaseAttributePtr a4 =  createAttribute("version","var1");
    con.addAttribute(*DPL::StaticPointerCast<Attribute>(a4));
    con.addAttribute(*DPL::StaticPointerCast<Attribute>(a2));
    attrSet.insert(a1);
    attrSet.insert(a2);

    RUNNER_ASSERT(
        evaluateResult(con,attrSet,Attribute::MatchResult::MRTrue));
}

TESTSUITE03(15){
    Condition con(Condition::OR);
    AttributeSet attrSet;

    BaseAttributePtr a1 = createAttribute("version",NULL,NULL);
    a1->setUndetermind(true);
    BaseAttributePtr a4 = createAttribute("version","var1");
    con.addAttribute(*DPL::StaticPointerCast<Attribute>(a4));
    attrSet.insert(a1);

    RUNNER_ASSERT(
        evaluateResult(con,attrSet,Attribute::MatchResult::MRUndetermined));
}

TESTSUITE03(16){
    Condition con(Condition::OR);
    AttributeSet attrSet;

    BaseAttributePtr a1 =  createAttribute("version","var1","aaa");
    a1->setUndetermind(true);
    BaseAttributePtr a4 =  createAttribute("version","var1");
    con.addAttribute(*DPL::StaticPointerCast<Attribute>(a4));
    attrSet.insert(a1);

    RUNNER_ASSERT(
        evaluateResult(con,attrSet,Attribute::MatchResult::MRUndetermined));
}

TESTSUITE03(17){
    Condition con(Condition::AND);
    AttributeSet attrSet;

    BaseAttributePtr a1 = createAttribute("version","var1");
    BaseAttributePtr a2 = createAttribute("version1","var1");
    BaseAttributePtr a3 = createAttribute("version2","var1");
    BaseAttributePtr a4 = createAttribute("version3","var1");
    BaseAttributePtr a5 = createAttribute("version1","var2");

    Condition con2(Condition::AND);
    con2.addAttribute(*DPL::StaticPointerCast<Attribute>(a2));
    Condition con3(Condition::OR);
    con3.addAttribute(*DPL::StaticPointerCast<Attribute>(a3));
    con3.addAttribute(*DPL::StaticPointerCast<Attribute>(a4));

    con.addCondition(con2);
    con.addCondition(con3);

    con.addAttribute(*DPL::StaticPointerCast<Attribute>(a4));
    attrSet.insert(a1);
    attrSet.insert(a3);
    attrSet.insert(a4);
    attrSet.insert(a5);

    RUNNER_ASSERT(
        evaluateResult(con,attrSet,Attribute::MatchResult::MRFalse));
}

TESTSUITE03(18){
    Condition con(Condition::OR);
    AttributeSet attrSet;

    BaseAttributePtr a1 = createAttribute("version","var1");
    BaseAttributePtr a2 = createAttribute("version1","var1");
    BaseAttributePtr a3 = createAttribute("version2","var1");
    BaseAttributePtr a4 = createAttribute("version3","var1");
    BaseAttributePtr a5 = createAttribute("version1","var2");

    Condition con2(Condition::AND);
    con2.addAttribute(*DPL::StaticPointerCast<Attribute>(a2));
    con2.addAttribute(*DPL::StaticPointerCast<Attribute>(a5));
    Condition con3(Condition::AND);
    con3.addAttribute(*DPL::StaticPointerCast<Attribute>(a3));
    con3.addAttribute(*DPL::StaticPointerCast<Attribute>(a4));
    Condition con4(Condition::AND);
    con4.addAttribute(*DPL::StaticPointerCast<Attribute>(a2));

    con3.addCondition(con4);
    con.addCondition(con2);
    con.addCondition(con3);

    attrSet.insert(a1);
    attrSet.insert(a3);
    attrSet.insert(a4);
    attrSet.insert(a5);

    RUNNER_ASSERT(
        evaluateResult(con,attrSet,Attribute::MatchResult::MRFalse));
}

TESTSUITE03(19){
    Condition con(Condition::OR);
    AttributeSet attrSet;

    BaseAttributePtr a1 = createAttribute("version","var1");
    BaseAttributePtr a2 = createAttribute("version1","var1");
    BaseAttributePtr a3 = createAttribute("version2","var1");
    BaseAttributePtr a4 = createAttribute("version3","var1");
    BaseAttributePtr a5 = createAttribute("version1","var2");

    Condition con2(Condition::AND);
    con2.addAttribute(*DPL::StaticPointerCast<Attribute>(a2));
    con2.addAttribute(*DPL::StaticPointerCast<Attribute>(a5));
    Condition con3(Condition::OR);
    con3.addAttribute(*DPL::StaticPointerCast<Attribute>(a3));
    con3.addAttribute(*DPL::StaticPointerCast<Attribute>(a4));

    con.addCondition(con2);
    con.addCondition(con3);

    attrSet.insert(a1);
    attrSet.insert(a3);
    attrSet.insert(a4);
    attrSet.insert(a5);

    RUNNER_ASSERT(
        evaluateResult(con,attrSet,Attribute::MatchResult::MRTrue));
}

TESTSUITE03(20){
    Condition con(Condition::OR);
    AttributeSet attrSet;

    BaseAttributePtr a1 = createAttribute("version","var1");
    BaseAttributePtr a2 = createAttribute("version1","var1");
    BaseAttributePtr a3 = createAttribute("version2","var1");
    BaseAttributePtr a4 = createAttribute("version3","var1");

    BaseAttributePtr a5 = createAttribute("version1","var2");
    BaseAttributePtr a6 = createAttribute("version2","var2");
    BaseAttributePtr a7 = createAttribute("version3","var2");
    BaseAttributePtr a8 = createAttribute("version4","var2");

    Condition con2(Condition::OR);
    Condition con3(Condition::OR);
    Condition con4(Condition::OR);
    Condition con5(Condition::OR);

    con2.addAttribute(*DPL::StaticPointerCast<Attribute>(a2));
    con3.addAttribute(*DPL::StaticPointerCast<Attribute>(a3));
    con4.addAttribute(*DPL::StaticPointerCast<Attribute>(a4));
    con5.addAttribute(*DPL::StaticPointerCast<Attribute>(a1));

    con2.addCondition(con4);
    con3.addCondition(con5);
    con.addCondition(con2);
    con.addCondition(con3);

    attrSet.insert(a1);
    attrSet.insert(a5);
    attrSet.insert(a6);
    attrSet.insert(a7);
    attrSet.insert(a8);

    RUNNER_ASSERT(
        evaluateResult(con,attrSet,Attribute::MatchResult::MRTrue));
}
