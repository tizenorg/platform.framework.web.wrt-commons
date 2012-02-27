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
 * @file    TestSuite01.cpp
 * @author  unknown
 * @version 1.0
 * @brief   Test cases for PolicyEvaluator class.
 */
#include <string>

#include <dpl/log/log.h>
#include <dpl/test/test_runner.h>

#include <dpl/ace-dao-rw/AceDAO.h>
#include <dpl/ace/Preference.h>
#include <dpl/ace/SettingsLogic.h>
#include <dpl/ace/PolicyEvaluator.h>

#include "AttributeSetter.h"
#include "Interfaces.h"
#include "PEPSingleton.h"

#define GENERAL_TEST_POLICY "/usr/etc/ace/general-test.xml"
#define GENERAL_TEST_POLICY_UNDETERMIND "/usr/etc/ace/undefined-test.xml"
#define GENERAL_TEST_POLICY_GSETTINGS "/usr/etc/ace/policy-test-gsettings.xml"
#define GENERAL_TEST_POLICY_TEST "/usr/etc/ace/policy-test.xml"

#define CLEANENV(d)                                                         \
    do{                                                                     \
        if (PEPSingleton::Instance().getPdp()->getCurrentPolicy() != (d)) { \
            PEPSingleton::Instance().getPdp()->updatePolicy(d);             \
        }                                                                   \
        AttributeSetterSingleton::Instance().clear();                       \
        AceDB::AceDAO::clearWidgetDevCapSettings();                         \
        AceDB::AceDAO::clearDevCapSettings();                               \
    }while(0)

#define QU(nr, subid, resid)                                                        \
    do{                                                                             \
        AttributeSetterSingleton::Instance().addValue((nr), "id", (subid));         \
        AttributeSetterSingleton::Instance().addValue((nr), "resource-id", (resid));\
    }while(0)

#define QA(nr, attrname, attrvalue) \
    AttributeSetterSingleton::Instance().addValue((nr), (attrname), (attrvalue))

#define QC(nr, expect)                                                         \
    do{                                                                        \
        Request request((nr), WidgetExecutionPhase_Unknown);                   \
        std::string a =                                                        \
            AttributeSetterSingleton::Instance().getValue((nr),"resource-id"); \
        request.addDeviceCapability(a);                                        \
        PolicyResult result = PEPSingleton::Instance().check(request);         \
        RUNNER_ASSERT(result == (expect));                                     \
    }while(0)

#define QCP(nr, param, expect)                                                 \
    do{                                                                        \
        Request request((nr), WidgetExecutionPhase_Unknown, (param));          \
        std::string a =                                                        \
            AttributeSetterSingleton::Instance().getValue((nr),"resource-id"); \
        request.addDeviceCapability(a);                                        \
        PolicyResult result = PEPSingleton::Instance().check(request);         \
        RUNNER_ASSERT(result == (expect));                                     \
    }while(0)

