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
 * @file    TestSuite02.cpp
 * @author  unknown
 * @version 1.0
 * @brief   Test cases for Attribute class.
 */
#include <string>

#include <dpl/log/log.h>
#include <dpl/test/test_runner.h>

#include <dpl/ace/Attribute.h>
#include <dpl/ace-dao-ro/BaseAttribute.h>

#include <dpl/ace/PolicyEvaluator.h>

#include "PEPSingleton.h"

#define POLICY_ATTR_EXAMPLE  "/usr/etc/ace/attr_policy-example.xml"
#define POLICY_ATTR_EXAMPLE1 "/usr/etc/ace/attr_policy-example1.xml"
#define POLICY_ATTR_EXAMPLE2 "/usr/etc/ace/attr_policy-example2.xml"
#define POLICY_ATTR_EXAMPLE3 "/usr/etc/ace/attr_policy-example3.xml"
#define POLICY_ATTR_EXAMPLE4 "/usr/etc/ace/attr_policy-example4.xml"
#define POLICY_ATTR_EXAMPLE5 "/usr/etc/ace/attr_policy-example5.xml"
#define POLICY_ATTR_EXAMPLE6 "/usr/etc/ace/attr_policy-example6.xml"
#define POLICY_ATTR_EXAMPLE7 "/usr/etc/ace/attr_policy-example7.xml"
#define POLICY_ATTR_EXAMPLE8 "/usr/etc/ace/attr_policy-example8.xml"

#define CLEANENV(d)                                                         \
    do{                                                                     \
        if (PEPSingleton::Instance().getPdp()->getCurrentPolicy() != (d)) { \
            PEPSingleton::Instance().getPdp()->updatePolicy(d);             \
        }                                                                   \
    }while(0)

#define PEPTR PEPSingleton::Instance().getPdp()

#define TESTSUITE02(n) \
RUNNER_TEST(ts02_extract_attributes_tests_ ## n)

using namespace AceDB;

bool AttrEqual(const AttributeSet &actual, AttributeSet * expected)
{
    bool match = false;
    for (AttributeSet::const_iterator ita = actual.begin();
         ita != actual.end();
         ++ita)
    {
        match = false;
        for (AttributeSet::const_iterator ite = expected->begin();
             ite != expected->end();
             ++ite)
        {
            if ((*(*ita)->getName() == *(*ite)->getName()) &&
                ((*ita)->getType() == (*ite)->getType()))
            {
                match = true;
            }
        }
        if (!match)
        {
            LogError("Not found " <<
                 *(*ita)->getName() <<
                 " " << static_cast<int>((*ita)->getType()));
            return false;
        }
    }
    return true;
}

TESTSUITE02(01){
    CLEANENV(POLICY_ATTR_EXAMPLE);

    AttributeSet attrSet;
    std::string n1("name");
    BaseAttributePtr a1(new Attribute(&n1,
                                      Attribute::Match::Equal,
                                      Attribute::Type::Subject));
    attrSet.insert(a1);


    std::string n2("resource-id");
    BaseAttributePtr a2(new Attribute(&n2,
                                      Attribute::Match::Equal,
                                      Attribute::Type::Resource));
    attrSet.insert(a2);

    PEPTR->extractAttributesTest();

    bool result = AttrEqual(attrSet, PEPTR->getAttributeSet());
    RUNNER_ASSERT(result);
}

TESTSUITE02(02){
    CLEANENV(POLICY_ATTR_EXAMPLE1);

    AttributeSet attrSet;

    std::string n3("uri.host");
    BaseAttributePtr a3(new Attribute(&n3,
                                      Attribute::Match::Equal,
                                      Attribute::Type::Subject));
    attrSet.insert(a3);

    std::string n4("key-root-trust");
    BaseAttributePtr a4(new Attribute(&n4,
                                      Attribute::Match::Equal,
                                      Attribute::Type::Subject));
    attrSet.insert(a4);

    std::string n5("id");
    BaseAttributePtr a5(new Attribute(&n5,
                                      Attribute::Match::Equal,
                                      Attribute::Type::Subject));
    attrSet.insert(a5);

    std::string n6("signer-id");
    BaseAttributePtr a6(new Attribute(&n6,
                                      Attribute::Match::Equal,
                                      Attribute::Type::Subject));
    attrSet.insert(a6);

    std::string n7("version");
    BaseAttributePtr a7(new Attribute(&n7,
                                      Attribute::Match::Equal,
                                      Attribute::Type::Subject));
    attrSet.insert(a7);

    std::string n8("r8v2");
    BaseAttributePtr a8(new Attribute(&n8,
                                      Attribute::Match::Equal,
                                      Attribute::Type::Resource));
    attrSet.insert(a8);

    std::string n9("author");
    BaseAttributePtr a9(new Attribute(&n9,
                                      Attribute::Match::Equal,
                                      Attribute::Type::Resource));
    attrSet.insert(a9);

    std::string n10("r9a.scheme");
    BaseAttributePtr a10(new Attribute(&n10,
                                       Attribute::Match::Equal,
                                       Attribute::Type::Resource));
    attrSet.insert(a10);

    std::string n11("r9b.authority");
    BaseAttributePtr a11(new Attribute(&n11,
                                       Attribute::Match::Equal,
                                       Attribute::Type::Resource));
    attrSet.insert(a11);

    std::string n12("r9c.scheme-authority");
    BaseAttributePtr a12(new Attribute(&n12,
                                       Attribute::Match::Equal,
                                       Attribute::Type::Resource));
    attrSet.insert(a12);

    PEPTR->extractAttributesTest();

    bool result = AttrEqual(attrSet, PEPTR->getAttributeSet());
    RUNNER_ASSERT(result);
}

TESTSUITE02(03){
    CLEANENV(POLICY_ATTR_EXAMPLE2);

    AttributeSet attrSet;

    std::string n1("id");
    BaseAttributePtr a1(new Attribute(&n1,
                                      Attribute::Match::Equal,
                                      Attribute::Type::Subject));
    attrSet.insert(a1);

    std::string n3("version");
    BaseAttributePtr a3(new Attribute(&n3,
                                      Attribute::Match::Equal,
                                      Attribute::Type::Subject));
    attrSet.insert(a3);

    std::string n2("resource-id");
    BaseAttributePtr a2(new Attribute(&n2,
                                      Attribute::Match::Equal,
                                      Attribute::Type::Resource));
    attrSet.insert(a2);

    std::string n4("author");
    BaseAttributePtr a4(new Attribute(&n4,
                                      Attribute::Match::Equal,
                                      Attribute::Type::Resource));
    attrSet.insert(a4);

    PEPTR->extractAttributesTest();

    bool result = AttrEqual(attrSet, PEPTR->getAttributeSet());
    RUNNER_ASSERT(result);
}

TESTSUITE02(04){
    CLEANENV(POLICY_ATTR_EXAMPLE3);

    AttributeSet attrSet;

    std::string n1("id");
    BaseAttributePtr a1(new Attribute(&n1,
                                      Attribute::Match::Equal,
                                      Attribute::Type::Subject));
    attrSet.insert(a1);

    std::string n3("version");
    BaseAttributePtr a3(new Attribute(&n3,
                                      Attribute::Match::Equal,
                                      Attribute::Type::Subject));
    attrSet.insert(a3);

    std::string n2("resource-id");
    BaseAttributePtr a2(new Attribute(&n2,
                                      Attribute::Match::Equal,
                                      Attribute::Type::Resource));
    attrSet.insert(a2);

    std::string n4("author");
    BaseAttributePtr a4(new Attribute(&n4,
                                      Attribute::Match::Equal,
                                      Attribute::Type::Resource));
    attrSet.insert(a4);

    PEPTR->extractAttributesTest();

    bool result = AttrEqual(attrSet, PEPTR->getAttributeSet());
    RUNNER_ASSERT(result);
}

TESTSUITE02(05){
    CLEANENV(POLICY_ATTR_EXAMPLE4);

    AttributeSet attrSet;

    std::string n1("id");
    BaseAttributePtr a1(new Attribute(&n1,
                                      Attribute::Match::Equal,
                                      Attribute::Type::Subject));
    attrSet.insert(a1);

    std::string n3("version");
    BaseAttributePtr a3(new Attribute(&n3,
                                      Attribute::Match::Equal,
                                      Attribute::Type::Subject));
    attrSet.insert(a3);


    std::string n2("resource-id");
    BaseAttributePtr a2(new Attribute(&n2,
                                      Attribute::Match::Equal,
                                      Attribute::Type::Resource));
    attrSet.insert(a2);

    std::string n4("author");
    BaseAttributePtr a4(new Attribute(&n4,
                                      Attribute::Match::Equal,
                                      Attribute::Type::Resource));
    attrSet.insert(a4);

    std::string n5("author");
    BaseAttributePtr a5(new Attribute(&n5,
                                      Attribute::Match::Equal,
                                      Attribute::Type::Environment));
    attrSet.insert(a5);

    std::string n6("version");
    BaseAttributePtr a6(new Attribute(&n6,
                                      Attribute::Match::Equal,
                                      Attribute::Type::Environment));
    attrSet.insert(a6);

    std::string n7("ver");
    BaseAttributePtr a7(new Attribute(&n7,
                                      Attribute::Match::Equal,
                                      Attribute::Type::Environment));
    attrSet.insert(a7);

    PEPTR->extractAttributesTest();

    bool result = AttrEqual(attrSet, PEPTR->getAttributeSet());
    RUNNER_ASSERT(result);
}

TESTSUITE02(06){
    CLEANENV(POLICY_ATTR_EXAMPLE5);

    AttributeSet attrSet;

    std::string n3("uri.host");
    BaseAttributePtr a3(new Attribute(&n3,
                                      Attribute::Match::Equal,
                                      Attribute::Type::Subject));
    attrSet.insert(a3);

    std::string n4("key-root-trust");
    BaseAttributePtr a4(new Attribute(&n4,
                                      Attribute::Match::Equal,
                                      Attribute::Type::Subject));
    attrSet.insert(a4);

    std::string n5("id");
    BaseAttributePtr a5(new Attribute(&n5,
                                      Attribute::Match::Equal,
                                      Attribute::Type::Subject));
    attrSet.insert(a5);

    std::string n6("signer-id");
    BaseAttributePtr a6(new Attribute(&n6,
                                      Attribute::Match::Equal,
                                      Attribute::Type::Subject));
    attrSet.insert(a6);

    std::string n7("version");
    BaseAttributePtr a7(new Attribute(&n7,
                                      Attribute::Match::Equal,
                                      Attribute::Type::Subject));
    attrSet.insert(a7);

    std::string n13("name");
    BaseAttributePtr a13(new Attribute(&n13,
                                       Attribute::Match::Equal,
                                       Attribute::Type::Subject));
    attrSet.insert(a13);

    std::string n14("resource-id");
    BaseAttributePtr a14(new Attribute(&n14,
                                       Attribute::Match::Equal,
                                       Attribute::Type::Subject));
    attrSet.insert(a14);

    std::string n15("r8v2");
    BaseAttributePtr a15(new Attribute(&n15,
                                       Attribute::Match::Equal,
                                       Attribute::Type::Subject));
    attrSet.insert(a15);

    std::string n16("author");
    BaseAttributePtr a16(new Attribute(&n16,
                                       Attribute::Match::Equal,
                                       Attribute::Type::Subject));
    attrSet.insert(a16);

    std::string n17("r9a.scheme");
    BaseAttributePtr a17(new Attribute(&n17,
                                       Attribute::Match::Equal,
                                       Attribute::Type::Subject));
    attrSet.insert(a17);

    std::string n18("r9b.authority");
    BaseAttributePtr a18(new Attribute(&n18,
                                       Attribute::Match::Equal,
                                       Attribute::Type::Subject));
    attrSet.insert(a18);

    std::string n19("r9c.scheme-authority");
    BaseAttributePtr a19(new Attribute(&n19,
                                       Attribute::Match::Equal,
                                       Attribute::Type::Subject));
    attrSet.insert(a19);

    std::string n20("resource-id");
    BaseAttributePtr a20(new Attribute(&n20,
                                       Attribute::Match::Equal,
                                       Attribute::Type::Resource));
    attrSet.insert(a20);

    std::string n8("r8v2");
    BaseAttributePtr a8(new Attribute(&n8,
                                      Attribute::Match::Equal,
                                      Attribute::Type::Resource));
    attrSet.insert(a8);

    std::string n9("author");
    BaseAttributePtr a9(new Attribute(&n9,
                                      Attribute::Match::Equal,
                                      Attribute::Type::Resource));
    attrSet.insert(a9);

    std::string n10("r9a.scheme");
    BaseAttributePtr a10(new Attribute(&n10,
                                       Attribute::Match::Equal,
                                       Attribute::Type::Resource));
    attrSet.insert(a10);

    std::string n11("r9b.authority");
    BaseAttributePtr a11(new Attribute(&n11,
                                       Attribute::Match::Equal,
                                       Attribute::Type::Resource));
    attrSet.insert(a11);

    std::string n12("r9c.scheme-authority");
    BaseAttributePtr a12(new Attribute(&n12,
                                       Attribute::Match::Equal,
                                       Attribute::Type::Resource));
    attrSet.insert(a12);

    std::string n21("resource-id");
    BaseAttributePtr a21(new Attribute(&n21,
                                       Attribute::Match::Equal,
                                       Attribute::Type::Environment));
    attrSet.insert(a21);

    std::string n22("r8v2");
    BaseAttributePtr a22(new Attribute(&n22,
                                       Attribute::Match::Equal,
                                       Attribute::Type::Environment));
    attrSet.insert(a22);

    std::string n23("author");
    BaseAttributePtr a23(new Attribute(&n23,
                                       Attribute::Match::Equal,
                                       Attribute::Type::Environment));
    attrSet.insert(a23);

    std::string n24("r9a.scheme");
    BaseAttributePtr a24(new Attribute(&n24,
                                       Attribute::Match::Equal,
                                       Attribute::Type::Environment));
    attrSet.insert(a24);

    std::string n25("r9b.authority");
    BaseAttributePtr a25(new Attribute(&n25,
                                       Attribute::Match::Equal,
                                       Attribute::Type::Environment));
    attrSet.insert(a25);

    std::string n26("r9c.scheme-authority");
    BaseAttributePtr a26(new Attribute(&n26,
                                       Attribute::Match::Equal,
                                       Attribute::Type::Environment));
    attrSet.insert(a26);

    PEPTR->extractAttributesTest();

    bool result = AttrEqual(attrSet, PEPTR->getAttributeSet());
    RUNNER_ASSERT(result);
}

TESTSUITE02(07){
    CLEANENV(POLICY_ATTR_EXAMPLE6);

    AttributeSet attrSet;

    std::string n3("s-uri.host");
    BaseAttributePtr a3(new Attribute(&n3,
                                      Attribute::Match::Equal,
                                      Attribute::Type::Subject));
    attrSet.insert(a3);

    std::string n4("s-key-root-trust");
    BaseAttributePtr a4(new Attribute(&n4,
                                      Attribute::Match::Equal,
                                      Attribute::Type::Subject));
    attrSet.insert(a4);

    std::string n5("s-id");
    BaseAttributePtr a5(new Attribute(&n5,
                                      Attribute::Match::Equal,
                                      Attribute::Type::Subject));
    attrSet.insert(a5);

    std::string n6("s-signer-id");
    BaseAttributePtr a6(new Attribute(&n6,
                                      Attribute::Match::Equal,
                                      Attribute::Type::Subject));
    attrSet.insert(a6);

    std::string n7("s-version");
    BaseAttributePtr a7(new Attribute(&n7,
                                      Attribute::Match::Equal,
                                      Attribute::Type::Subject));
    attrSet.insert(a7);

    std::string n13("s-name");
    BaseAttributePtr a13(new Attribute(&n13,
                                       Attribute::Match::Equal,
                                       Attribute::Type::Subject));
    attrSet.insert(a13);

    std::string n14("s-resource-id");
    BaseAttributePtr a14(new Attribute(&n14,
                                       Attribute::Match::Equal,
                                       Attribute::Type::Subject));
    attrSet.insert(a14);

    std::string n15("s-r8v2");
    BaseAttributePtr a15(new Attribute(&n15,
                                       Attribute::Match::Equal,
                                       Attribute::Type::Subject));
    attrSet.insert(a15);

    std::string n16("s-author");
    BaseAttributePtr a16(new Attribute(&n16,
                                       Attribute::Match::Equal,
                                       Attribute::Type::Subject));
    attrSet.insert(a16);

    std::string n17("s-r9a.scheme");
    BaseAttributePtr a17(new Attribute(&n17,
                                       Attribute::Match::Equal,
                                       Attribute::Type::Subject));
    attrSet.insert(a17);

    std::string n18("s-r9b.authority");
    BaseAttributePtr a18(new Attribute(&n18,
                                       Attribute::Match::Equal,
                                       Attribute::Type::Subject));
    attrSet.insert(a18);

    std::string n19("s-r9c.scheme-authority");
    BaseAttributePtr a19(new Attribute(&n19,
                                       Attribute::Match::Equal,
                                       Attribute::Type::Subject));
    attrSet.insert(a19);

    std::string n20("r-resource-id");
    BaseAttributePtr a20(new Attribute(&n20,
                                       Attribute::Match::Equal,
                                       Attribute::Type::Resource));
    attrSet.insert(a20);

    std::string n8("r-r8v2");
    BaseAttributePtr a8(new Attribute(&n8,
                                      Attribute::Match::Equal,
                                      Attribute::Type::Resource));
    attrSet.insert(a8);

    std::string n9("r-author");
    BaseAttributePtr a9(new Attribute(&n9,
                                      Attribute::Match::Equal,
                                      Attribute::Type::Resource));
    attrSet.insert(a9);

    std::string n10("r-r9a.scheme");
    BaseAttributePtr a10(new Attribute(&n10,
                                       Attribute::Match::Equal,
                                       Attribute::Type::Resource));
    attrSet.insert(a10);

    std::string n11("r-r9b.authority");
    BaseAttributePtr a11(new Attribute(&n11,
                                       Attribute::Match::Equal,
                                       Attribute::Type::Resource));
    attrSet.insert(a11);

    std::string n12("r-r9c.scheme-authority");
    BaseAttributePtr a12(new Attribute(&n12,
                                       Attribute::Match::Equal,
                                       Attribute::Type::Resource));
    attrSet.insert(a12);

    std::string n21("e-resource-id");
    BaseAttributePtr a21(new Attribute(&n21,
                                       Attribute::Match::Equal,
                                       Attribute::Type::Environment));
    attrSet.insert(a21);

    std::string n22("e-r8v2");
    BaseAttributePtr a22(new Attribute(&n22,
                                       Attribute::Match::Equal,
                                       Attribute::Type::Environment));
    attrSet.insert(a22);

    std::string n23("e-author");
    BaseAttributePtr a23(new Attribute(&n23,
                                       Attribute::Match::Equal,
                                       Attribute::Type::Environment));
    attrSet.insert(a23);

    std::string n24("e-r9a.scheme");
    BaseAttributePtr a24(new Attribute(&n24,
                                       Attribute::Match::Equal,
                                       Attribute::Type::Environment));
    attrSet.insert(a24);

    std::string n25("e-r9b.authority");
    BaseAttributePtr a25(new Attribute(&n25,
                                       Attribute::Match::Equal,
                                       Attribute::Type::Environment));
    attrSet.insert(a25);

    std::string n26("e-r9c.scheme-authority");
    BaseAttributePtr a26(new Attribute(&n26,
                                       Attribute::Match::Equal,
                                       Attribute::Type::Environment));
    attrSet.insert(a26);

    PEPTR->extractAttributesTest();

    bool result = AttrEqual(attrSet, PEPTR->getAttributeSet());
    RUNNER_ASSERT(result);
}

TESTSUITE02(08){
    CLEANENV(POLICY_ATTR_EXAMPLE2);

    AttributeSet attrSet;

    std::string n1("id");
    BaseAttributePtr a1(new Attribute(&n1,
                                      Attribute::Match::Glob,
                                      Attribute::Type::Subject));
    attrSet.insert(a1);

    std::string n3("version");
    BaseAttributePtr a3(new Attribute(&n3,
                                      Attribute::Match::Glob,
                                      Attribute::Type::Subject));
    attrSet.insert(a3);


    std::string n2("resource-id");
    BaseAttributePtr a2(new Attribute(&n2,
                                      Attribute::Match::Glob,
                                      Attribute::Type::Resource));
    attrSet.insert(a2);

    std::string n4("author");
    BaseAttributePtr a4(new Attribute(&n4,
                                      Attribute::Match::Glob,
                                      Attribute::Type::Resource));
    attrSet.insert(a4);

    PEPTR->extractAttributesTest();

    bool result = AttrEqual(attrSet, PEPTR->getAttributeSet());
    RUNNER_ASSERT(result);
}

TESTSUITE02(09){
    CLEANENV(POLICY_ATTR_EXAMPLE2);

    AttributeSet attrSet;

    std::string n1("id");
    BaseAttributePtr a1(new Attribute(&n1,
                                      Attribute::Match::Regexp,
                                      Attribute::Type::Subject));
    attrSet.insert(a1);

    std::string n3("version");
    BaseAttributePtr a3(new Attribute(&n3,
                                      Attribute::Match::Regexp,
                                      Attribute::Type::Subject));
    attrSet.insert(a3);


    std::string n2("resource-id");
    BaseAttributePtr a2(new Attribute(&n2,
                                      Attribute::Match::Regexp,
                                      Attribute::Type::Resource));
    attrSet.insert(a2);

    std::string n4("author");
    BaseAttributePtr a4(new Attribute(&n4,
                                      Attribute::Match::Regexp,
                                      Attribute::Type::Resource));
    attrSet.insert(a4);

    PEPTR->extractAttributesTest();

    bool result = AttrEqual(attrSet, PEPTR->getAttributeSet());
    RUNNER_ASSERT(result);
}

TESTSUITE02(10){
    CLEANENV(POLICY_ATTR_EXAMPLE2);

    AttributeSet attrSet;

    std::string n1("id");
    BaseAttributePtr a1(new Attribute(&n1,
                                      Attribute::Match::Regexp,
                                      Attribute::Type::Subject));
    attrSet.insert(a1);

    std::string n3("version");
    BaseAttributePtr a3(new Attribute(&n3,
                                      Attribute::Match::Equal,
                                      Attribute::Type::Subject));
    attrSet.insert(a3);


    std::string n2("resource-id");
    BaseAttributePtr a2(new Attribute(&n2,
                                      Attribute::Match::Glob,
                                      Attribute::Type::Resource));
    attrSet.insert(a2);

    std::string n4("author");
    BaseAttributePtr a4(new Attribute(&n4,
                                      Attribute::Match::Regexp,
                                      Attribute::Type::Resource));
    attrSet.insert(a4);

    PEPTR->extractAttributesTest();

    bool result = AttrEqual(attrSet, PEPTR->getAttributeSet());
    RUNNER_ASSERT(result);
}

TESTSUITE02(11){
    CLEANENV(POLICY_ATTR_EXAMPLE7);

    AttributeSet attrSet;

    std::string n3("uri.host");
    BaseAttributePtr a3(new Attribute(&n3,
                                      Attribute::Match::Equal,
                                      Attribute::Type::Subject));
    attrSet.insert(a3);

    std::string n4("key-root-trust");
    BaseAttributePtr a4(new Attribute(&n4,
                                      Attribute::Match::Equal,
                                      Attribute::Type::Subject));
    attrSet.insert(a4);

    std::string n5("id");
    BaseAttributePtr a5(new Attribute(&n5,
                                      Attribute::Match::Equal,
                                      Attribute::Type::Subject));
    attrSet.insert(a5);

    std::string n6("signer-id");
    BaseAttributePtr a6(new Attribute(&n6,
                                      Attribute::Match::Equal,
                                      Attribute::Type::Subject));
    attrSet.insert(a6);

    std::string n7("version");
    BaseAttributePtr a7(new Attribute(&n7,
                                      Attribute::Match::Equal,
                                      Attribute::Type::Subject));
    attrSet.insert(a7);

    std::string n13("name");
    BaseAttributePtr a13(new Attribute(&n13,
                                       Attribute::Match::Equal,
                                       Attribute::Type::Subject));
    attrSet.insert(a13);

    std::string n14("resource-id");
    BaseAttributePtr a14(new Attribute(&n14,
                                       Attribute::Match::Equal,
                                       Attribute::Type::Subject));
    attrSet.insert(a14);

    std::string n15("r8v2");
    BaseAttributePtr a15(new Attribute(&n15,
                                       Attribute::Match::Equal,
                                       Attribute::Type::Subject));
    attrSet.insert(a15);

    std::string n16("author");
    BaseAttributePtr a16(new Attribute(&n16,
                                       Attribute::Match::Equal,
                                       Attribute::Type::Subject));
    attrSet.insert(a16);

    std::string n17("r9a.scheme");
    BaseAttributePtr a17(new Attribute(&n17,
                                       Attribute::Match::Equal,
                                       Attribute::Type::Subject));
    attrSet.insert(a17);

    std::string n18("r9b.authority");
    BaseAttributePtr a18(new Attribute(&n18,
                                       Attribute::Match::Equal,
                                       Attribute::Type::Subject));
    attrSet.insert(a18);

    std::string n19("r9c.scheme-authority");
    BaseAttributePtr a19(new Attribute(&n19,
                                       Attribute::Match::Equal,
                                       Attribute::Type::Subject));
    attrSet.insert(a19);

    std::string n20("resource-id");
    BaseAttributePtr a20(new Attribute(&n20,
                                       Attribute::Match::Equal,
                                       Attribute::Type::Resource));
    attrSet.insert(a20);

    std::string n8("r8v2");
    BaseAttributePtr a8(new Attribute(&n8,
                                      Attribute::Match::Equal,
                                      Attribute::Type::Resource));
    attrSet.insert(a8);

    std::string n9("author");
    BaseAttributePtr a9(new Attribute(&n9,
                                      Attribute::Match::Equal,
                                      Attribute::Type::Resource));
    attrSet.insert(a9);

    std::string n10("r9a.scheme");
    BaseAttributePtr a10(new Attribute(&n10,
                                       Attribute::Match::Equal,
                                       Attribute::Type::Resource));
    attrSet.insert(a10);

    std::string n11("r9b.authority");
    BaseAttributePtr a11(new Attribute(&n11,
                                       Attribute::Match::Equal,
                                       Attribute::Type::Resource));
    attrSet.insert(a11);

    std::string n12("r9c.scheme-authority");
    BaseAttributePtr a12(new Attribute(&n12,
                                       Attribute::Match::Equal,
                                       Attribute::Type::Resource));
    attrSet.insert(a12);

    std::string n21("resource-id");
    BaseAttributePtr a21(new Attribute(&n21,
                                       Attribute::Match::Equal,
                                       Attribute::Type::Environment));
    attrSet.insert(a21);

    std::string n22("r8v2");
    BaseAttributePtr a22(new Attribute(&n22,
                                       Attribute::Match::Equal,
                                       Attribute::Type::Environment));
    attrSet.insert(a22);

    std::string n23("author");
    BaseAttributePtr a23(new Attribute(&n23,
                                       Attribute::Match::Equal,
                                       Attribute::Type::Environment));
    attrSet.insert(a23);

    std::string n24("r9a.scheme");
    BaseAttributePtr a24(new Attribute(&n24,
                                       Attribute::Match::Equal,
                                       Attribute::Type::Environment));
    attrSet.insert(a24);

    std::string n25("r9b.authority");
    BaseAttributePtr a25(new Attribute(&n25,
                                       Attribute::Match::Equal,
                                       Attribute::Type::Environment));
    attrSet.insert(a25);

    std::string n26("r9c.scheme-authority");
    BaseAttributePtr a26(new Attribute(&n26,
                                       Attribute::Match::Equal,
                                       Attribute::Type::Environment));
    attrSet.insert(a26);

    PEPTR->extractAttributesTest();

    bool result = AttrEqual(attrSet, PEPTR->getAttributeSet());
    RUNNER_ASSERT(result);
}

TESTSUITE02(12){
    CLEANENV(POLICY_ATTR_EXAMPLE8);

    AttributeSet attrSet;

    std::string n1("id");
    BaseAttributePtr a1(new Attribute(&n1,
                                      Attribute::Match::Regexp,
                                      Attribute::Type::Subject));
    attrSet.insert(a1);

    std::string n3("version");
    BaseAttributePtr a3(new Attribute(&n3,
                                      Attribute::Match::Equal,
                                      Attribute::Type::Subject));
    attrSet.insert(a3);


    std::string n2("resource-id");
    BaseAttributePtr a2(new Attribute(&n2,
                                      Attribute::Match::Glob,
                                      Attribute::Type::Resource));
    attrSet.insert(a2);

    std::string n4("author");
    BaseAttributePtr a4(new Attribute(&n4,
                                      Attribute::Match::Regexp,
                                      Attribute::Type::Resource));
    attrSet.insert(a4);

    PEPTR->extractAttributesTest();

    bool result = AttrEqual(attrSet, PEPTR->getAttributeSet());
    RUNNER_ASSERT(result);
}