#define TESTSUITE01(n) \
RUNNER_TEST(ts01_general_tests_ ## n)

TESTSUITE01(01){
    CLEANENV(GENERAL_TEST_POLICY);
    QU(1, "subject", "resource");
    QA(1, "version", "3");
    QC(1, PolicyEffect::PERMIT);
}

TESTSUITE01(02){
    CLEANENV(GENERAL_TEST_POLICY);
    QU(2, "subject2", "resource");
    QA(2, "version", "1");
    QC(2, PolicyEffect::DENY);
}

TESTSUITE01(03){
    CLEANENV(GENERAL_TEST_POLICY);
    QU(3, "subject3", "resource2");
    QC(3, PolicyEffect::DENY);
}

TESTSUITE01(04){
    CLEANENV(GENERAL_TEST_POLICY);
    QU(4, "subject4", "resource4");
    QA(4, "version", "4");
    QC(4, PolicyEffect::PERMIT);
}

TESTSUITE01(05){
    CLEANENV(GENERAL_TEST_POLICY);

    QU(5, "subject5", "resource5");
    QA(5, "version", "5");
    QC(5, PolicyEffect::PERMIT);

    QU(6, "subject5", "resource5");
    QA(6, "version", "1");
    QC(6, PolicyDecision::NOT_APPLICABLE);

    QU(7, "subject6", "resource6");
    QA(7, "version", "1");
    QC(7, PolicyEffect::PERMIT);
}

TESTSUITE01(06){
    CLEANENV(GENERAL_TEST_POLICY);

    QU(6, "subject7", "resource7");
    QA(6, "version", "7");
    QA(6, "author", "author");
    QC(6, PolicyDecision::NOT_APPLICABLE);

    QU(7, "subject7", "resource7");
    QA(7, "version", "7");
    QA(7, "author", "author2");
    QC(7, PolicyEffect::PERMIT);
}

TESTSUITE01(07){
    CLEANENV(GENERAL_TEST_POLICY);
    QU(3, "subject7", "resource7");
    QA(3, "version", "1");
    QA(3, "author", "author3");
    QC(3, PolicyEffect::DENY);
}

TESTSUITE01(08){
    CLEANENV(GENERAL_TEST_POLICY);

    QU(8, "s8a", "r8");
    QA(8, "r8v1", "1");
    QA(8, "r8v2", "2");
    QC(8, PolicyEffect::PERMIT);

    QU(9, "s8b", "r8");
    QA(9, "r8v1", "1");
    QA(9, "r8v2", "2");
    QC(9, PolicyEffect::DENY);

    QU(19, "s8c", "r8");
    QA(19, "r8v1", "1");
    QA(19, "r8v2", "2");
    QC(19, PolicyDecision::NOT_APPLICABLE);
}

TESTSUITE01(09){
    CLEANENV(GENERAL_TEST_POLICY);

    QU(1, "s9a", "r9");
    QA(1, "r9a", "http://onet.pl:90/test.html");
    QA(1, "r9b", "http://onet.pl:80/test.html");
    QA(1, "r9c", "http://onet.pl:80");
    QA(1, "r9d", "http://onet.pl:80");
    QA(1, "r9e", "http://onet.pl:80/test.html");
    QA(1, "r9g", "http://onet.pl:80/test.html");
    QC(1, PolicyEffect::PERMIT);

    QU(2, "s9b", "r9");
    QA(2, "r9a", "http://onet.pl:90/test.html");
    QA(2, "r9b", "http://onet.pl:80/test.html");
    QA(2, "r9c", "http://onet.pl:80");
    QA(2, "r9d", "http://onet.pl:80");
    QA(2, "r9e", "http://onet.pl:80/test.html");
    QA(2, "r9g", "http://onet.pl:80/test.html");
    QC(2, PolicyEffect::DENY);

    QU(3, "s9c", "r9");
    QA(3, "r9a", "http://onet.pl:90/test.html");
    QA(3, "r9b", "http://onet.pl:80/test.html");
    QA(3, "r9c", "http://onet.pl:80");
    QA(3, "r9d", "http://onet.pl:80");
    QA(3, "r9e", "http://onet.pl:80/test.html");
    QA(3, "r9g", "http://onet.pl:80/test.html");
    QC(3, PolicyEffect::PERMIT);

    QU(4, "s9d", "r9");
    QA(4, "r9a", "http://onet.pl:90/test.html");
    QA(4, "r9b", "http://onet.pl:80/test.html");
    QA(4, "r9c", "http://onet.pl:80");
    QA(4, "r9d", "http://onet.pl:80");
    QA(4, "r9e", "http://onet.pl:80/test.html");
    QA(4, "r9g", "http://onet.pl:80/test.html");
    QC(4, PolicyEffect::DENY);

    QU(5, "s9e", "r9");
    QA(5, "r9a", "http://onet.pl:90/test.html");
    QA(5, "r9b", "http://onet.pl:80/test.html");
    QA(5, "r9c", "http://onet.pl:80");
    QA(5, "r9d", "http://onet.pl:80");
    QA(5, "r9e", "http://onet.pl:80/test.html");
    QA(5, "r9g", "http://onet.pl:80/test.html");
    QC(5, PolicyEffect::DENY);

    QU(6, "s9f", "r9");
    QA(6, "r9a", "http://onet.pl:90/test.html");
    QA(6, "r9b", "http://onet.pl:80/test.html");
    QA(6, "r9c", "http://onet.pl:80");
    QA(6, "r9d", "http://onet.pl:80");
    QA(6, "r9e", "http://onet.pl:80/test.html");
    QA(6, "r9g", "http://onet.pl:80/test.html");
    QC(6, PolicyEffect::PERMIT);

    QU(7, "s9g", "r9");
    QA(7, "r9a", "http://onet.pl:90/test.html");
    QA(7, "r9b", "http://onet.pl:80/test.html");
    QA(7, "r9c", "http://onet.pl:80");
    QA(7, "r9d", "http://onet.pl:80");
    QA(7, "r9e", "http://onet.pl:80/test.html");
    QA(7, "r9g", "http://onet.pl:80/test.html");
    QC(7, PolicyEffect::DENY);

    QU(8, "s9h", "r9");
    QA(8, "r9a", "http://onet.pl:90/test.html");
    QA(8, "r9b", "http://onet.pl:80/test.html");
    QA(8, "r9c", "http://onet.pl:80");
    QA(8, "r9d", "http://onet.pl:80");
    QA(8, "r9e", "http://onet.pl:80/test.html");
    QA(8, "r9g", "http://onet.pl:80/test.html");
    QC(8, PolicyEffect::PERMIT);
}

TESTSUITE01(10){
    CLEANENV(GENERAL_TEST_POLICY);

    QU(1, "s10a", "r10");
    QA(1, "r10a", "15ba");
    QA(1, "r10c", "15a1");
    QC(1, PolicyEffect::PERMIT);

    QU(2, "s10b", "r10");
    QA(2, "r10a", "15ba");
    QA(2, "r10c", "15a1");
    QC(2, PolicyEffect::DENY);

    QU(3, "s10c", "r10");
    QA(3, "r10a", "15ba");
    QA(3, "r10c", "15a1");
    QC(3, PolicyResult::UNDETERMINED);
}

TESTSUITE01(11){
    CLEANENV(GENERAL_TEST_POLICY);

    QU(1, "s15", "device:pim.contacts");
    QA(1, "uri", "//buu.com.pl");
    QC(1, PolicyEffect::PERMIT);
}

TESTSUITE01(12){
    CLEANENV(GENERAL_TEST_POLICY);

    QU(1, "s16", "device:pim.contacts");
    QA(1, "uri", "//v.com.pl");
    QC(1, PolicyEffect::PERMIT);
}

TESTSUITE01(13){
    CLEANENV(GENERAL_TEST_POLICY);

    QU(1, "s17a", "resource4");
    QA(1, "version", "4");
    QC(1, PolicyEffect::PERMIT);

    QU(2, "s17b", "resource4");
    QA(2, "version", "4");
    QC(2, PolicyEffect::DENY);
}

TESTSUITE01(14){
    CLEANENV(GENERAL_TEST_POLICY);

    QU(1, "s18a", "device:pim.contacts");
    QA(1, "uri", "buu.com.pl");
    QC(1, PolicyEffect::PERMIT);

    QU(2, "s18b", "device:pim.contacts");
    QA(2, "uri", "buu.com.pl");
    QC(2, PolicyEffect::PERMIT);

    QU(3, "s18c", "device:pim.contacts");
    QA(3, "uri", "buu.com.pl");
    QC(3, PolicyEffect::PERMIT);
}

TESTSUITE01(15){
    CLEANENV(GENERAL_TEST_POLICY);

    QU(1, "s19.1", "resource4");
    QA(1, "key-root-trust", "voperator");
    QC(1, PolicyEffect::DENY);

    QU(2, "s19.2", "resource4");
    QA(2, "key-root-trust", "voperator");
    QC(2, PolicyEffect::PERMIT);
}

TESTSUITE01(16){
    CLEANENV(GENERAL_TEST_POLICY);

    QU(1, "s20.1", "resource4");
    QA(1, "signer-id", "v7zha89%49x£$");
    QC(1, PolicyEffect::DENY);

    QU(2, "s20.2", "resource4");
    QA(2, "signer-id", "v7zha89%49x£$");
    QC(2, PolicyEffect::PERMIT);
}

TESTSUITE01(17){
    CLEANENV(GENERAL_TEST_POLICY);

    QU(1, "s21", "undetermined");
    QC(1, PolicyDecision::NOT_APPLICABLE);


    QU(2, "s21a", "undetermined");
    QC(2, PolicyEffect::PERMIT);
}

TESTSUITE01(18){
    CLEANENV(GENERAL_TEST_POLICY);
    QU(1, "s23", "device:pim.contacts");
    QA(1, "version", "undetermined");
    QC(1, PolicyDecision::NOT_APPLICABLE);
}

TESTSUITE01(19){
    CLEANENV(GENERAL_TEST_POLICY);
    QU(1, "s24", "device:pim");
    QC(1, PolicyEffect::PERMIT);
}

TESTSUITE01(20){
    CLEANENV(GENERAL_TEST_POLICY);
    QU(1, "s25.1", "device:pim.contacts");
    QA(1, "version", "5");
    QA(1, "roaming", "off");
    QC(1, PolicyEffect::PERMIT);
}

TESTSUITE01(21){
    CLEANENV(GENERAL_TEST_POLICY);
    QU(1, "s25.2", "device:pim.contacts");
    QA(1, "version", "undetermined");
    QC(1, PolicyDecision::NOT_APPLICABLE);
}

TESTSUITE01(22){
    CLEANENV(GENERAL_TEST_POLICY_UNDETERMIND);
    QU(1, "s25.3", "device:pim.contacts");
// undetermined    QA(1, "version", "");
    QC(1, PolicyResult::UNDETERMINED);
}

TESTSUITE01(23){
    CLEANENV(GENERAL_TEST_POLICY_UNDETERMIND);
    QU(1, "s25.4", "device:pim.contacts");
    //undetermined
    //QA(1, "version", "", true);
    QC(1, PolicyResult::UNDETERMINED);
}

TESTSUITE01(24){
    CLEANENV(GENERAL_TEST_POLICY_UNDETERMIND);
    QU(1, "s26.1", "device:pim.contacts");
    QA(1, "version", "5");
    QC(1, PolicyEffect::PERMIT);
}

TESTSUITE01(25){
    CLEANENV(GENERAL_TEST_POLICY_UNDETERMIND);
    QU(1, "s26.2", "device:pim.contacts");
    //undetermined
    //QA(1, "version", "", true);
    QC(1,  PolicyEffect::PERMIT);
}

TESTSUITE01(26){
    CLEANENV(GENERAL_TEST_POLICY_UNDETERMIND);
    QU(1, "s26.3", "device:pim.contacts");
    //undetermined
    //QA(1, "version", "", true);
    QC(1,  PolicyEffect::PERMIT);
}

TESTSUITE01(27){
    CLEANENV(GENERAL_TEST_POLICY_UNDETERMIND);
    QU(1, "s26.4", "device:pim.contacts");
    //undetermined
    //QA(1, "version", "undetermined", true);
    QC(1, PolicyResult::UNDETERMINED);
}

TESTSUITE01(28){
    CLEANENV(GENERAL_TEST_POLICY_UNDETERMIND);
    QU(1, "s27.1", "device:pim.contacts");
    //undetermined
    //QA(1, "version", "undetermined", true);
    QC(1, PolicyEffect::PERMIT);
}

TESTSUITE01(29){
    CLEANENV(GENERAL_TEST_POLICY_UNDETERMIND);
    //Test undetermined, we have to refine this tests to test something
    QU(1, "s27.2", "device:pim.contacts");
    //undetermined
    //QA(1, "version", "undetermined", true);
    QC(1, PolicyResult::UNDETERMINED);
}

TESTSUITE01(30){
    CLEANENV(GENERAL_TEST_POLICY_UNDETERMIND);
    //Test undetermined, we have to refine this tests to test something
    QU(1, "s27.3", "device:pim.contacts");
    //undetermined
    //QA(1, "version", "undetermined", true);
    QC(1, PolicyDecision::NOT_APPLICABLE);
}

TESTSUITE01(31){
    CLEANENV(GENERAL_TEST_POLICY_UNDETERMIND);
    //Test undetermined, we have to refine this tests to test something
    QU(1, "s27.4", "device:pim.contacts");
    //undetermined
    //QA(1, "version", "undetermined", true);
    QC(1, PolicyEffect::PERMIT);
}

TESTSUITE01(32){
    CLEANENV(GENERAL_TEST_POLICY_UNDETERMIND);
    //Test undetermined, we have to refine this tests to test something
    QU(1, "s28", "device:pim.contacts");
    //undetermined
    //QA(1, "version", "undetermined", true);
    QC(1, PolicyEffect::PERMIT);
}

TESTSUITE01(33){
    CLEANENV(GENERAL_TEST_POLICY_UNDETERMIND);
    //Test undetermined, we have to refine this tests to test something
    QU(1, "s29", "device:pim.contacts");
    //undetermined
    //QA(1, "version", "undetermined", true);
    QC(1, PolicyEffect::PERMIT);
}

TESTSUITE01(34){
    CLEANENV(GENERAL_TEST_POLICY_UNDETERMIND);
    //Test undetermined, we have to refine this tests to test something
    QU(1, "s30.1", "device:pim.contacts");
    //undetermined
    //QA(1, "version", "undetermined", true);
    QC(1, PolicyResult::UNDETERMINED);
}

TESTSUITE01(35){
    CLEANENV(GENERAL_TEST_POLICY_UNDETERMIND);
    //Test undetermined, we have to refine this tests to test something
    QU(1, "s30.2", "device:pim.contacts");
    //undetermined
    //QA(1, "version", "undetermined", true);
    QC(1, PolicyResult::UNDETERMINED);
}

TESTSUITE01(36){
    CLEANENV(GENERAL_TEST_POLICY_UNDETERMIND);
    //Test undetermined, we have to refine this tests to test something
    QU(1, "s30.3", "device:pim.contacts");
    //undetermined
    //QA(1, "version", "undetermined", true);
    QC(1, PolicyResult::UNDETERMINED);
}

TESTSUITE01(37){
    CLEANENV(GENERAL_TEST_POLICY_UNDETERMIND);
    //Test undetermined, we have to refine this tests to test something
    QU(1, "s30.4", "device:pim.contacts");
    //undetermined
    //QA(1, "version", "undetermined", true);
    QC(1, PolicyEffect::PERMIT);
}

TESTSUITE01(38){
    CLEANENV(GENERAL_TEST_POLICY_UNDETERMIND);
    //Test undetermined, we have to refine this tests to test something
    QU(1, "s31.1", "device:pim.contacts");
    //undetermined
    //QA(1, "version", "undetermined", true);
    QC(1, PolicyResult::UNDETERMINED);
}

TESTSUITE01(39){
    CLEANENV(GENERAL_TEST_POLICY_UNDETERMIND);
    //Test undetermined, we have to refine this tests to test something
    //undetermined
    //QA(1, "version", "undetermined", true);
    QU(1, "s31.2.1", "device:pim.contacts");
    QC(1, PolicyEffect::DENY);
}

TESTSUITE01(40){
    CLEANENV(GENERAL_TEST_POLICY_UNDETERMIND);
    //Test undetermined, we have to refine this tests to test something
    //undetermined
    //QA(1, "version", "undetermined", true);
    QU(1, "s31.2.2", "device:pim.contacts");
    QC(1, PolicyResult::UNDETERMINED);
}

TESTSUITE01(41){
    CLEANENV(GENERAL_TEST_POLICY_UNDETERMIND);
    //Test undetermined, we have to refine this tests to test something
    //undetermined
    //QA(1, "version", "undetermined", true);
    QU(1, "s31.3", "device:pim.contacts");
    QC(1, PolicyResult::UNDETERMINED);
}

TESTSUITE01(42){
    CLEANENV(GENERAL_TEST_POLICY_UNDETERMIND);
    //Test undetermined, we have to refine this tests to test something
    //undetermined
    //QA(1, "version", "undetermined", true);
    QU(1, "s32.1", "device:pim.contacts");
    QC(1, PolicyResult::UNDETERMINED);
}

TESTSUITE01(43){
    CLEANENV(GENERAL_TEST_POLICY_UNDETERMIND);
    //Test undetermined, we have to refine this tests to test something
    //undetermined
    //QA(1, "version", "undetermined", true);
    QU(1, "s32.2.1", "device:pim.contacts");
    QC(1, PolicyEffect::PERMIT);
}

TESTSUITE01(44){
    CLEANENV(GENERAL_TEST_POLICY_UNDETERMIND);
    //Test undetermined, we have to refine this tests to test something
    //undetermined
    //QA(1, "version", "undetermined", true);
    QU(1, "s32.2.2", "device:pim.contacts");
    QC(1, PolicyResult::UNDETERMINED);
}

TESTSUITE01(45){
    CLEANENV(GENERAL_TEST_POLICY_UNDETERMIND);
    //Test undetermined, we have to refine this tests to test something
    //undetermined
    //QA(1, "version", "undetermined", true);
    QU(1, "s32.3", "device:pim.contacts");
    QC(1, PolicyResult::UNDETERMINED);
}

TESTSUITE01(46){
    CLEANENV(GENERAL_TEST_POLICY_UNDETERMIND);
    //Test undetermined, we have to refine this tests to test something
    //undetermined
    //QA(1, "version", "undetermined", true);
    QU(1, "s33.1", "device:pim.contacts");
    QC(1, PolicyResult::UNDETERMINED);
}

TESTSUITE01(47){
    CLEANENV(GENERAL_TEST_POLICY_UNDETERMIND);
    //Test undetermined, we have to refine this tests to test something
    //undetermined
    //QA(1, "version", "undetermined", true);
    QU(1, "s33.2", "device:pim.contacts");
    QC(1, PolicyEffect::PERMIT);
}

TESTSUITE01(48){
    CLEANENV(GENERAL_TEST_POLICY_UNDETERMIND);
    //Test undetermined, we have to refine this tests to test something
    //undetermined
    //QA(1, "version", "undetermined", true);
    QU(1, "s33.3", "device:pim.contacts");
    QC(1, PolicyEffect::PERMIT);
}

TESTSUITE01(49){
    CLEANENV(GENERAL_TEST_POLICY_UNDETERMIND);
    //Test undetermined, we have to refine this tests to test something
    //undetermined
    //QA(1, "version", "undetermined", true);
    QU(1, "s34.1", "device:pim.contacts");
    QC(1, PolicyResult::UNDETERMINED);
}

TESTSUITE01(50){
    CLEANENV(GENERAL_TEST_POLICY_UNDETERMIND);
    //undetermined
    //QA(1, "version", "undetermined", true);
    QU(1, "s34.2", "device:pim.contacts");
    QC(1, PolicyResult::UNDETERMINED);
}

TESTSUITE01(51){
    CLEANENV(GENERAL_TEST_POLICY_UNDETERMIND);
    //undetermined
    //QA(1, "version", "undetermined", true);
    QU(1, "s35.1", "device:pim.contacts");
    QC(1, PolicyResult::UNDETERMINED);
}

TESTSUITE01(52){
    CLEANENV(GENERAL_TEST_POLICY_UNDETERMIND);
    //undetermined
    //QA(1, "version", "undetermined", true);
    QU(1, "s35.2", "device:pim.contacts");
    QC(1, PolicyResult::UNDETERMINED);
}

TESTSUITE01(53){
    CLEANENV(GENERAL_TEST_POLICY_UNDETERMIND);
    QU(1, "s36", "device:pim.contacts");
    //undetermined
    //QA(1, "version", "undetermined", true);
    QC(1, PolicyDecision::NOT_APPLICABLE);
}

TESTSUITE01(54){
    CLEANENV(GENERAL_TEST_POLICY);
    QU(1, "s37", "device:pim.contacts.read");
    QC(1, PolicyEffect::PROMPT_ONESHOT);
}

TESTSUITE01(55){
    CLEANENV(GENERAL_TEST_POLICY);

    QU(1, "s38", "device:pim.contacts.read");
    QC(1, PolicyEffect::PROMPT_SESSION);

    QU(2, "s38.4", "device:pim.contacts.read");
    QC(2, PolicyEffect::PROMPT_SESSION);
}

TESTSUITE01(56){
    CLEANENV(GENERAL_TEST_POLICY);

    QU(1, "s39", "device:pim.contacts.read");
    QC(1, PolicyEffect::PROMPT_BLANKET);

    QU(2, "s39.4", "device:pim.contacts.read");
    QC(2, PolicyEffect::PROMPT_BLANKET);
}

TESTSUITE01(57){
    CLEANENV(GENERAL_TEST_POLICY);
    QU(1, "s40", "r40");
    QC(1, PolicyEffect::PERMIT);
}

TESTSUITE01(58){
    CLEANENV(GENERAL_TEST_POLICY);
    QU(1, "s41", "r41");
    QC(1, PolicyEffect::PERMIT);
}

TESTSUITE01(59){
    CLEANENV(GENERAL_TEST_POLICY);
    QU(1, "s42.1", "r42.1");
    QC(1, PolicyEffect::DENY);
}

TESTSUITE01(60){
    CLEANENV(GENERAL_TEST_POLICY);
    QU(1, "s42.2", "r42.2");
    QC(1, PolicyEffect::DENY);

}

TESTSUITE01(61){
    CLEANENV(GENERAL_TEST_POLICY);
    QU(1, "s43.1", "r43.1");
    QC(1, PolicyEffect::DENY);
}

TESTSUITE01(62){
    CLEANENV(GENERAL_TEST_POLICY);
    QU(1, "s43.2", "r43.2");
    QC(1,  PolicyEffect::DENY);
}

TESTSUITE01(63){
    CLEANENV(GENERAL_TEST_POLICY);
    QU(1, "s44.1", "r44.1");
    QC(1, PolicyEffect::PERMIT);
}

TESTSUITE01(64){
    CLEANENV(GENERAL_TEST_POLICY);
    QU(1, "s44.2", "r44.2");
    QC(1, PolicyEffect::PERMIT);
}

TESTSUITE01(65){
    CLEANENV(GENERAL_TEST_POLICY);
    QU(1, "s45.1", "r45.1");
    QC(1,  PolicyEffect::PERMIT);
}

TESTSUITE01(66){
    CLEANENV(GENERAL_TEST_POLICY);
    QU(1, "s45.2", "r45.2");
    QC(1, PolicyEffect::DENY);
}

TESTSUITE01(67){
    CLEANENV(GENERAL_TEST_POLICY);
    QU(1, "s46.1", "r46.1");
    QC(1,  PolicyEffect::PERMIT);
}

TESTSUITE01(68){
    CLEANENV(GENERAL_TEST_POLICY);
    QU(1, "s46.2", "r46.2");
    QC(1, PolicyEffect::PERMIT);
}

TESTSUITE01(69){
    CLEANENV(GENERAL_TEST_POLICY);
    QU(1, "s47.1", "r47.1");
    QC(1, PolicyEffect::DENY);
}

TESTSUITE01(70){
    CLEANENV(GENERAL_TEST_POLICY);
    QU(1, "s47.2", "r47.2");
    QC(1,  PolicyEffect::DENY);
}

TESTSUITE01(71){
    CLEANENV(GENERAL_TEST_POLICY);
    QU(1, "s48", "device:pim.contacts");
    QA(1, "uri", "http://www.test.pl:80");
    QC(1,  PolicyEffect::PERMIT);
}

TESTSUITE01(72){
    CLEANENV(GENERAL_TEST_POLICY);
    QU(1, "BF00", "BFR00");
    QC(1, PolicyEffect::DENY);
}

TESTSUITE01(73){
    CLEANENV(GENERAL_TEST_POLICY);
    QU(1, "BF01", "BFR01");
    QC(1, PolicyEffect::PERMIT);
}

TESTSUITE01(74){
    CLEANENV(GENERAL_TEST_POLICY);
    QU(1, "BF02", "BFR02");
    QC(1, PolicyEffect::PERMIT);
}

TESTSUITE01(75){
    CLEANENV(GENERAL_TEST_POLICY);
    QU(1, "BF03", "BFR03");
    QC(1,  PolicyEffect::DENY);
}

TESTSUITE01(76){
    CLEANENV(GENERAL_TEST_POLICY);
    QU(1, "BF04", "BFR04");
    QC(1, PolicyEffect::PERMIT);
}

TESTSUITE01(77){
    CLEANENV(GENERAL_TEST_POLICY);
    FunctionParamImpl functionParam;
    functionParam.addAttribute("param:recipients","+44091234");
    QU(1, "paramTestSubject", "resource");
    QA(1, "dev-cap","messaging");
    QCP(1, &functionParam, PolicyEffect::DENY);
}

TESTSUITE01(78){
    CLEANENV(GENERAL_TEST_POLICY);
    FunctionParamImpl functionParam;
    functionParam.addAttribute("param:recipients","+44081234");
    QU(1, "paramTestSubject", "resource");
    QA(1, "dev-cap","messaging");
    QCP(1, &functionParam, PolicyEffect::PERMIT);
}

TESTSUITE01(79){
    CLEANENV(GENERAL_TEST_POLICY);
    FunctionParamImpl functionParam;
    functionParam.addAttribute("param:recipients","+4812345656");
    QU(1, "paramTestSubject", "resource");
    QA(1, "dev-cap","messaging");
    QCP(1, &functionParam, PolicyEffect::PERMIT);
}

TESTSUITE01(80){
    CLEANENV(GENERAL_TEST_POLICY);
    FunctionParamImpl functionParam;
    functionParam.addAttribute("param:quality","low");
    QU(1, "paramTestSubject", "resource");
    QA(1, "dev-cap","messaging");
    QCP(1, &functionParam, PolicyDecision::NOT_APPLICABLE);
}

TESTSUITE01(81){
    CLEANENV(GENERAL_TEST_POLICY);
    FunctionParamImpl functionParam;
    functionParam.addAttribute("param:quality", "high");
    QU(1, "paramTestSubject", "resource");
    QA(1, "dev-cap","camera");
    QCP(1, &functionParam, PolicyEffect::PERMIT);
}

TESTSUITE01(82){
    CLEANENV(GENERAL_TEST_POLICY);

    QU(1, "s61a", "r61a");
    QC(1, PolicyEffect::DENY);

    FunctionParamImpl functionParam;
    functionParam.addAttribute("param:name", "type");
    QU(2, "s61b", "r61b");
    QCP(2, &functionParam, PolicyEffect::PERMIT);

    QU(3, "s61c", "r61c");
    QC(3,  PolicyEffect::DENY);

    FunctionParamImpl functionParam4;
    functionParam4.addAttribute("param:name", "type");
    functionParam4.addAttribute("param:name", "port");
    QU(4, "s61d", "r61d");
    QCP(4, &functionParam4, PolicyEffect::PERMIT);
}

TESTSUITE01(83){
    CLEANENV(GENERAL_TEST_POLICY_TEST);

    PermissionList sList;
    sList.push_back(PermissionTriple(1, "b1", Preference::PREFERENCE_PERMIT));
    sList.push_back(PermissionTriple(2, "bb2", Preference::PREFERENCE_DENY));
    sList.push_back(PermissionTriple(3, "d3", Preference::PREFERENCE_DEFAULT));
    SettingsLogic::setWidgetDevCapSettings(sList);

    QU(1, "a1", "b1");
    QC(1,  PolicyEffect::DENY);

    QU(2, "aa2", "bb2");
    QC(2, PolicyEffect::DENY);

    QU(3, "c3", "d3");
    QC(3, PolicyEffect::PERMIT);
}

TESTSUITE01(84){
    CLEANENV(GENERAL_TEST_POLICY_TEST);
    PermissionList sList;
    sList.push_back(PermissionTriple(3, "d3", Preference::PREFERENCE_ONE_SHOT_PROMPT));
    SettingsLogic::setWidgetDevCapSettings(sList);
    QU(3, "c3", "d3");
    QC(3, PolicyEffect::PROMPT_ONESHOT);
}

TESTSUITE01(85){
    CLEANENV(GENERAL_TEST_POLICY_TEST);

    PermissionList sList;
    sList.push_back(PermissionTriple(1, "bnp", Preference::PREFERENCE_DEFAULT));
    sList.push_back(PermissionTriple(2, "bbnp", Preference::PREFERENCE_DENY));
    sList.push_back(PermissionTriple(3, "dnp", Preference::PREFERENCE_PERMIT));

    SettingsLogic::setWidgetDevCapSettings(sList);

    QU(1, "anp", "bnp");
    QC(1, PolicyDecision::NOT_APPLICABLE);

    QU(2, "aanp", "bbnp");
    QC(2, PolicyEffect::DENY);

    QU(3, "cnp", "dnp");
    QC(3, PolicyEffect::PERMIT);
}

TESTSUITE01(86){
    CLEANENV(GENERAL_TEST_POLICY_TEST);

    PermissionList sList;
    sList.push_back(PermissionTriple(1, "b1", Preference::PREFERENCE_DEFAULT));
    sList.push_back(PermissionTriple(2, "bb", Preference::PREFERENCE_DENY));
    sList.push_back(PermissionTriple(3, "d3", Preference::PREFERENCE_PERMIT));
    sList.push_back(PermissionTriple(4, "b1", Preference::PREFERENCE_PERMIT));
    sList.push_back(PermissionTriple(5, "bb2", Preference::PREFERENCE_DENY));
    sList.push_back(PermissionTriple(6, "d3", Preference::PREFERENCE_DEFAULT));

    SettingsLogic::setWidgetDevCapSettings(sList);

    QU(1, "a1", "b1");
    QC(1, PolicyEffect::DENY);

    QU(2, "aa", "bb");
    QC(2, PolicyEffect::DENY);

    QU(3, "c3", "d3");
    QC(3, PolicyEffect::PERMIT);

    QU(5, "aa2", "bb2");
    QC(5, PolicyEffect::DENY);
}

TESTSUITE01(87){
    CLEANENV(GENERAL_TEST_POLICY_TEST);

    QU(1, "c3", "d3");
    QC(1, PolicyEffect::PERMIT);

    QU(2, "aa2", "bb2");
    QC(2, PolicyEffect::PERMIT);

    SettingsLogic::updateDevCapSetting("d3", Preference::PREFERENCE_DENY);
    SettingsLogic::updateDevCapSetting("bb2", Preference::PREFERENCE_DEFAULT);

    QC(1, PolicyEffect::DENY);

    QC(2, PolicyEffect::PERMIT);

    SettingsLogic::updateDevCapSetting("d3", Preference::PREFERENCE_PERMIT);

    QC(1, PolicyEffect::PERMIT);
}

TESTSUITE01(88){
    CLEANENV(GENERAL_TEST_POLICY_GSETTINGS);
    std::string d3("d3");

    QU(1, "c3", "d3");
    QC(1, PolicyEffect::PERMIT);

    QU(2, "aa2", "bb2");
    QC(2, PolicyEffect::PERMIT);

    QU(3, "a1", "d3");
    QC(3, PolicyEffect::DENY);

    SettingsLogic::updateDevCapSetting(d3, Preference::PREFERENCE_DEFAULT);

    QC(1, PolicyEffect::PERMIT);

    QC(2, PolicyEffect::PERMIT);

    QC(3, PolicyEffect::DENY);

    SettingsLogic::updateDevCapSetting(d3, Preference::PREFERENCE_DENY);

    QC(1, PolicyEffect::DENY);

    QC(3, PolicyEffect::DENY);

    SettingsLogic::updateDevCapSetting(d3, Preference::PREFERENCE_PERMIT);

    QC(1, PolicyEffect::PERMIT);

    QC(3, PolicyEffect::DENY);
}

